#ifndef _SOMBRA_H
#define _SOMBRA_H

#include "config.h"

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

#define SHADER_LIB_CAMINHO "shaders/sombra/calcularSombra.glsl"

void carregarShaderSombras();
void setFramebufferSombras(CorpoCeleste *sistema, const int numCorpos);
void calcularSombras(GLFWwindow *window, CorpoCeleste *sistema, const int numCorpos);

#endif