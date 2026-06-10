#ifndef _TELA_H
#define _TELA_H

#define RAZAO_TELA 16.0/9.0

void processarInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* configurarTela();

#endif