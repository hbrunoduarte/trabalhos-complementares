#include "libs/config.h"
#include "libs/venus.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderVenus(DadosVenus *dados) {
    
    char *lib = lerArquivo(SHADER_LIB_CAMINHO);
    char *vertPlaneta = lerArquivo("shaders/planeta/planetaVertexShader.vs");
    char *fragPlaneta = lerArquivo("shaders/planeta/planetaFragShader.vs");
    dados->shaderSuperficie = carregarShader(vertPlaneta, fragPlaneta, lib);

    // 2. Atmosfera (Usa o vertex genérico, mas o frag shader animado de Vênus)
    char *fragAtmosfera = lerArquivo("shaders/venus/atmosferaFragShader.vs");
    dados->shaderAtmosfera = carregarShader(vertPlaneta, fragAtmosfera, NULL);

    free(vertPlaneta);
    free(fragPlaneta);
    free(lib);
    free(fragAtmosfera);
}

DadosVenus* getDadosVenus() {
    DadosVenus *dados = malloc(sizeof(DadosVenus));
    compilarShaderVenus(dados);

    dados->idSuperficie = carregarTextura("imagens/venus/venus-superficie.jpg");
    dados->idAtmosfera = carregarTextura("imagens/venus/venus-atmosfera.jpg");
    return dados;
}

void renderizarVenus(CorpoCeleste *venus, const vector *camera, float currentFrame) {
    DadosVenus *dados = (DadosVenus*) venus->dadosVisuais;
    float raioVisual = calcularRaioVisual(venus);
    vector posicaoVisual = calcularPosicaoVisual(venus);

    // superfície
    glUseProgram(dados->shaderSuperficie);

    mat4 modelMatrix;
    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * venus->velocidadeRotacao, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual});

    mat4 modelViewMatrix;
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderSuperficie, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderSuperficie, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderSuperficie, "model"), 1, GL_FALSE, (float*)modelMatrix);

    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 lightPosView;
    glm_mat4_mulv(globalViewMatrix, posSolMundo, lightPosView);
    glUniform3f(glGetUniformLocation(dados->shaderSuperficie, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idSuperficie);
    glUniform1i(glGetUniformLocation(dados->shaderSuperficie, "texPlaneta"), 0);

    glUniform1i(glGetUniformLocation(dados->shaderSuperficie, "isGasoso"), 1);
    glUniform1i(glGetUniformLocation(dados->shaderSuperficie, "recebeSombras"), 1);
    glUniform1f(glGetUniformLocation(dados->shaderSuperficie, "alphaMultiplicador"), 1.0f);
    glUniform1f(glGetUniformLocation(dados->shaderSuperficie, "tempo"), currentFrame);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, venus->depthMap);
    glUniform1i(glGetUniformLocation(dados->shaderSuperficie, "shadowMap"), 4);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderSuperficie, "lightSpaceMatrix"), 1, GL_FALSE, (float*)venus->lightSpaceMatrix);

    glBindVertexArray(venus->VAO);
    glDrawArrays(GL_TRIANGLES, 0, venus->totalVertices);

    // atmosfera
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); 

    glUseProgram(dados->shaderAtmosfera);

    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * venus->velocidadeRotacao * 1.2f, (vec3){0.0f, 1.0f, 0.0f}); 
    glm_scale(modelMatrix, (vec3){raioVisual * 1.02f, raioVisual * 1.02f, raioVisual * 1.02f});

    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAtmosfera, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAtmosfera, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAtmosfera, "model"), 1, GL_FALSE, (float*)modelMatrix);
    
    glUniform3f(glGetUniformLocation(dados->shaderAtmosfera, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);
    glUniform1f(glGetUniformLocation(dados->shaderAtmosfera, "tempo"), currentFrame);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idAtmosfera);
    glUniform1i(glGetUniformLocation(dados->shaderAtmosfera, "texNuvens"), 0);

    glDrawArrays(GL_TRIANGLES, 0, venus->totalVertices);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glUseProgram(0);
}