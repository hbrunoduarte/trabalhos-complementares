#include "libs/config.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

void compilarShaderTerra(DadosTerra *dados) {

    char *lib = lerArquivo(SHADER_LIB_CAMINHO);
    char *vertexShaderSource = lerArquivo("shaders/terra/terraVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/terra/terraFragShader.vs");

    dados->shaderTerraProgram = carregarShader(vertexShaderSource, fragmentShaderSource, lib);

    free(vertexShaderSource);
    free(fragmentShaderSource);
    free(lib);
}

void compilarShaderNuvens(DadosTerra *dados) {

    char *lib = lerArquivo(SHADER_LIB_CAMINHO);
    char *vertexShaderSource = lerArquivo("shaders/terra/nuvemVertexShader.vs");
    char *fragmentShaderSource = lerArquivo("shaders/terra/nuvemFragShader.vs");

    dados->shaderNuvemProgram = carregarShader(vertexShaderSource, fragmentShaderSource, lib);

    free(vertexShaderSource);
    free(fragmentShaderSource);
    free(lib);
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

void renderizarTerra(CorpoCeleste *terra, const vector *camera, float currentFrame) {

    DadosTerra *dados = (DadosTerra*) terra->dadosVisuais;
    float raioVisual = calcularRaioVisual(terra);
    vector posicaoVisual = calcularPosicaoVisual(terra);

    glUseProgram(dados->shaderTerraProgram);

    mat4 modelMatrix;
    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * terra->velocidadeRotacao * 2, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual});

    mat4 modelViewMatrix;
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "model"), 1, GL_FALSE, (float*)modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "lightSpaceMatrix"), 1, GL_FALSE, (float*)terra->lightSpaceMatrix);

    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 lightPosView;
    glm_mat4_mulv(globalViewMatrix, posSolMundo, lightPosView);
    glUniform3f(glGetUniformLocation(dados->shaderTerraProgram, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, dados->idTerraDia);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, dados->idTerraNoite);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, dados->idNuvens);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, dados->idNormal);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, terra->depthMap);
    
    glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texDia"), 0);
    glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texNoite"), 1);
    glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texNuvens"), 2);
    glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "texNormal"), 3);
    glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "shadowMap"), 4);

    glBindVertexArray(terra->VAO); 
    glDrawArrays(GL_TRIANGLES, 0, terra->totalVertices);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); 

    glUseProgram(dados->shaderNuvemProgram);

    glm_mat4_identity(modelMatrix);
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * -0.5f, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual * 1.01f, raioVisual * 1.01f, raioVisual * 1.01f}); // Ligeiramente maior que a Terra

    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderNuvemProgram, "modelView"), 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderNuvemProgram, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderNuvemProgram, "model"), 1, GL_FALSE, (float*)modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderNuvemProgram, "lightSpaceMatrix"), 1, GL_FALSE, (float*)terra->lightSpaceMatrix);

    glUniform1f(glGetUniformLocation(dados->shaderNuvemProgram, "tempo"), currentFrame);

    glUniform3f(glGetUniformLocation(dados->shaderNuvemProgram, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, dados->idNuvens);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, terra->depthMap);
    
    glUniform1i(glGetUniformLocation(dados->shaderNuvemProgram, "texNuvens"), 0);
    glUniform1i(glGetUniformLocation(dados->shaderNuvemProgram, "shadowMap"), 4);
    
    glDrawArrays(GL_TRIANGLES, 0, terra->totalVertices);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}
