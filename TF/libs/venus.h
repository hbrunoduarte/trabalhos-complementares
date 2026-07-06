#ifndef _VENUS_H
#define _VENUS_H

typedef struct {
    GLint shaderSuperficie;
    GLint shaderAtmosfera;
    GLint idSuperficie;
    GLint idAtmosfera;
} DadosVenus;

DadosVenus* getDadosVenus();
void renderizarVenus(CorpoCeleste *venus, const vector *camera, float currentFrame);

#endif