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

void renderizarTerra(CorpoCeleste *terra, const vector *camera, const vector *cameraFront, float currentFrame) {

    DadosTerra *dados = (DadosTerra*) terra->dadosVisuais;
    float raioVisual = calcularRaioVisual(terra);
    vector posicaoVisual = calcularPosicaoVisual(terra);

    glUseProgram(dados->shaderTerraProgram);

    // 2. CÁLCULO DA MATRIZ MODEL (TRANSFORMAÇÕES DO PLANETA)
    mat4 modelMatrix;
    glm_mat4_identity(modelMatrix); // Começa com uma matriz limpa
    glm_translate(modelMatrix, posicaoVisual.raw);
    glm_rotate(modelMatrix, currentFrame * terra->velocidadeRotacao * 2, (vec3){0.0f, 1.0f, 0.0f});
    glm_scale(modelMatrix, (vec3){raioVisual, raioVisual, raioVisual});

    // 3. COMBINAR VIEW E MODEL
    mat4 modelViewMatrix;
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    // 5. ENVIAR MATRIZES PARA O SHADER
    // A cglm já formata a matriz no padrão coluna-maior do OpenGL (GL_FALSE)
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "model"), 1, GL_FALSE, (float*)modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderTerraProgram, "lightSpaceMatrix"), 1, GL_FALSE, (float*)terra->lightSpaceMatrix);

    // 6. CALCULAR A POSIÇÃO DO SOL (Luz) NA VISÃO DA CÂMARA
    // O Sol está na origem do mundo (0, 0, 0). 
    vec4 posSolMundo = {0.0f, 0.0f, 0.0f, 1.0f};
    vec4 lightPosView;
    glm_mat4_mulv(globalViewMatrix, posSolMundo, lightPosView);
    glUniform3f(glGetUniformLocation(dados->shaderTerraProgram, "lightPosView"), lightPosView[0], lightPosView[1], lightPosView[2]);

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

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, terra->depthMap);
    glUniform1i(glGetUniformLocation(dados->shaderTerraProgram, "shadowMap"), 4);

    glBindBuffer(GL_ARRAY_BUFFER, terra->VBO);

    glEnableVertexAttribArray(0); // Posição
    glEnableVertexAttribArray(1); // Normal
    glEnableVertexAttribArray(2); // Textura UV
    glEnableVertexAttribArray(3); // Normal Mapping

    // Argumentos: location, quantidade de valores associados, tipo de dado, quantidade de memória por 'linha', início dos dados
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idNuvens);
    glUniform1i(glGetUniformLocation(dados->shaderNuvemProgram, "texNuvens"), 0);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, terra->depthMap);
    glUniform1i(glGetUniformLocation(dados->shaderNuvemProgram, "shadowMap"), 4);
    
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

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glUseProgram(0);

}
