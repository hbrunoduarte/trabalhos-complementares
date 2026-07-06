#include "libs/config.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderAnel(DadosAnelSaturno *dados) {

    char *lib = lerArquivo(SHADER_LIB_CAMINHO);
    char *vertexAnel = lerArquivo("shaders/anel/anelVertexShader.vs");
    char *fragAnel = lerArquivo("shaders/anel/anelFragShader.vs");
    dados->shaderProgram = carregarShader(vertexAnel, fragAnel, lib);

    free(vertexAnel);
    free(fragAnel);
    free(lib);
}

DadosAnelSaturno* getDadosAnelSaturno() {
    DadosAnelSaturno *dados = malloc(sizeof(DadosAnelSaturno));
    compilarShaderAnel(dados);
    dados->idTextura = carregarTextura("imagens/saturno/saturno-anel.png");
    return dados;
}

void renderizarAnelSaturno(CorpoCeleste *anel, const vector *camera, float currentFrame) {
    DadosAnelSaturno *dados = (DadosAnelSaturno*) anel->dadosVisuais;
    float raioVisual = calcularRaioVisual(anel->orbita);
    vector posicaoVisual = calcularPosicaoVisual(anel->orbita);

    mat4 modelMatrix;
    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 posSolView;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(dados->shaderProgram);

    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, anel->velocidadeRotacao, (vec3){0.0f, 0.0f, 1.0f}); 
    glm_rotate(modelMatrix, currentFrame * anel->velocidadeRotacao * 0.5f, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual}); 

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "model"), 1, GL_FALSE, (float*)modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "view"), 1, GL_FALSE, (float*)globalViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "projection"), 1, GL_FALSE, (float*)globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, (float*)anel->lightSpaceMatrix);

    glm_mat4_mulv(globalViewMatrix, posSolMundo, posSolView);
    glUniform3f(glGetUniformLocation(dados->shaderProgram, "lightPosView"), posSolView[0], posSolView[1], posSolView[2]);

    glUniform1i(glGetUniformLocation(dados->shaderProgram, "recebeSombras"), 1);
    glUniform1f(glGetUniformLocation(dados->shaderProgram, "alphaMultiplicador"), 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idTextura);
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "texAnel"), 0);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, anel->depthMap);
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "shadowMap"), 4);

    glBindVertexArray(anel->VAO);
    glDrawArrays(GL_TRIANGLES, 0, anel->totalVertices);

    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glUseProgram(0);
}