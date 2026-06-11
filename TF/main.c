#include "libs/config.h"

extern float lastX;
extern float lastY;
extern float yaw;
extern float pitch;

extern vector camera;
extern vector cameraFront;

extern float deltaTime;
extern float lastFrame;
extern booleano running;

void updateCamera(GLFWwindow *window) {

    processarInput(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    vector alvo = addVectors(camera, cameraFront);

    gluLookAt(
        camera.x, camera.y, camera.z,
        alvo.x, alvo.y, alvo.z,
        0.0f, 1.0f, 0.0f
    );
}

void initLighting() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat luzAmbiente[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
    GLfloat luzDifusa[]   = { 0.8f, 0.8f, 0.8f, 1.0f }; 
    GLfloat luzPosicao[]  = { 0.0f, 0.0f, 0.0f, 0.0f }; 

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, luzPosicao);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}

int main() {
    
    GLFWwindow *window = configurarTela();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    framebufferSizeCallback(window, width, height);
    
    EsferaMesh *terraMesh = criarEsferaArray(1.0f, 60, 60);
    int totalVertices = terraMesh->numVertices;

    // --- PASSO 2: ENVIAR PARA A GPU (VRAM) ---
    GLuint VBO;
    glGenBuffers(1, &VBO); // Pede um "ID" de buffer para o OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Diz que este será o buffer ativo no momento
    
    long tamanhoBytes = totalVertices * SPHERE_INFO * sizeof(float);
    
    // Copia os dados da RAM para a Placa de Vídeo
    glBufferData(GL_ARRAY_BUFFER, tamanhoBytes, terraMesh->dados, GL_STATIC_DRAW);

    CorpoCeleste terra;
    terra.massa = 1.0f;
    terra.posicao = (vector){60.0f, 0.0f, 0.0f};
    terra.velocidade = (vector){0.0f, 0.0f, 15.0f};
    terra.raioVisual = 0.5f;
    terra.VBO = VBO;
    terra.totalVertices = totalVertices;
    terra.renderizar = renderizarTerra; // Aponta para a função em terra.c
    terra.dadosVisuais = getDadosTerra();

    CorpoCeleste sol;
    sol.massa = 1000.0f;
    sol.posicao = (vector){0.0f, 0.0f, 0.0f};
    sol.velocidade = (vector){0.0f, 0.0f, 0.0f};
    sol.raioVisual = 54.5f;
    sol.VBO = VBO;
    sol.totalVertices = totalVertices;
    sol.renderizar = renderizarSol; // Aponta para a função em sol.c
    sol.dadosVisuais = getDadosSol();
    
    CorpoCeleste sistemaSolar[] = {sol, terra};
    int numCorposCelestes = 2;

    free(terraMesh->dados);
    free(terraMesh);

    while (!glfwWindowShouldClose(window) && running) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateCamera(window);

        for (int i = 0; i < numCorposCelestes; i++)
            sistemaSolar[i].renderizar(&sistemaSolar[i], &camera, &cameraFront, currentFrame);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i = 0; i < numCorposCelestes; i++)
        free(sistemaSolar[i].dadosVisuais);

    glfwTerminate();
    return 0;
}