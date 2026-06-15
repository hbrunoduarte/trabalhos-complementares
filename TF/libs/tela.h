#ifndef _TELA_H
#define _TELA_H

#define MIN_TIMESPEED 0.03125
#define MAX_TIMESPEED 64.0

void updateCamera(GLFWwindow *window);
void processarInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* configurarTela();

#endif