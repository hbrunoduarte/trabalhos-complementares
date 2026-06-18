#ifndef _PLANETA_H
#define _PLANETA_H

typedef struct {
    GLint shaderProgramPlaneta;
    GLint idTextura;
} DadosPlaneta;

DadosPlaneta* getDadosPlaneta(const char* caminhoTextura);
void renderizarPlaneta(CorpoCeleste *planeta, const vector *camera, const vector *cameraFront, float currentFrame);

#endif