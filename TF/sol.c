#include "solar.h"
#include "sol.h"

const char* vertexShaderSol = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aTexCoord;\n" // Lemos a UV do VBO
"\n"
"out vec2 TexCoord;\n"
"\n"
"uniform mat4 modelView;\n"
"uniform mat4 projection;\n"
"\n"
"void main() {\n"
"   TexCoord = aTexCoord;\n"
"   gl_Position = projection * modelView * vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderSol = "#version 330 core\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D texSol;\n"
"uniform float tempo;\n" // Recebe o relógio do jogo
"\n"
"void main() {\n"
"   // Camada 1: Desliza a textura rapidamente para a direita\n"
"   vec2 uv1 = TexCoord + vec2(tempo * 0.005, 0.0);\n"
"   \n"
"   // Camada 2: Desliza na diagonal contrária, um pouco mais devagar\n"
"   vec2 uv2 = TexCoord + vec2(-tempo * 0.0003, tempo * 0.002);\n"
"   \n"
"   // Lê os pixels da imagem nas duas posições distorcidas\n"
"   vec3 cor1 = texture(texSol, uv1).rgb;\n"
"   vec3 cor2 = texture(texSol, uv2).rgb;\n"
"   \n"
"   // MÁGICA: Mistura as duas texturas e multiplica por 1.5 para estourar o brilho!\n"
"   // Como a tela de LCD não mostra cores acima de 1.0, o amarelo vira quase branco \n"
"   // no centro, parecendo incandescente.\n"
"   vec3 corFinal = (cor1 + cor2) * 0.8 * 1.5;\n"
"   \n"
"   FragColor = vec4(corFinal, 1.0);\n"
"}\n";

GLint shaderProgramSol = -1;
GLint idTextura = -1;

// Função para compilar o shader do sol (chame ela na sua main igual a outra)
GLuint compilarShaderSol() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSol, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSol, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgramSol = glCreateProgram();
    glAttachShader(shaderProgramSol, vertexShader);
    glAttachShader(shaderProgramSol, fragmentShader);
    glLinkProgram(shaderProgramSol);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgramSol;
}
   
void carregarTexturaSol() {
    shaderProgramSol = compilarShaderSol();
    idTextura = carregarTextura("imagens/sol/sun.jpg");
}

void criarSol(float currentFrame, GLint VBO, int totalVertices) {
        
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, idTextura); 

    glPushMatrix();
        glScalef(3.0f, 3.0f, 3.0f); 

        // Ativa o Shader do Sol
        glUseProgram(shaderProgramSol);

        // Captura as matrizes matemáticas
        float modelViewSol[16];
        float projectionSol[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewSol);
        glGetFloatv(GL_PROJECTION_MATRIX, projectionSol);

        // Envia as matrizes para o Shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramSol, "modelView"), 1, GL_FALSE, modelViewSol);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramSol, "projection"), 1, GL_FALSE, projectionSol);

        // Envia o TEMPO para animar o plasma
        // Usamos currentFrame que você já calcula no seu loop (glfwGetTime())
        glUniform1f(glGetUniformLocation(shaderProgramSol, "tempo"), currentFrame);

        // Ativa a gaveta da textura do Sol
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idTextura);
        glUniform1i(glGetUniformLocation(shaderProgramSol, "texSol"), 0);

        // Diz ao VBO como ler os dados (Posição = 0, UV = 2)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(0); // Posição
        glEnableVertexAttribArray(2); // Textura UV
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

        // Bate o carimbo da esfera!
        glDrawArrays(GL_TRIANGLES, 0, totalVertices);

        // Desliga tudo para limpar o estado
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(2);
        glUseProgram(0);
    glPopMatrix();
}
