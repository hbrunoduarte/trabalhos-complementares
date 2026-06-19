#include "libs/config.h"
#include "libs/planeta.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderPlaneta(DadosPlaneta *dados) {
    char *vertexShaderSource = lerArquivo("shaders/planeta/planetaVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/planeta/planetaFragShader.vs");

    dados->shaderProgramPlaneta = carregarShader(vertexShaderSource, fragmentShaderSource, NULL);

    free(vertexShaderSource);
    free(fragmentShaderSource);
}

DadosPlaneta* getDadosPlaneta(const char* caminhoTextura) {
    DadosPlaneta *dados = malloc(sizeof(DadosPlaneta));
    compilarShaderPlaneta(dados);
    dados->idTextura = carregarTextura(caminhoTextura);
    return dados;
}

void renderizarPlaneta(CorpoCeleste *planeta, const vector *camera, const vector *cameraFront, float currentFrame) {
    DadosPlaneta *dados = (DadosPlaneta*) planeta->dadosVisuais;
    float raioVisual = calcularRaioVisual(planeta);
    vector posicaoVisual = calcularPosicaoVisual(planeta);

    glUseProgram(dados->shaderProgramPlaneta);

    // 1. CÁLCULO DA MATRIZ MODEL (TRANSFORMAÇÕES DO PLANETA)
    mat4 modelMatrix;
    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * planeta->velocidadeRotacao, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual});

    // 2. COMBINAR VIEW E MODEL
    mat4 modelViewMatrix;
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    // 3. ENVIAR MATRIZES PARA O SHADER
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "model"), 1, GL_FALSE, (float*)modelMatrix);

    // 4. CALCULAR A POSIÇÃO DO SOL (LUZ) NA PERSPETIVA DA CÂMERA
    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 lightPosView;
    glm_mat4_mulv(globalViewMatrix, posSolMundo, lightPosView);
    glUniform3f(glGetUniformLocation(dados->shaderProgramPlaneta, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

    // 5. BIND DA TEXTURA DO PLANETA
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idTextura);
    glUniform1i(glGetUniformLocation(dados->shaderProgramPlaneta, "texPlaneta"), 0);

    // 6. CONFIGURAR O VBO PARA DESENHAR
    glBindBuffer(GL_ARRAY_BUFFER, planeta->VBO);

    glEnableVertexAttribArray(0); // Posição
    glEnableVertexAttribArray(1); // Normal
    glEnableVertexAttribArray(2); // Textura UV

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));

    // 7. DESENHAR A ESFERA
    glDrawArrays(GL_TRIANGLES, 0, planeta->totalVertices);

    // 8. LIMPAR O ESTADO DO OPENGL
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glUseProgram(0);
}