#ifndef _CONFIG_H
#define _CONFIG_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"

#define STB_IMAGE_IMPLEMENTATION

#define BUFFER_LEITURA_ARQUIVO 1024

#define booleano char

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD (M_PI / 180.0f)

typedef struct corpo_celeste {

    // Física do corpo
    float massa;
    vector posicao;
    vector velocidade;
    vector forcaAcumulada;

    // Dados para renderização
    float raioVisual;
    GLint VBO;           // Todos usam a mesma malha da esfera
    int totalVertices;   // Quantidade de vértices da malha

    // Função e dados para realizar a renderização
    void (*renderizar)(struct corpo_celeste* self, const vector* camera, const vector* cameraFront, float currentFrame);
    void* dadosVisuais; // Ponteiro genérico para guardar as texturas e shaders específicos

} CorpoCeleste;

#include "terra.h"
#include "sol.h"
#include "tela.h"
#include "malha.h"

char* lerArquivo(const char *caminho);
GLuint carregarShader(const char *vertexShaderSource, const char *fragShaderSource);
GLuint carregarTextura(const char* caminho);

#endif