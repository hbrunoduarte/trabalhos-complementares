#include "libs/config.h"

extern float lastX;
extern float lastY;
extern float yaw;
extern float pitch;
extern int width;
extern int height;
extern float speedTime;

extern vector camera;
extern vector cameraFront;

extern float deltaTime;
extern float lastFrame;
extern booleano running;
extern booleano pause;

extern mat4 globalViewMatrix;

int main() {
    
    GLFWwindow *window = configurarTela();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glfwGetFramebufferSize(window, &width, &height);
    framebufferSizeCallback(window, width, height);
    
    EsferaMesh *terraMesh = criarEsferaArray(1.0f, 60, 60);
    int totalVertices = terraMesh->numVertices;

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    long tamanhoBytes = totalVertices * SPHERE_INFO * sizeof(float);
    
    glBufferData(GL_ARRAY_BUFFER, tamanhoBytes, terraMesh->dados, GL_STATIC_DRAW);

    free(terraMesh->dados);
    free(terraMesh);

    CorpoCeleste sol = criarCorpoCeleste(1.9891e30, 1408.0f, VETOR_NULO,
                                         0.0f, 0.0f, 14.71*DEG2RAD,
                                         renderizarSol, getDadosSol(), NULL);
    
    CorpoCeleste terra = criarCorpoCeleste(5.9722e24, 5515.0f, VETOR_NULO,
                                           150e6, 2e-3f, 7.27e-2f,
                                           renderizarTerra, getDadosTerra(), &sol);

    CorpoCeleste terra2 = criarCorpoCeleste(7.346e22, 3344.0f, VETOR_NULO,
                                            3844000.0f, 0.22f, 7.27e-5f,
                                            renderizarTerra, getDadosTerra(), &terra);
    
    int numCorposCelestes = 3;
    CorpoCeleste *sistemaSolar = malloc(sizeof(CorpoCeleste) * numCorposCelestes);
    sistemaSolar[0] = sol;
    sistemaSolar[1] = terra;
    sistemaSolar[2] = terra2;

    sistemaSolar[1].orbita = &sistemaSolar[0]; // A Terra orbita o Sol do array
    sistemaSolar[2].orbita = &sistemaSolar[1];

    for (int i = 0; i < numCorposCelestes; i++) {
        sistemaSolar[i].VBO = VBO;
        sistemaSolar[i].totalVertices = totalVertices;
    }

    carregarShaderSombras();
    setFramebufferSombras(sistemaSolar+1, numCorposCelestes-1);

    while (!glfwWindowShouldClose(window) && running) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateCamera(window);

        if (!pause) {
            for (int i = 1; i < numCorposCelestes; i++) { // Ignora o Sol (0) para mantê-lo fixo no centro
                atualizarFisica(&sistemaSolar[i], speedTime * deltaTime);
            }
        }

        calcularSombras(window, sistemaSolar+1, numCorposCelestes-1);

        for (int i = 0; i < numCorposCelestes; i++)
            sistemaSolar[i].renderizar(&sistemaSolar[i], &camera, &cameraFront, currentFrame);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i = 0; i < numCorposCelestes; i++)
        free(sistemaSolar[i].dadosVisuais);
    free(sistemaSolar);

    glfwTerminate();
    return 0;
}