#include "libs/config.h"
#include "libs/venus.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderVenus(DadosVenus *dados) {
    // 1. Superfície (Usa o genérico normal)
    char *vertPlaneta = lerArquivo("shaders/planeta/planetaVertexShader.vs");
    char *fragPlaneta = lerArquivo("shaders/planeta/planetaFragShader.vs");
    dados->shaderSuperficie = carregarShader(vertPlaneta, fragPlaneta, NULL);

    // 2. Atmosfera (Usa o vertex genérico, mas o frag shader animado de Vênus)
    char *fragAtmosfera = lerArquivo("shaders/venus/atmosferaFragShader.vs");
    dados->shaderAtmosfera = carregarShader(vertPlaneta, fragAtmosfera, NULL);

    free(vertPlaneta);
    free(fragPlaneta);
    free(fragAtmosfera);
}

DadosVenus* getDadosVenus() {
    DadosVenus *dados = malloc(sizeof(DadosVenus));
    compilarShaderVenus(dados);

    dados->idSuperficie = carregarTextura("imagens/venus/venus-superficie.jpg");
    dados->idAtmosfera = carregarTextura("imagens/venus/venus-atmosfera.jpg");
    return dados;
}

void renderizarVenus(CorpoCeleste *venus, const vector *camera, const vector *cameraFront, float currentFrame) {
    DadosVenus *dados = (DadosVenus*) venus->dadosVisuais;
    float raioVisual = calcularRaioVisual(venus);
    vector posicaoVisual = calcularPosicaoVisual(venus);

    // ==========================================
    // PASSO 1: DESENHAR A SUPERFÍCIE DE VÊNUS
    // ==========================================
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

    // Configurar o VBO para a superfície
    glBindBuffer(GL_ARRAY_BUFFER, venus->VBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, venus->totalVertices);

   // ==========================================
    // PASSO 2: DESENHAR A ATMOSFERA (Estilo Terra)
    // ==========================================
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); 

    glUseProgram(dados->shaderAtmosfera);

    // Escala sutil para a atmosfera ficar rente ao planeta
    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * venus->velocidadeRotacao * 1.2f, (vec3){0.0f, 1.0f, 0.0f}); 
    glm_scale(modelMatrix, (vec3){raioVisual * 1.02f, raioVisual * 1.02f, raioVisual * 1.02f});

    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAtmosfera, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAtmosfera, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAtmosfera, "model"), 1, GL_FALSE, (float*)modelMatrix);
    
    // ENVIAR A LUZ E O TEMPO (Igual na Terra)
    glUniform3f(glGetUniformLocation(dados->shaderAtmosfera, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);
    glUniform1f(glGetUniformLocation(dados->shaderAtmosfera, "tempo"), currentFrame);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idAtmosfera);
    // ATENÇÃO AQUI: Como estamos usando o shader da Terra, a variável chama 'texNuvens'
    glUniform1i(glGetUniformLocation(dados->shaderAtmosfera, "texNuvens"), 0);

    // Re-configurar o VBO para desenhar
    glBindBuffer(GL_ARRAY_BUFFER, venus->VBO); 
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, venus->totalVertices);

    // Limpeza
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glUseProgram(0);
}