#include "libs/config.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderSaturno(DadosSaturno *dados) {
    char *lib = lerArquivo(SHADER_LIB_CAMINHO); 
    char *vertexShaderSource = lerArquivo("shaders/planeta/planetaVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/planeta/planetaFragShader.vs");

    dados->shaderProgramPlaneta = carregarShader(vertexShaderSource, fragmentShaderSource, lib);

    free(vertexShaderSource);
    free(fragmentShaderSource);
    free(lib);
}

void setTexturaAnel(DadosSaturno *dados, GLint texAnel) {
    dados->idTexturaAnel = texAnel;
}

DadosSaturno* getDadosSaturno() {
    DadosSaturno *dados = malloc(sizeof(DadosSaturno));
    compilarShaderSaturno(dados);
    dados->idTextura = carregarTextura(TEX_SATURNO);
    return dados;
}

void renderizarSaturno(CorpoCeleste *saturno, const vector *camera, float currentFrame) {

    DadosSaturno *dados = (DadosSaturno*) saturno->dadosVisuais;
    float raioVisual = calcularRaioVisual(saturno);
    vector posicaoVisual = calcularPosicaoVisual(saturno);

    glUseProgram(dados->shaderProgramPlaneta);

    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 lightPosView;
    glm_mat4_mulv(globalViewMatrix, posSolMundo, lightPosView);
    glUniform3f(glGetUniformLocation(dados->shaderProgramPlaneta, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, dados->idTextura);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, saturno->depthMap);

    glUniform1i(glGetUniformLocation(dados->shaderProgramPlaneta, "texPlaneta"), 0);
    glUniform1f(glGetUniformLocation(dados->shaderProgramPlaneta, "opacidadeAnel"), 0.6);
    glUniform1i(glGetUniformLocation(dados->shaderProgramPlaneta, "usarAnel"), 1);
    glUniform1i(glGetUniformLocation(dados->shaderProgramPlaneta, "isGasoso"), 1);
    glUniform1i(glGetUniformLocation(dados->shaderProgramPlaneta, "recebeSombras"), 1);
    glUniform1i(glGetUniformLocation(dados->shaderProgramPlaneta, "shadowMap"), 4);

    glBindVertexArray(saturno->VAO);

    for (int i = 0; i < 3; i++) {

        if (i > 0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
        }

        float scaleMult = 1.0f + (i * 0.02f); 
        
        float tempoLayer = currentFrame * (1.0f + i * 0.6f);
        
        float alphaLayer = (i == 0) ? 1.0f : 0.25f / i; 

        mat4 modelMatrix;
        glm_mat4_identity(modelMatrix);
        glm_translate(modelMatrix, posicaoVisual.raw);
        glm_rotate(modelMatrix, (currentFrame * saturno->velocidadeRotacao) + (i * currentFrame * 0.1f), (vec3){0.0f, 1.0f, 0.0f});
        glm_scale(modelMatrix, (vec3){raioVisual * scaleMult, raioVisual * scaleMult, raioVisual * scaleMult});

        mat4 modelViewMatrix;
        glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

        glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "projection"), 1, GL_FALSE, globalProjectionMatrix);
        glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "model"), 1, GL_FALSE, (float*)modelMatrix);
        glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgramPlaneta, "lightSpaceMatrix"), 1, GL_FALSE, (float*)saturno->lightSpaceMatrix);
        
        glUniform1f(glGetUniformLocation(dados->shaderProgramPlaneta, "tempo"), tempoLayer);
        glUniform1f(glGetUniformLocation(dados->shaderProgramPlaneta, "alphaMultiplicador"), alphaLayer);

        glDrawArrays(GL_TRIANGLES, 0, saturno->totalVertices);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glUseProgram(0);
}