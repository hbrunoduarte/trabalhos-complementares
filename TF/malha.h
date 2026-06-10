#ifndef _MALHA_H
#define _MALHA_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD (M_PI / 180.0f)

#define SPHERE_INFO 11

typedef struct {
    float *dados;
    int numVertices;
} EsferaMesh;

#endif