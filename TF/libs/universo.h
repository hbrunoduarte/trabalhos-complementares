#ifndef _UNIVERSO_H
#define _UNIVERSO_H

typedef struct {
    GLint shaderCeu;
    GLint idTextura;
    GLuint vboCeu;
    int totalVertices;
} DadosUniverso;

DadosUniverso* inicializarUniverso();
void renderizarUniverso(DadosUniverso *dados, const vector *camera);

#endif