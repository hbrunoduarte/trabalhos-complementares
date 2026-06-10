#ifndef _SOLAR_H
#define _SOLAR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vector.h"

#define BUFFER_LEITURA_ARQUIVO 1024

#define booleano char

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD (M_PI / 180.0f)

char* lerArquivo(const char *caminho);
GLuint carregarShader(const char *vertexShaderSource, const char *fragShaderSource);
GLuint carregarTextura(const char* caminho);

#endif