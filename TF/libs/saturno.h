#ifndef _SATURNO_H
#define _SATURNO_H

#include "config.h"

typedef struct {
    GLint shaderProgramPlaneta;
    GLint idTextura;
    GLint idTexturaAnel;
} DadosSaturno;

void setTexturaAnel(DadosSaturno *dados, GLint texAnel);
DadosSaturno* getDadosSaturno();
void renderizarSaturno(CorpoCeleste *saturno, const vector *camera, float currentFrame);

#endif