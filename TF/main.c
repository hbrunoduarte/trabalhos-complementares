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
    
    int numCorposCelestes = 10;
    CorpoCeleste *sistemaSolar = malloc(sizeof(CorpoCeleste) * numCorposCelestes);

    sistemaSolar[SOL] = criarCorpoCeleste(1.9891e30, 1408.0f, VETOR_NULO,
                                         0.0f, 0.0f, 14.71*DEG2RAD,
                                         renderizarSol, getDadosSol(), NULL);

    sistemaSolar[MERCURIO] = criarCorpoCeleste(3.3011e23, 2439.7f, VETOR_NULO,
                                              57.9e6, 4.7e-3f, 1.08e-2f,
                                              renderizarPlaneta, getDadosPlaneta("imagens/mercurio/mercurio.jpg"), 
                                              &sistemaSolar[SOL]);
    
    sistemaSolar[VENUS] = criarCorpoCeleste(4.8675e24, 5243.0f, VETOR_NULO,
                                           108.2e6, 1.0e-3f, 2.8e-2f,
                                           renderizarVenus, getDadosVenus(), &sistemaSolar[SOL]);

    sistemaSolar[TERRA] = criarCorpoCeleste(5.9722e24, 5515.0f, VETOR_NULO,
                                           150e6, 2e-3f, 7.27e-2f,
                                           renderizarTerra, getDadosTerra(), &sistemaSolar[SOL]);

    sistemaSolar[MARTE] = criarCorpoCeleste(6.4171e23, 3389.5f, VETOR_NULO,
                                           227.9e6, 1.5e-3f, 7.0e-2f, 
                                           renderizarPlaneta, getDadosPlaneta("imagens/marte/marte.jpg"),
                                           &sistemaSolar[SOL]);

    sistemaSolar[JUPITER] = criarCorpoCeleste(1.8982e27, 1326.0f, VETOR_NULO,
                                             778.5e6, 1.7e-4f, 1.7e-1f,
                                             renderizarPlaneta, getDadosPlaneta("imagens/jupiter/jupiter.jpg"), 
                                             &sistemaSolar[SOL]);

    sistemaSolar[SATURNO] = criarCorpoCeleste(5.6834e26, 687.0f, VETOR_NULO,
                                              1433.5e6, 4.4e-4f, 5.3e-2f,
                                              renderizarSaturno, getDadosSaturno(), &sistemaSolar[SOL]);

    sistemaSolar[URANO] = criarCorpoCeleste(8.6810e25, 1270.0f, VETOR_NULO,
                                           2871e6, 2.4e-5f, 9.7e-2f,
                                           renderizarPlaneta, getDadosPlaneta("imagens/urano/urano.jpg"), 
                                           &sistemaSolar[SOL]);

    sistemaSolar[NETUNO] = criarCorpoCeleste(1.0241e26, 1638.0f, VETOR_NULO,
                                            4495e6, 1.2e-5f, 1.1e-1f,
                                            renderizarPlaneta, getDadosPlaneta("imagens/netuno/netuno.jpg"), 
                                            &sistemaSolar[SOL]);

    sistemaSolar[LUA] = criarCorpoCeleste(7.346e22, 3344.0f, VETOR_NULO,
                                            3844000.0f, 0.22f, 7.27e-5f,
                                            renderizarPlaneta, getDadosPlaneta("imagens/lua/lua.jpg"), 
                                            &sistemaSolar[TERRA]);


    for (int i = 0; i < numCorposCelestes; i++) {
        sistemaSolar[i].VBO = VBO;
        sistemaSolar[i].totalVertices = totalVertices;
    }

    carregarShaderSombras();
    setFramebufferSombras(sistemaSolar+1, numCorposCelestes-1);

    printf("\n");
    printf("=======================================================\n");
    printf(" CONTROLES DE VISUALIZACAO:\n");
    printf("=======================================================\n");
    printf(" [W] / [S]        - Mover para frente / tras\n");
    printf(" [A] / [D]        - Mover para a esquerda / direita\n");
    printf(" [Setas / Mouse] - Mexer a camera\n");
    printf(" [Scroll]         - Ajuste de zoom\n");
    printf(" [SHIFT ESQ]      - Segurar para movimento rápido\n");
    printf(" [ESPACO]         - Mover para cima\n");
    printf(" [CTRL ESQ]       - Mover para baixo\n");
    printf("=======================================================\n");
    printf(" OUTROS CONTROLES:\n");
    printf("=======================================================\n");
    printf("  [K] / [L] - Pausar / Retomar movimento dos planetas\n");
    printf("  [Mouse 1] - Acelerar o tempo (Botao Esq.)\n");
    printf("  [Mouse 2] - Desacelerar o tempo (Botao Dir.)\n");
    printf("  [ESC]     - Fechar o programa\n");
    printf("=======================================================\n");

    DadosUniverso *universo = inicializarUniverso();

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

        renderizarUniverso(universo, &camera);

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