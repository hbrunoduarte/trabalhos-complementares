#ifndef _ANELSATURNO_H
#define _ANELSATURNO_H

#include "config.h"

typedef struct {
    GLint shaderProgram;
    GLint idTextura;
} DadosAnelSaturno;

DadosAnelSaturno* getDadosAnelSaturno();
void renderizarAnelSaturno(CorpoCeleste *anel, const vector *camera, float currentFrame);

#endif