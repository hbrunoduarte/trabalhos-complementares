#include "libs/config.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderSol(DadosSol *dados) {
    char *vertexShaderSource = lerArquivo("shaders/sol/solVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/sol/solFragShader.vs");

    dados->shaderProgramSol = carregarShader(vertexShaderSource, fragmentShaderSource, NULL);

    free(vertexShaderSource);
    free(fragmentShaderSource);
}
   
void carregarTexturaSol(DadosSol *dados) {
    compilarShaderSol(dados);
    dados->idTextura = carregarTextura("imagens/sol/sun.jpg");
}

DadosSol* getDadosSol() {
    DadosSol *dados = malloc(sizeof(DadosSol));
    carregarTexturaSol(dados);
    return dados;
}

void renderizarSol(CorpoCeleste *sol, vector *camera, float currentFrame) {
    
    DadosSol *dados = (DadosSol*) sol->dadosVisuais;
    float raioVisual = calcularRaioVisual(sol);

    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, dados->idTextura); 

    // Ativa o Shader do Sol
    glUseProgram(dados->shaderProgramSol);

    // calcula as transformações sobre o modelo
    mat4 modelMatrix;
    glm_mat4_identity(modelMatrix);
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual});

    // coloca o modelo em relação à câmera
    mat4 modelViewSol;
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewSol);


    // Envia as matrizes para o Shader
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramSol, "modelView"), 1, GL_FALSE, modelViewSol);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramSol, "projection"), 1, GL_FALSE, globalProjectionMatrix);

    // Envia o TEMPO para animar o plasma
    // Usamos currentFrame que você já calcula no seu loop (glfwGetTime())
    glUniform1f(glGetUniformLocation(dados->shaderProgramSol, "tempo"), currentFrame);

    // Ativa a gaveta da textura do Sol
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idTextura);
    glUniform1i(glGetUniformLocation(dados->shaderProgramSol, "texSol"), 0);

    glBindVertexArray(sol->VAO);
    glDrawArrays(GL_TRIANGLES, 0, sol->totalVertices);

    glBindVertexArray(0);
    glUseProgram(0);
}
