#ifndef _SOL_H
#define _SOL_H

typedef struct {
    GLint shaderProgramSol;
    GLint idTextura;
} DadosSol;

DadosSol* getDadosSol();
void renderizarSol(CorpoCeleste *sol, vector *camera, float currentFrame);

#endif