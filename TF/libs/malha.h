#ifndef _MALHA_H
#define _MALHA_H

#define SPHERE_INFO 11

typedef struct {
    float *dados;
    int numVertices;
} EsferaMesh;

void addVertice(float* array, int* index, float r, float phi, float theta);
EsferaMesh* criarEsferaArray(float r, unsigned int pTheta, unsigned int pPhi);

#endif