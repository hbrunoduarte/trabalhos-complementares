#ifndef _CONFIG_H
#define _CONFIG_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cglm/cglm.h>
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
    float densidade;
    vector posicao;
    float distanciaOrbital;
    float anguloAtual;
    float velocidadeAngular;
    float opacidade;
    
    // Dados para renderização
    float raio;
    float velocidadeRotacao;
    GLint VBO;
    GLint VAO;
    int totalVertices;

    // Função e dados para realizar a renderização
    void(*renderizar)(struct corpo_celeste* self, const vector* camera, float currentFrame);
    struct corpo_celeste *orbita;
    void* dadosVisuais; // Ponteiro genérico para guardar as texturas e shaders específicos

    // Renderização e física
    GLuint depthMapFBO;      // O Framebuffer deste planeta
    GLuint depthMap;         // A textura da sombra gerada para este planeta
    mat4 lightSpaceMatrix;

} CorpoCeleste;

typedef void(*funcaoRenderizacao)(struct corpo_celeste* self, const vector* camera, float currentFrame);
typedef enum {
    SOL, MERCURIO, VENUS, TERRA, MARTE, JUPITER, SATURNO, URANO, NETUNO, LUA, ANEL_SATURNO, ASTEROIDE
} Planetas;

#include "sombra.h"
#include "fisica.h"
#include "terra.h"
#include "sol.h"
#include "tela.h"
#include "malha.h"
#include "planeta.h"
#include "venus.h"
#include "universo.h"
#include "anelSaturno.h"
#include "asteroide.h"
#include "saturno.h"

char* lerArquivo(const char *caminho);
GLuint carregarShader(const char *vertexShaderSource, const char *fragShaderSource, const char *libSource);
GLuint carregarTextura(const char* caminho);

#define TEX_MERCURIO "imagens/mercurio/mercurio.jpg"
#define TEX_VENUS "imagens/venus/venus.jpg"
#define TEX_MARTE "imagens/marte/marte.jpg"
#define TEX_JUPITER "imagens/jupiter/jupiter.jpg"
#define TEX_SATURNO "imagens/saturno/saturno.jpg"
#define TEX_URANO "imagens/urano/urano.jpg"
#define TEX_NETUNO "imagens/netuno/netuno.jpg"
#define TEX_LUA "imagens/lua/lua.jpg"

#endif