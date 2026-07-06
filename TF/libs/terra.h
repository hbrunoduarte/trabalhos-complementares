#ifndef _TERRA_H
#define _TERRA_H

typedef struct {
    GLint shaderTerraProgram;
    GLint shaderNuvemProgram;
    GLint idTerraDia;
    GLint idTerraNoite;
    GLint idNuvens;
    GLint idNormal;
    GLuint idHeight;
} DadosTerra;

DadosTerra* getDadosTerra();
void renderizarTerra(CorpoCeleste *terra, const vector *camera, float currentFrame);

#endif