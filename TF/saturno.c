#include "libs/config.h"
#include "libs/saturno.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderSaturno(DadosSaturno *dados) {
    char *vertPlaneta = lerArquivo("shaders/planeta/planetaVertexShader.vs");
    char *fragPlaneta = lerArquivo("shaders/planeta/planetaFragShader.vs");
    dados->shaderPlaneta = carregarShader(vertPlaneta, fragPlaneta, NULL);

    char *fragAnel = lerArquivo("shaders/anel/anelFragShader.vs");
    dados->shaderAnel = carregarShader(vertPlaneta, fragAnel, NULL);

    free(vertPlaneta);
    free(fragPlaneta);
    free(fragAnel);
}

DadosSaturno* getDadosSaturno() {
    DadosSaturno *dados = malloc(sizeof(DadosSaturno));
    compilarShaderSaturno(dados);

    dados->idSuperficie = carregarTextura("imagens/saturno/saturno.jpg");
    dados->idAnel = carregarTextura("imagens/saturno/saturno-anel.png");

    DiscoMesh *anelMesh = criarDiscoArray(1.2f, 2.4f, 60);
    dados->totalVerticesAnel = anelMesh->numVertices;

    glGenBuffers(1, &dados->vboAnel);
    glBindBuffer(GL_ARRAY_BUFFER, dados->vboAnel);
    
    glBufferData(GL_ARRAY_BUFFER, anelMesh->numVertices * 8 * sizeof(float), anelMesh->dados, GL_STATIC_DRAW);

    free(anelMesh->dados);
    free(anelMesh);

    return dados;
}

void renderizarSaturno(CorpoCeleste *saturno, const vector *camera, const vector *cameraFront, float currentFrame) {
    DadosSaturno *dados = (DadosSaturno*) saturno->dadosVisuais;
    float raioVisual = calcularRaioVisual(saturno);
    vector posicaoVisual = calcularPosicaoVisual(saturno);

    mat4 modelMatrix;
    mat4 modelViewMatrix;
    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 lightPosView;

    // 1. DESENHAR O PLANETA
    glUseProgram(dados->shaderPlaneta);

    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * saturno->velocidadeRotacao, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual});
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);
    glm_mat4_mulv(globalViewMatrix, posSolMundo, lightPosView);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderPlaneta, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderPlaneta, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderPlaneta, "model"), 1, GL_FALSE, (float*)modelMatrix);
    glUniform3f(glGetUniformLocation(dados->shaderPlaneta, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idSuperficie);
    glUniform1i(glGetUniformLocation(dados->shaderPlaneta, "texPlaneta"), 0);

    glBindBuffer(GL_ARRAY_BUFFER, saturno->VBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, saturno->totalVertices);

    // 2. DESENHAR O ANEL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(dados->shaderAnel);

    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, 26.0f * DEG2RAD, (vec3){0.0f, 0.0f, 1.0f}); 
    glm_rotate(modelMatrix, currentFrame * saturno->velocidadeRotacao * 0.5f, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual}); 

    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAnel, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAnel, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderAnel, "model"), 1, GL_FALSE, (float*)modelMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idAnel);
    glUniform1i(glGetUniformLocation(dados->shaderAnel, "texAnel"), 0);

    glBindBuffer(GL_ARRAY_BUFFER, dados->vboAnel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    glDrawArrays(GL_TRIANGLES, 0, dados->totalVerticesAnel);

    glDisable(GL_BLEND);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glUseProgram(0);
}