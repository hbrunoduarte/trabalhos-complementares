#include "libs/config.h"
#include "libs/universo.h"

extern mat4 globalProjectionMatrix;
extern mat4 globalViewMatrix;

DadosUniverso* inicializarUniverso() {
    DadosUniverso *dados = malloc(sizeof(DadosUniverso));

    char *vertCeu = lerArquivo("shaders/planeta/planetaVertexShader.vs");
    char *fragCeu = lerArquivo("shaders/universo/ceuFragShader.vs");
    dados->shaderCeu = carregarShader(vertCeu, fragCeu, NULL);
    free(vertCeu); free(fragCeu);

    dados->idTextura = carregarTextura("imagens/estrelas/estrelas.jpg");

    // Criamos uma malha de esfera bem grande (o tamanho exato não importa por causa do DepthMask)
    EsferaMesh *ceuMesh = criarEsferaArray(500.0f, 60, 60);
    dados->totalVertices = ceuMesh->numVertices;

    glGenBuffers(1, &dados->vboCeu);
    glBindBuffer(GL_ARRAY_BUFFER, dados->vboCeu);
    
    glBufferData(GL_ARRAY_BUFFER, ceuMesh->numVertices * SPHERE_INFO * sizeof(float), ceuMesh->dados, GL_STATIC_DRAW);

    free(ceuMesh->dados);
    free(ceuMesh);

    return dados;
}

void renderizarUniverso(DadosUniverso *dados, const vector *camera) {
    glDepthMask(GL_FALSE);
    
    glUseProgram(dados->shaderCeu);

    mat4 modelMatrix;
    glm_mat4_identity(modelMatrix);
    
    glm_translate(modelMatrix, camera->raw); 
    
    mat4 modelViewMatrix;
    glm_mat4_mul(globalViewMatrix, modelMatrix, modelViewMatrix);

    glUniformMatrix4fv(glGetUniformLocation(dados->shaderCeu, "modelView"), 1, GL_FALSE, (float*)modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderCeu, "projection"), 1, GL_FALSE, globalProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(dados->shaderCeu, "model"), 1, GL_FALSE, (float*)modelMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dados->idTextura);
    glUniform1i(glGetUniformLocation(dados->shaderCeu, "texCeu"), 0);

    glBindBuffer(GL_ARRAY_BUFFER, dados->vboCeu);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)(6 * sizeof(float)));
    
    glDrawArrays(GL_TRIANGLES, 0, dados->totalVertices);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glUseProgram(0);
    
    glDepthMask(GL_TRUE); 
}