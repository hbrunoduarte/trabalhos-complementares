#include "solar.h"
#include "terra.h"
#include "sol.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400.0, lastY = 300.0;
booleano running = 1;

// Posição do jogador no mundo (x, y, z)
vector camera = {0.0f, 0.0f, 5.0f};
vector cameraFront = {0.0f, 0.0f, 0.0f};
float camStep = 0.05f;

float yaw = -90.0f; // esquerda/direita
float pitch = 0.0f; // cima/baixo

/*typedef struct {

    float posicao[3];
    float velocidade[3];

    float massa;
    float densidade;
    float raio;

    float cor[3];
    booleano brilha;

} planeta;*/

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

// Função para processar o input do teclado
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

GLuint carregarTextura(const char* caminho) {
    GLuint texturaID;
    glGenTextures(1, &texturaID);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    // Configura o comportamento da textura (Filtro Suave e Repetição)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // O OpenGL inverte o eixo Y em relação às imagens comuns, então pedimos ao stb para consertar
    stbi_set_flip_vertically_on_load(1);

    int largura, altura, canaisCores;
    // Carrega os pixels da imagem para a memória RAM
    unsigned char *dados = stbi_load(caminho, &largura, &altura, &canaisCores, 0);
    
    if (dados) {
        // Envia para a GPU
        GLenum formato = (canaisCores == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, dados);
    } else {
        printf("Falha ao carregar a textura: %s\n", caminho);
    }
    
    // Libera a memória RAM, pois a GPU já guardou a imagem
    stbi_image_free(dados);

    return texturaID;
}


void addVertice(float* array, int* index, float r, float phi, float theta) {
    float x = r * sin(phi) * cos(theta);
    float y = r * cos(phi);
    float z = r * sin(phi) * sin(theta);
    
    array[(*index)++] = x;
    array[(*index)++] = y;
    array[(*index)++] = z;
    
    array[(*index)++] = x / r;
    array[(*index)++] = y / r;
    array[(*index)++] = z / r;

    array[(*index)++] = 1.0f - theta / (2.0f * M_PI);
    array[(*index)++] = 1.0f - phi / M_PI;

    array[(*index)++] = -sin(theta);
    array[(*index)++] = 0.0f;
    array[(*index)++] = cos(theta);
}

EsferaMesh* criarEsferaArray(float r, unsigned int pTheta, unsigned int pPhi) {
    EsferaMesh *malha = malloc(sizeof(EsferaMesh));
    
    // Cada "quadrado" na malha da esfera precisa de 2 triângulos.
    // 2 triângulos * 3 vértices cada = 6 vértices por quadrado.
    malha->numVertices = pPhi * pTheta * 6;
    
    // Cada vértice ocupa 6 floats na memória (3 de posição + 3 de normal + 2 de textura)
    malha->dados = malloc(malha->numVertices * SPHERE_INFO * sizeof(float));
    
    int index = 0;
    float dTheta = (float)(2.0 * M_PI / pTheta);
    float dPhi = (float)(M_PI / pPhi);

    // Loop externo (Latitude)
    for (unsigned int i = 0; i < pPhi; ++i) {
        float phi1 = i * dPhi;
        float phi2 = (i + 1) * dPhi;

        // Loop interno (Longitude)
        for (unsigned int j = 0; j < pTheta; ++j) {
            float theta1 = j * dTheta;
            float theta2 = (j + 1) * dTheta;

            // Para cada passo do loop, calculamos os 4 cantos de um quadrado da superfície
            // V1: Topo-Esquerda
            // V2: Baixo-Esquerda
            // V3: Topo-Direita
            // V4: Baixo-Direita

            // Triângulo 1 (V1, V2, V3)
            addVertice(malha->dados, &index, r, phi1, theta1); // V1
            addVertice(malha->dados, &index, r, phi2, theta1); // V2
            addVertice(malha->dados, &index, r, phi1, theta2); // V3

            // Triângulo 2 (V3, V2, V4)
            addVertice(malha->dados, &index, r, phi1, theta2); // V3
            addVertice(malha->dados, &index, r, phi2, theta1); // V2
            addVertice(malha->dados, &index, r, phi2, theta2); // V4
        }
    }
    
    return malha; // Retorna o array carregado
}

void initLighting() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat luzAmbiente[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
    GLfloat luzDifusa[]   = { 0.8f, 0.8f, 0.8f, 1.0f }; 
    GLfloat luzPosicao[]  = { 5.0f, 5.0f, 5.0f, 0.0f }; 

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, luzPosicao);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Falha ao inicializar o GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Minha Primeira Janela GLFW em C", NULL, NULL);
    
    if (!window) {
        fprintf(stderr, "Falha ao criar a janela GLFW\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; 
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Falha ao inicializar o GLEW\n");
        return -1;
    }

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    initLighting();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    
    EsferaMesh *terraMesh = criarEsferaArray(1.0f, 40, 40);
    
    // Guardamos o número de vértices pois vamos deletar a struct em seguida
    int totalVertices = terraMesh->numVertices;

    // --- PASSO 2: ENVIAR PARA A GPU (VRAM) ---
    GLuint VBO;
    glGenBuffers(1, &VBO); // Pede um "ID" de buffer para o OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Diz que este será o buffer ativo no momento
    
    // O tamanho do buffer é: número de vértices * 6 floats por vértice * tamanho do float
    long tamanhoBytes = totalVertices * SPHERE_INFO * sizeof(float);
    
    // Copia os dados da RAM para a Placa de Vídeo
    glBufferData(GL_ARRAY_BUFFER, tamanhoBytes, terraMesh->dados, GL_STATIC_DRAW);

    carregarTexturaSol();
    carregarTexturaTerra();
    
    free(terraMesh->dados);
    free(terraMesh);

    // Loop Principal
    while (!glfwWindowShouldClose(window) && running) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateCamera(window);

        criarSol(currentFrame, VBO, totalVertices);
        criarTerra(&camera, &cameraFront, VBO, currentFrame, totalVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}