#include "libs/config.h"

GLint shaderTerraProgram = -1;
GLint shaderNuvemProgram = -1;
GLint idTerraDia = -1;
GLint idTerraNoite = -1;
GLint idNuvens = -1;
GLint idNormal = -1;

void compilarShaderTerra() {
    
    char *vertexShaderSource = lerArquivo("shaders/terra/terraVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/terra/terraFragShader.vs");

    shaderTerraProgram = carregarShader(vertexShaderSource, fragmentShaderSource);

    free(vertexShaderSource);
    free(fragmentShaderSource);
}

void compilarShaderNuvens() {

    char *vertexShaderSource = lerArquivo("shaders/terra/nuvemVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/terra/nuvemFragShader.vs");

    shaderNuvemProgram = carregarShader(vertexShaderSource, fragmentShaderSource);

    free(vertexShaderSource);
    free(fragmentShaderSource);
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
        
        glTranslatef(60.0f, 0.0f, 0.0f);
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
        glTranslatef(60.0f, 0.0f, 0.0f); 
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
