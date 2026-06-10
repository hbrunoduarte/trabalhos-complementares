#ifndef _SOLAR_H
#define _SOLAR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"

#define SPHERE_INFO 11

#define booleano char

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD (M_PI / 180.0f)

typedef struct {
    float *dados;
    int numVertices;
} EsferaMesh;

GLuint carregarTextura(const char* caminho);

#endif