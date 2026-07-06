#ifndef _PLANETA_H
#define _PLANETA_H

typedef struct {
    GLint shaderProgramPlaneta;
    GLint idTextura;
    booleano isGasoso;
    booleano globalShade;
} DadosPlaneta;

DadosPlaneta* getDadosPlaneta(const char* caminhoTextura, booleano isGasoso, booleano needGlobalShade);
void renderizarPlaneta(CorpoCeleste *planeta, const vector *camera, float currentFrame);

#endif