#ifndef _TELA_H
#define _TELA_H

void updateCamera(GLFWwindow *window);
void processarInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* configurarTela();

#endif