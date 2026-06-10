#include "solar.h"
#include "sol.h"

GLint shaderProgramSol = -1;
GLint idTextura = -1;

void compilarShaderSol() {
    char *vertexShaderSource = lerArquivo("shaders/sol/solVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/sol/solFragShader.vs");

    shaderProgramSol = carregarShader(vertexShaderSource, fragmentShaderSource);

    free(vertexShaderSource);
    free(fragmentShaderSource);
}
   
void carregarTexturaSol() {
    compilarShaderSol();
    idTextura = carregarTextura("imagens/sol/sun.jpg");
}

void criarSol(float currentFrame, GLint VBO, int totalVertices) {
        
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, idTextura); 

    glPushMatrix();
        glScalef(54.5f, 54.5f, 54.5f); 

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
