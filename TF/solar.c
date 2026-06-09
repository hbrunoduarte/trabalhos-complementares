#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

#define booleano char

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD (M_PI / 180.0f)

typedef struct {
    float x;
    float y;
    float z;
} vector;

vector addVectors(vector u, vector v) {
    vector sum;
    sum.x = u.x + v.x;
    sum.y = u.y + v.y;
    sum.z = u.z + v.z;
    return sum;
}

vector mulVector(vector u, float alpha) {
    vector mul;
    mul.x = u.x * alpha;
    mul.y = u.y * alpha;
    mul.z = u.z * alpha;
    return mul;
}

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


void criarEsfera(GLfloat r, unsigned int pTheta, unsigned int pPhi) {

	float theta = 0.0f, phi = 0.0f;
    
    // Passo (incremento) de cada ângulo baseado na quantidade de subdivisões
    float dTheta = (float)(2.0 * M_PI / pTheta); // 360 graus
    float dPhi = (float)(M_PI / pPhi);           // 180 graus

    // Loop externo percorre a latitude (de cima para baixo)
    for (unsigned int i = 0; i < pPhi; ++i) {
        phi = i * dPhi;
        float proximoPhi = (i + 1) * dPhi;

        // Inicia uma nova faixa de polígonos
        glBegin(GL_QUAD_STRIP);
        
        // Loop interno percorre a longitude (dando a volta na esfera)
        // Usamos <= pTheta para garantir que a volta se feche perfeitamente
        for (unsigned int j = 0; j <= pTheta; ++j) {
            theta = j * dTheta;

            // Vértice 1 (Latitude atual)
            float x1 = r * sin(phi) * cos(theta);
            float y1 = r * cos(phi);
            float z1 = r * sin(phi) * sin(theta);
            
            glNormal3f(x1/r, y1/r, z1/r);
            glVertex3f(x1, y1, z1);

            // Vértice 2 (Próxima latitude)
            float x2 = r * sin(proximoPhi) * cos(theta);
            float y2 = r * cos(proximoPhi);
            float z2 = r * sin(proximoPhi) * sin(theta);
            
            glNormal3f(x2/r, y2/r, z2/r);
            glVertex3f(x2, y2, z2);
        }
        
        glEnd();
    }
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

    // 2. Criar a janela (Largura, Altura, Título, Monitor, Compartilhamento)
    GLFWwindow* window = glfwCreateWindow(800, 600, "Minha Primeira Janela GLFW em C", NULL, NULL);
    
    // Verificar se a janela foi criada com sucesso
    if (!window) {
        fprintf(stderr, "Falha ao criar a janela GLFW\n");
        glfwTerminate();
        return -1;
    }

    // 3. Tornar o contexto OpenGL da janela o contexto atual
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    initLighting();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    
    // 4. Loop Principal (Loop de Renderização)
    while (!glfwWindowShouldClose(window) && running) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateCamera(window);

		glColor3f(1.0f, 0.0f, 0.0f);
        criarEsfera(1.0f, 10, 10);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}