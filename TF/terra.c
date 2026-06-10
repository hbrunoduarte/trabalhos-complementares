#include "solar.h"
#include "terra.h"

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"layout (location = 3) in vec3 aTangent;\n" // Recebe a Tangente do C
"\n"
"out vec3 FragPos;\n"
"out vec2 TexCoord;\n"
"out mat3 TBN;\n" // Matriz de Rotação do Relevo
"\n"
"uniform mat4 modelView;\n"
"uniform mat4 projection;\n"
"\n"
"void main() {\n"
"   FragPos = vec3(modelView * vec4(aPos, 1.0));\n"
"   TexCoord = aTexCoord;\n"
"   \n"
"   // Transforma a Normal e a Tangente para o ponto de vista da câmera\n"
"   vec3 T = normalize(mat3(modelView) * aTangent);\n"
"   vec3 N = normalize(mat3(modelView) * aNormal);\n"
"   \n"
"   // Garante que elas estão perfeitamente a 90 graus uma da outra (Gram-Schmidt)\n"
"   T = normalize(T - dot(T, N) * N);\n"
"   \n"
"   // Calcula o terceiro eixo (Bitangente) e monta a matriz\n"
"   vec3 B = cross(N, T);\n"
"   TBN = mat3(T, B, N);\n"
"   \n"
"   gl_Position = projection * modelView * vec4(aPos, 1.0);\n"
"}\n";

// ==========================================
// CÓDIGO DO FRAGMENT SHADER (Mistura das 4 texturas)
// ==========================================
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 FragPos;\n"
"in vec2 TexCoord;\n"
"in mat3 TBN;\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D texDia;\n"
"uniform sampler2D texNoite;\n"
"uniform sampler2D texNuvens;\n"
"uniform sampler2D texNormal;\n" // Textura roxa/azul das montanhas
"uniform vec3 lightPosView;\n"
"\n"
"void main() {\n"
"   // 1. LÊ O RELEVO E DISTORCE A LUZ\n"
"   vec3 normalTextura = texture(texNormal, TexCoord).rgb;\n"
"   normalTextura = normalTextura * 2.0 - 1.0; // Converte de cor (0 a 1) para vetor (-1 a 1)\n"
"   vec3 norm = normalize(TBN * normalTextura); // Montanha distorce a normal da esfera!\n"
"   \n"
"   vec3 lightDir = normalize(lightPosView - FragPos);\n"
"   float impactoLuz = dot(norm, lightDir);\n" // Pode ser negativo (Noite)
"   \n"
"   // O smoothstep cria a penumbra do crepúsculo\n"
"   float mixDiaNoite = smoothstep(-0.1, 0.2, impactoLuz);\n"
"   \n"
"   // 2. LÊ AS OUTRAS IMAGENS\n"
"   vec4 corDia = texture(texDia, TexCoord);\n"
"   vec4 corNoite = texture(texNoite, TexCoord);\n"
"   vec4 corNuvens = texture(texNuvens, TexCoord);\n"
"   \n"
"   // 3. MISTURA TUDO\n"
"   // Nuvens fazem sombra sobre o dia\n"
"   vec3 diaComNuvens = mix(corDia.rgb, vec3(1.0), corNuvens.r);\n"
"   \n"
"   // Nuvens escondem as luzes das cidades\n"
"   vec3 noiteComNuvens = mix(corNoite.rgb, vec3(0.0), corNuvens.r);\n"
"   \n"
"   // Escolhe entre o Dia ou a Noite baseado na luz e no relevo\n"
"   vec3 corFinal = mix(noiteComNuvens, diaComNuvens, mixDiaNoite);\n"
"   \n"
"   FragColor = vec4(corFinal, 1.0);\n"
"}\n";

const char* vertexShaderNuvens = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec2 TexCoord;\n"
"\n"
"uniform mat4 modelView;\n"
"uniform mat4 projection;\n"
"\n"
"void main() {\n"
"   FragPos = vec3(modelView * vec4(aPos, 1.0));\n"
"   Normal = mat3(transpose(inverse(modelView))) * aNormal;\n"
"   TexCoord = aTexCoord;\n"
"   gl_Position = projection * modelView * vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderNuvens = "#version 330 core\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D texNuvens;\n"
"uniform vec3 lightPosView;\n"
"uniform float tempo;\n" // NOVO: O relógio do jogo
"\n"
"void main() {\n"
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPosView - FragPos);\n"
"   float impactoLuz = max(dot(norm, lightDir), 0.0);\n"
"   \n"
"   // NOVO: Desliza a textura suavemente para o lado\n"
"   vec2 uvAnimada = TexCoord + vec2(tempo * 0.02, 0.0);\n"
"   \n"
"   // Lê a textura usando o UV animado!\n"
"   vec4 corNuvem = texture(texNuvens, uvAnimada);\n"
"   \n"
"   vec3 corFinal = vec3(1.0) * impactoLuz;\n"
"   float alpha = corNuvem.r;\n"
"   \n"
"   FragColor = vec4(corFinal, alpha);\n"
"}\n";

GLint shaderTerraProgram = -1;
GLint shaderNuvemProgram = -1;
GLint idTerraDia = -1;
GLint idTerraNoite = -1;
GLint idNuvens = -1;
GLint idNormal = -1;

void compilarShaderTerra() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderTerraProgram = glCreateProgram();
    glAttachShader(shaderTerraProgram, vertexShader);
    glAttachShader(shaderTerraProgram, fragmentShader);
    glLinkProgram(shaderTerraProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void compilarShaderNuvens() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderNuvens, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderNuvens, NULL);
    glCompileShader(fragmentShader);

    shaderNuvemProgram = glCreateProgram();
    glAttachShader(shaderNuvemProgram, vertexShader);
    glAttachShader(shaderNuvemProgram, fragmentShader);
    glLinkProgram(shaderNuvemProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void carregarTexturaTerra() {
    compilarShaderTerra();
    compilarShaderNuvens();
    idTerraDia = carregarTextura("imagens/terra/earth.jpg");
    idTerraNoite = carregarTextura("imagens/terra/earthNight.jpg");
    idNuvens = carregarTextura("imagens/terra/earthClouds.jpg");
    idNormal = carregarTextura("imagens/terra/earthNormal.png");
}

void criarTerra(const vector *camera, const vector *cameraFront, GLint VBO, float currentFrame, int totalVertices) {
    glPushMatrix();
        
        glTranslatef(5.0f, 0.0f, 0.0f);
        glRotatef(currentFrame * 10.0f, 0.0f, 1.0f, 0.0f); // Gira a terra
        glScalef(0.5f, 0.5f, 0.5f);

        glUseProgram(shaderTerraProgram);

        float modelViewMatrix[16];
        float projectionMatrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
        glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

        glUniformMatrix4fv(glGetUniformLocation(shaderTerraProgram, "modelView"), 1, GL_FALSE, modelViewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(shaderTerraProgram, "projection"), 1, GL_FALSE, projectionMatrix);

        // --- POSIÇÃO DO SOL ---
        // A matriz View pura contém as coordenadas do mundo filtradas pela câmera.
        // Para saber onde o centro do universo (Sol) está na visão da câmera, nós 
        // recalculamos essa matriz sem as translações exclusivas do planeta.
        glPushMatrix();
            glLoadIdentity();
            vector alvo = addVectors(*camera, *cameraFront);
            gluLookAt(camera->x, camera->y, camera->z, alvo.x, alvo.y, alvo.z, 0.0f, 1.0f, 0.0f);
            float viewMatrix[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
        glPopMatrix();
        
        // O Sol está em X=0, Y=0, Z=0. Seus componentes na matriz de visão ficam nessas 3 posições:
        glUniform3f(glGetUniformLocation(shaderTerraProgram, "lightPosView"), viewMatrix[12], viewMatrix[13], viewMatrix[14]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idTerraDia);
        glUniform1i(glGetUniformLocation(shaderTerraProgram, "texDia"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, idTerraNoite);
        glUniform1i(glGetUniformLocation(shaderTerraProgram, "texNoite"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, idNuvens);
        glUniform1i(glGetUniformLocation(shaderTerraProgram, "texNuvens"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, idNormal);
        glUniform1i(glGetUniformLocation(shaderTerraProgram, "texNormal"), 3);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glEnableVertexAttribArray(0); // Posição
        glEnableVertexAttribArray(1); // Normal
        glEnableVertexAttribArray(2); // Textura UV
        glEnableVertexAttribArray(3); // Normal Mapping

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(8 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, totalVertices);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        
        glActiveTexture(GL_TEXTURE0);

    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); 

    glUseProgram(shaderNuvemProgram);
    
    glPushMatrix();
        glTranslatef(5.0f, 0.0f, 0.0f); 
        // Rotação geométrica da esfera de nuvens
        glRotatef(currentFrame * -0.5f, 0.0f, 1.0f, 0.0f);
        glScalef(0.505f, 0.505f, 0.505f); // Ligeiramente maior que a Terra
    
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
        glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

        glUniformMatrix4fv(glGetUniformLocation(shaderNuvemProgram, "modelView"), 1, GL_FALSE, modelViewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(shaderNuvemProgram, "projection"), 1, GL_FALSE, projectionMatrix);

        // --- CORREÇÃO 1: Enviando o 'tempo' para animar as UVs ---
        glUniform1f(glGetUniformLocation(shaderNuvemProgram, "tempo"), currentFrame);

        // Recalculando a luz
        glPushMatrix();
            glLoadIdentity();
            alvo = addVectors(*camera, *cameraFront);
            gluLookAt(camera->x, camera->y, camera->z, alvo.x, alvo.y, alvo.z, 0.0f, 1.0f, 0.0f);
            glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
        glPopMatrix();

        glUniform3f(glGetUniformLocation(shaderNuvemProgram, "lightPosView"), viewMatrix[12], viewMatrix[13], viewMatrix[14]);

        // --- CORREÇÃO 2: Bind Limpo da Textura ---
        // O shader de nuvens SÓ precisa da textura de nuvens.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idNuvens);
        glUniform1i(glGetUniformLocation(shaderNuvemProgram, "texNuvens"), 0);
        
        // Desenhando o VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glEnableVertexAttribArray(0); // Posição
        glEnableVertexAttribArray(1); // Normal
        glEnableVertexAttribArray(2); // Textura UV

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, totalVertices);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glUseProgram(0);

}
