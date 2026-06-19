#ifndef _SATURNO_H
#define _SATURNO_H

typedef struct {
    GLint shaderPlaneta; 
    GLint shaderAnel;    
    GLint idSuperficie;
    GLint idAnel;
    GLuint vboAnel;          // GUARDA A GEOMETRIA DO DISCO
    int totalVerticesAnel;   // QUANTIDADE DE PONTOS DO DISCO
} DadosSaturno;

DadosSaturno* getDadosSaturno();
void renderizarSaturno(CorpoCeleste *saturno, const vector *camera, const vector *cameraFront, float currentFrame);

#endif