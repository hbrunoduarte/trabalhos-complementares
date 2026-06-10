#include "solar.h"

extern float lastX;
extern float lastY;
extern float yaw;
extern float pitch;
extern vector camera;
extern vector cameraFront;
extern booleano running;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    
    static booleano firstMouse = 1;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido pois as coordenadas Y vão de cima para baixo
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){

    float scrollSpeed = 0.5f;

    if (yoffset>0) {
        camera = addVectors(camera, mulVector(cameraFront, scrollSpeed));
    } else if (yoffset<0) {
        camera = addVectors(camera, mulVector(cameraFront, -scrollSpeed));
    }
}

void processarInput(GLFWwindow *window) {

    float velocidadeRotacao = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  yaw -= velocidadeRotacao;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw += velocidadeRotacao;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    pitch += velocidadeRotacao;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  pitch -= velocidadeRotacao;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    float yawRad = yaw * DEG2RAD;
    float pitchRad = pitch * DEG2RAD;

    cameraFront.x = cos(yawRad) * cos(pitchRad);
    cameraFront.y = sin(pitchRad);
    cameraFront.z = sin(yawRad) * cos(pitchRad);

    float velCaminhada = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera = addVectors(camera, mulVector(cameraFront, velCaminhada));
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera = addVectors(camera, mulVector(cameraFront, -velCaminhada));
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwTerminate();
        glfwWindowShouldClose(window);
        running = 0;
    }
}
