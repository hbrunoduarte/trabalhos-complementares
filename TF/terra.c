#include "libs/config.h"

void compilarShaderTerra(DadosTerra *dados) {
    
    char *vertexShaderSource = lerArquivo("shaders/terra/terraVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/terra/terraFragShader.vs");

    dados->shaderTerraProgram = carregarShader(vertexShaderSource, fragmentShaderSource);

    free(vertexShaderSource);
    free(fragmentShaderSource);
}

void compilarShaderNuvens(DadosTerra *dados) {

    char *vertexShaderSource = lerArquivo("shaders/terra/nuvemVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/terra/nuvemFragShader.vs");

    dados->shaderNuvemProgram = carregarShader(vertexShaderSource, fragmentShaderSource);

    free(vertexShaderSource);
    free(fragmentShaderSource);
}

void carregarTexturaTerra(DadosTerra *dados) {
    compilarShaderTerra(dados);
    compilarShaderNuvens(dados);
    dados->idTerraDia = carregarTextura("imagens/terra/earth.jpg");
    dados->idTerraNoite = carregarTextura("imagens/terra/earthNight.jpg");
    dados->idNuvens = carregarTextura("imagens/terra/earthClouds.jpg");
    dados->idNormal = carregarTextura("imagens/terra/earthNormal.png");
}

DadosTerra* getDadosTerra() {
    DadosTerra *dados = malloc(sizeof(DadosTerra));
    carregarTexturaTerra(dados);
    return dados;
}

void renderizarTerra(CorpoCeleste *terra, const vector *camera, const vector *cameraFront, float currentFrame) {

    DadosTerra *dados = (DadosTerra*) terra->dadosVisuais;

    glPushMatrix();
        
        glTranslatef(terra->posicao.x, terra->posicao.y, terra->posicao.z);
        glRotatef(currentFrame * 10.0f, 0.0f, 1.0f, 0.0f); // Gira a terra
        glScalef(terra->raioVisual, terra->raioVisual, terra->raioVisual);

        glUseProgram(dados->shaderTerraProgram);

        float modelViewMatrix[16];
        float projectionMatrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
        glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

        glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "modelView"), 1, GL_FALSE, modelViewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "projection"), 1, GL_FALSE, projectionMatrix);

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
        glUniform3f(glGetUniformLocation(dados->shaderTerraProgram, "lightPosView"), viewMatrix[12], viewMatrix[13], viewMatrix[14]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dados->idTerraDia);
        glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texDia"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dados->idTerraNoite);
        glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texNoite"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, dados->idNuvens);
        glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texNuvens"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, dados->idNormal);
        glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texNormal"), 3);

        glBindBuffer(GL_ARRAY_BUFFER, terra->VBO);

        glEnableVertexAttribArray(0); // Posição
        glEnableVertexAttribArray(1); // Normal
        glEnableVertexAttribArray(2); // Textura UV
        glEnableVertexAttribArray(3); // Normal Mapping

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(8 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, terra->totalVertices);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        
        glActiveTexture(GL_TEXTURE0);

    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); 

    glUseProgram(dados->shaderNuvemProgram);
    
    glPushMatrix();
        glTranslatef(terra->posicao.x, terra->posicao.y, terra->posicao.z); 
        // Rotação geométrica da esfera de nuvens
        glRotatef(currentFrame * -0.5f, 0.0f, 1.0f, 0.0f);
        glScalef(terra->raioVisual*1.01, terra->raioVisual*1.01, terra->raioVisual*1.01); // Ligeiramente maior que a Terra
    
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
        glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

        glUniformMatrix4fv(glGetUniformLocation(dados->shaderNuvemProgram, "modelView"), 1, GL_FALSE, modelViewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(dados->shaderNuvemProgram, "projection"), 1, GL_FALSE, projectionMatrix);

        // --- CORREÇÃO 1: Enviando o 'tempo' para animar as UVs ---
        glUniform1f(glGetUniformLocation(dados->shaderNuvemProgram, "tempo"), currentFrame);

        // Recalculando a luz
        glPushMatrix();
            glLoadIdentity();
            alvo = addVectors(*camera, *cameraFront);
            gluLookAt(camera->x, camera->y, camera->z, alvo.x, alvo.y, alvo.z, 0.0f, 1.0f, 0.0f);
            glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
        glPopMatrix();

        glUniform3f(glGetUniformLocation(dados->shaderNuvemProgram, "lightPosView"), viewMatrix[12], viewMatrix[13], viewMatrix[14]);

        // --- CORREÇÃO 2: Bind Limpo da Textura ---
        // O shader de nuvens SÓ precisa da textura de nuvens.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dados->idNuvens);
        glUniform1i(glGetUniformLocation(dados->shaderNuvemProgram, "texNuvens"), 0);
        
        // Desenhando o VBO
        glBindBuffer(GL_ARRAY_BUFFER, terra->VBO);

        glEnableVertexAttribArray(0); // Posição
        glEnableVertexAttribArray(1); // Normal
        glEnableVertexAttribArray(2); // Textura UV

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, terra->totalVertices);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glUseProgram(0);

}
