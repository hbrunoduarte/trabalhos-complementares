#include "libs/config.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400.0, lastY = 300.0;
int width;
int height;
booleano running = 1;
booleano pause = 0;

float speedTime = 1.0;

// Posição do jogador no mundo (x, y, z)
vector camera = {60.0f, 15.0f, 5.0f};
vector cameraFront = {0.0f, 0.0f, 0.0f};

float yaw = -90.0f; // esquerda/direita
float pitch = 0.0f; // cima/baixo

mat4 globalViewMatrix;
mat4 globalProjectionMatrix;

void updateCamera(GLFWwindow *window) {

    processarInput(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vector alvoVec = addVectors(camera, cameraFront);
    vec3 camUp = {0.0f, 1.0f, 0.0f};
    
    glm_lookat(camera.raw, alvoVec.raw, camUp, globalViewMatrix);
}

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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    float scrollSpeed = 0.5f;

    if (yoffset>0) {
        camera = addVectors(camera, mulVector(cameraFront, scrollSpeed));
    } else if (yoffset<0) {
        camera = addVectors(camera, mulVector(cameraFront, -scrollSpeed));
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        speedTime *= speedTime >= MAX_TIMESPEED ? 1 : 2;
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        speedTime /= speedTime <= MIN_TIMESPEED ? 1 : 2;
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

    float velCaminhada = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera = addVectors(camera, mulVector(cameraFront, velCaminhada));
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera = addVectors(camera, mulVector(cameraFront, -velCaminhada));
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        pause = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        pause = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwTerminate();
        glfwWindowShouldClose(window);
        running = 0;
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    if (height == 0) return; 

    glViewport(0, 0, width, height);

    float proporcaoAtual = (float)width / (float)height;

    glm_perspective(glm_rad(45.0f), proporcaoAtual, 0.1f, 300.0f, globalProjectionMatrix);
}

GLFWwindow* configurarTela() {

    if (!glfwInit()) {
        fprintf(stderr, "Falha ao inicializar o GLFW\n");
        exit(-1);
    }
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Sistema Solar", NULL, NULL);
    
    if (!window) {
        fprintf(stderr, "Falha ao criar a janela GLFW\n");
        glfwTerminate();
        exit(-1);
    }
    glfwPollEvents();
    glfwMaximizeWindow(window);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; 
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Falha ao inicializar o GLEW\n");
        exit(-1);
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}
