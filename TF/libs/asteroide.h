#ifndef _ASTEROIDE_H
#define _ASTEROIDE_H

#include "config.h"

#define QTD_ASTEROIDES 30

typedef struct {
    GLuint idOclusao;
    GLuint idCor;
    GLuint idDisplacement;
    GLuint idNormal;
    GLuint idRugosidade;
    GLuint shaderProgram;
    float dispScale;
} DadosAsteroide;

typedef struct {
    int numAsteroides;
    CorpoCeleste *asteroides;
} DadosCinturao;

DadosAsteroide* getDadosAsteroide(float dispScale);
void renderizarAsteroide(CorpoCeleste *asteroide, const vector *camera, float currentFrame);
void inicializarCinturao(CorpoCeleste *asteroideBase, float desvio);

#endif