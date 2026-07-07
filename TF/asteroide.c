#include "libs/config.h"

#define CAMINHO_COR "imagens/asteroide/cor.png"
#define CAMINHO_DISPLACEMENT "imagens/asteroide/displacement.png"
#define CAMINHO_NORMAL "imagens/asteroide/normal.png"
#define CAMINHO_OCLUSAO "imagens/asteroide/oclusao.png"
#define CAMINHO_RUGOSIDADE "imagens/asteroide/rugosidade.png"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;
extern float tempoAcumulado;

void compilarShaderAsteroide(DadosAsteroide *dados) {

    char *lib = lerArquivo(SHADER_LIB_CAMINHO);
    char *vShader = lerArquivo("shaders/asteroide/asteroideVertexShader.vs");
    char *fShader = lerArquivo("shaders/asteroide/asteroideFragShader.vs");

    dados->shaderProgram = carregarShader(vShader, fShader, lib);

    free(lib);
    free(vShader);
    free(fShader);
}

void carregarTexturaAsteroide(DadosAsteroide *dados) {
    dados->idCor = carregarTextura(CAMINHO_COR);
    dados->idDisplacement = carregarTextura(CAMINHO_DISPLACEMENT);
    dados->idNormal = carregarTextura(CAMINHO_NORMAL);
    dados->idOclusao = carregarTextura(CAMINHO_OCLUSAO);
    dados->idRugosidade = carregarTextura(CAMINHO_RUGOSIDADE);
}

DadosAsteroide* getDadosAsteroide(float dispScale) {
    DadosAsteroide *dados = malloc(sizeof(DadosAsteroide));
    carregarTexturaAsteroide(dados);
    compilarShaderAsteroide(dados);
    dados->dispScale = dispScale;
    return dados;
}

void renderizarAsteroide(CorpoCeleste *asteroide, const vector *camera, float currentFrame) {

    DadosAsteroide *dados = (DadosAsteroide*) asteroide->dadosVisuais; 

    glUseProgram(dados->shaderProgram); 

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, dados->idCor); 
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, dados->idNormal); 
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, dados->idRugosidade); 
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, dados->idOclusao); 
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, dados->idDisplacement); 
    glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, asteroide->depthMap); 

    glUniform1i(glGetUniformLocation(dados->shaderProgram, "colorMap"), 0); 
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "normalGLMap"), 1); 
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "roughnessMap"), 2); 
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "aoMap"), 3); 
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "displacementMap"), 4); 
    glUniform1i(glGetUniformLocation(dados->shaderProgram, "shadowMap"), 5);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "view"), 1, GL_FALSE, (float*)globalViewMatrix); 
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "projection"), 1, GL_FALSE, (float*)globalProjectionMatrix); 
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, (float*)asteroide->lightSpaceMatrix); 
    glUniform1f(glGetUniformLocation(dados->shaderProgram, "dispScale"), dados->dispScale); 

    vec3 lightPos = {0.0f, 0.0f, 0.0f};
    glUniform3fv(glGetUniformLocation(dados->shaderProgram, "lightPos"), 1, lightPos);
    glUniform3fv(glGetUniformLocation(dados->shaderProgram, "viewPos"), 1, camera->raw);
    glUniform1f(glGetUniformLocation(dados->shaderProgram, "velAngular"), asteroide->velocidadeAngular);
    glUniform1f(glGetUniformLocation(dados->shaderProgram, "tempo"), tempoAcumulado);

    glBindVertexArray(asteroide->VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, asteroide->totalVertices, QTD_ASTEROIDES);
    
    glBindVertexArray(0);
    glUseProgram(0); 
}

mat4* matrizesAsteroides;

void inicializarCinturao(CorpoCeleste *asteroideBase, float desvio) {

    matrizesAsteroides = malloc(QTD_ASTEROIDES * sizeof(mat4));
    
    vector posVis = calcularPosicaoVisual(asteroideBase);
    float raioBase = glm_vec3_norm(posVis.raw);
    float raioVisualAsteroide = calcularRaioVisual(asteroideBase);

    for(int i = 0; i < QTD_ASTEROIDES; i++) {
        mat4 model;
        glm_mat4_identity(model);

        float angulo = (float)rand() / (float)RAND_MAX * 360.0f;
        float deslocamentoX = ((float)rand() / (float)RAND_MAX * 2.0f * desvio) - desvio;
        float deslocamentoY = ((float)rand() / (float)RAND_MAX * 2.0f * desvio) - desvio;
        float deslocamentoZ = ((float)rand() / (float)RAND_MAX * 2.0f * desvio) - desvio;

        float x = sin(angulo) * raioBase + deslocamentoX;
        float y = deslocamentoY * 0.4f;
        float z = cos(angulo) * raioBase + deslocamentoZ;
        
        glm_translate(model, (vec3){x, y, z});

        // CORREÇÃO 3: Multiplicar a variação aleatória pelo raio visual
        float variacao = ((float)rand() / (float)RAND_MAX * 0.5f) + 0.1f;
        float escalaFinal = variacao * raioVisualAsteroide;
        glm_scale(model, (vec3){escalaFinal, escalaFinal, escalaFinal});

        float rotAngulo = (float)rand() / (float)RAND_MAX * 360.0f;
        glm_rotate(model, rotAngulo, (vec3){0.4f, 0.6f, 0.8f});
        glm_mat4_copy(model, matrizesAsteroides[i]);

        printf("\nAsteroide %i: (%.3f, %.3f, %.3f)", i+1, x, y, z);
    }
    printf("\n");

    unsigned int instancedVBO;
    glGenBuffers(1, &instancedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instancedVBO);
    glBufferData(GL_ARRAY_BUFFER, QTD_ASTEROIDES * sizeof(mat4), &matrizesAsteroides[0], GL_STATIC_DRAW);

    glBindVertexArray(asteroideBase->VAO);

    int stride = sizeof(mat4);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(i * sizeof(vec4)));
        glVertexAttribDivisor(4 + i, 1);
    }
    glBindVertexArray(0);

    free(matrizesAsteroides);
}
