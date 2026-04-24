// gcc snowman.c -lglut -lGL -lGLU -lm -o snowman.out && ./snowman.out
// para compilar e executar

#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define COLOR(r,g,b) r/255.0,g/255.0,b/255.0

#define TAM_GRADE_NEVE_GLOBO 60
#define TAM_PASSO_NEVE_GLOBO 0.05f

float snowHeights[TAM_GRADE_NEVE_GLOBO][TAM_GRADE_NEVE_GLOBO];

#define GLOBE_RADIUS 1.2f

typedef enum {
    GROUND,
    UP,
    DRIFT,
    DOWN
} Stage;

typedef struct {
    float x, y, z;
    Stage stage;
    
    // Variáveis do estágio UP
    float elevation; // Ângulo de subida (de -PI/2 no fundo até +PI/2 no topo)
    float azimuth;   // Ângulo ao redor do globo (de 0 a 2*PI)
    
    // Variáveis do estágio Drift
    float driftTimer;
    float verticalMomentum;
    
    // Variáveis do estágio DOWN
    float fallSpeed;
    float flutterAngle;

    float freqX, freqZ, ampX, ampZ;
} SnowFlake;

#define MAX_PARTICLES 1000
#define AVERAGE_PARTICLES 500

SnowFlake particles[MAX_PARTICLES];

static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;
static GLfloat yCam = -1.0f;

void shakeGlobe();

void ChangeSize(int w, int h) {
    GLfloat fAspect;

    // previne divisão por zero
    if(h == 0)
        h = 1;

    // define a área da tela que o opengl pode "desenhar"
    glViewport(0, 0, w, h);

    // declaramos o aspecto da tela 
    fAspect = (GLfloat)w/(GLfloat)h;

    // entra no modo para mexer na câmera
    glMatrixMode(GL_PROJECTION);  
    // desfaz configurações anteriores da câmera que podiam
    // ter sido feitas
    glLoadIdentity();  
    // define o FOV (field of view), o aspecto da tela, 
    // o plano de corte "near" e o plano de corte "far"
    gluPerspective(35.0f, fAspect, 1.0, 40.0);

    // voltamos pro modo de mexer nos objetos ao
    // invés da câmera
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity();
}

void NormalKeys(unsigned char key, int x, int y) {
    // 27 é o código ascii para a tecla esc
    if (key == 27) {
        exit(0);
    }

    if(key == 'w' || key == 'W')
        yCam -= 0.1f;

    if(key == 's' || key == 'S')
        yCam += 0.1f;

    if (key == 'q' || key == 'Q')
        shakeGlobe();

    glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y) {
    if(key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if(key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    // evita que yRot tome um valor muito alto
    yRot = (GLfloat)((const int)yRot % 360);  

    if(key == GLUT_KEY_UP)
        xRot += 5.0f;

    if(key == GLUT_KEY_DOWN)
        xRot -= 5.0f;

    // evita que xRot tome um valor muito alto
    xRot = (GLfloat)((const int)xRot % 360);  

    // apagar a tela e rodar RenderScene de novo
    glutPostRedisplay();  
}

void desenhaEixos() {
    float tamanho = 10.0f; // Define o comprimento das linhas

    // Desativa a iluminação temporariamente se você estiver usando, 
    // para garantir que as cores dos eixos fiquem puras e brilhantes.
    glDisable(GL_LIGHTING); 

    // Aumenta a espessura da linha para ficar mais fácil de ver
    glLineWidth(2.0f); 

    glBegin(GL_LINES);
    
        // Eixo X (Vermelho) - Vai da origem (0,0,0) para a direita
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(tamanho, 0.0f, 0.0f);

        // Eixo Y (Verde) - Vai da origem (0,0,0) para cima
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, tamanho, 0.0f);

        // Eixo Z (Azul) - Vai da origem (0,0,0) "para fora" da tela, na sua direção
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, tamanho);
        
    glEnd();

    // Restaura a espessura da linha para o padrão
    glLineWidth(1.0f);
    
    glEnable(GL_LIGHTING); // Reative a iluminação se estiver usando
}

void createSnowman(GLUquadricObj *pObj) {
    // -- CABECA --

	// cor branca
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.0f);
        glutSolidSphere(0.24f, 26, 13);
	glPopMatrix();

    // -- NARIZ --

    // cor laranja
	glColor3f(1.0f, 0.4f, 0.51f);  

	glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.2f);
        glutSolidCone(0.04f, 0.3f, 26, 13);
	glPopMatrix();

    // -- OLHOS --

    // cor preta
    glColor3f(0.0f, 0.0f, 0.0f);

    // olho esquerdo
    glPushMatrix();
        glTranslatef(-0.08f, 1.08f, 0.2f);
        glutSolidSphere(0.03f, 10, 10);
    glPopMatrix();

    // olho direito
    glPushMatrix();
        glTranslatef(0.08f, 1.08f, 0.2f);
        glutSolidSphere(0.03f, 10, 10);
    glPopMatrix();

    // -- BARRIGA --

    // cor branca
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
        glTranslatef(0.0f, 0.55f, 0.0f);
        glutSolidSphere(0.30f, 26, 13);
    glPopMatrix();

    // -- BASE --

    glPushMatrix();
        glTranslatef(0.0f, 0.1f, 0.0f);
        glutSolidSphere(0.38f, 26, 13);
    glPopMatrix();

    // -- BRACOS -- 

    // cor marrom 
    glColor3f(0.36f, 0.20f, 0.09f);

    // braco direito
    glPushMatrix();
        glTranslatef(0.28f, 0.60f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.02f, 0.5f, 10, 10);
    glPopMatrix();

    // braco esquerdo
    glPushMatrix();
        glTranslatef(-0.28f, 0.60f, 0.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.02f, 0.5f, 10, 10);
    glPopMatrix();

    // -- CHAPEU --

    // cor cinza bem escuro
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
        glTranslatef(0.0f, 1.18f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.25f, 0.25f, 0.05f, 26, 13);

        // tampa de cima
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.05f);
            gluDisk(pObj, 0.0f, 0.25f, 26, 1);
        glPopMatrix();
            
        // tampa de baixo
        glPushMatrix();
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(pObj, 0.0f, 0.25f, 26, 1);
        glPopMatrix();
    glPopMatrix();

    // cor vermelha
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
        glTranslatef(0.0, 1.23, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.16f, 0.16f, 0.05f, 26, 13);
    glPopMatrix();

    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
        glTranslatef(0.0, 1.28, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.16f, 0.16f, 0.1f, 26, 13);

        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.1f);
            gluDisk(pObj, 0.0f, 0.16f, 26, 1);
        glPopMatrix();
    glPopMatrix();
}

void createSnow(float raioBase) {
    srand(time(NULL));
    for(int x = 0; x < TAM_GRADE_NEVE_GLOBO; x++) {
        for(int z = 0; z < TAM_GRADE_NEVE_GLOBO; z++) {
            // Gera um ruído leve (irregularidade da neve) entre 0.0 e 0.08
            float ruido = ((rand() % 100) / 100.0f) * 0.08f; 
            
            // Altura base fixada em -0.15f para afundar a base do boneco na neve
            snowHeights[x][z] = -0.15f + ruido;
        }
    }
}

void drawSnow(float raioBase) {
    glColor3f(1.0f, 1.0f, 1.0f);

    for(int z = 0; z < TAM_GRADE_NEVE_GLOBO - 1; z++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < TAM_GRADE_NEVE_GLOBO; x++) {
            
            // --- CÁLCULO DO PONTO ATUAL ---
            float mx1 = (x - TAM_GRADE_NEVE_GLOBO / 2.0f) * TAM_PASSO_NEVE_GLOBO;
            float mz1 = (z - TAM_GRADE_NEVE_GLOBO / 2.0f) * TAM_PASSO_NEVE_GLOBO;
            float dist1 = sqrt(mx1*mx1 + mz1*mz1);
            
            // Puxa o vértice para a borda se ele passar do raio estipulado
            if (dist1 > raioBase) {
                mx1 = (mx1 / dist1) * raioBase; 
                mz1 = (mz1 / dist1) * raioBase; 
            }
            glNormal3f(0.0f, 1.0f, 0.0f); // Aponta a normal para cima para o reflexo da luz
            glVertex3f(mx1, snowHeights[x][z], mz1);

            // --- CÁLCULO DO PONTO DA FRENTE ---
            float mx2 = (x - TAM_GRADE_NEVE_GLOBO / 2.0f) * TAM_PASSO_NEVE_GLOBO;
            float mz2 = ((z + 1) - TAM_GRADE_NEVE_GLOBO / 2.0f) * TAM_PASSO_NEVE_GLOBO;
            float dist2 = sqrt(mx2*mx2 + mz2*mz2);
            
            // Repete a checagem de borda
            if (dist2 > raioBase) {
                mx2 = (mx2 / dist2) * raioBase;
                mz2 = (mz2 / dist2) * raioBase;
            }
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(mx2, snowHeights[x][z+1], mz2);
        }
        glEnd();
    }
}

void drawSnowFlakes() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPushMatrix();
        glScalef(1.2f, 1.2f, 1.2f);
        glTranslatef(0.0f, 0.6f, 0.0f);

        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].stage != GROUND) {
                glColor4f(COLOR(0xFF, 0xFF, 0xFF), 0.5f);
                glVertex3f(particles[i].x, particles[i].y, particles[i].z);
            }
        }
        glEnd();
    glPopMatrix();

    glDisable(GL_BLEND);
}

void shakeGlobe() {
    int particlesToShake = ((rand() % 100)/100.0) * MAX_PARTICLES;
    if (particlesToShake < AVERAGE_PARTICLES)
        particlesToShake = AVERAGE_PARTICLES;

    int shakedParticles = 0;
    for (int i = 0; i < MAX_PARTICLES && shakedParticles < particlesToShake; i++) {
        if (particles[i].stage == GROUND) {

            shakedParticles++;

            particles[i].stage = UP;
            
            // Adiciona uma variação aleatória para não subirem em anel
            float randomOffset = ((float)(rand() % 100) / 100.0f) * (M_PI / 4.0f);
            particles[i].elevation = -M_PI / 2.0f + randomOffset; 
            
            particles[i].azimuth = ((float)(rand() % 360)) * (M_PI / 180.0f);

            particles[i].fallSpeed = 0.5f + ((float)(rand() % 100) / 100.0f) * 1.5f;

            particles[i].flutterAngle = ((float)(rand() % 360)) * (M_PI / 180.0f);
            
            // Cada floco tem sua própria curva
            particles[i].freqX = 0.8f + ((float)(rand() % 100) / 100.0f) * 0.7f;
            particles[i].freqZ = 1.1f + ((float)(rand() % 100) / 100.0f) * 0.7f;
            
            // Alguns flocos vão longe, outros caem quase retos
            particles[i].ampX = 0.3f + ((float)(rand() % 100) / 100.0f) * 1.0f;
            particles[i].ampZ = 0.3f + ((float)(rand() % 100) / 100.0f) * 1.0f;
        }
    }
}

void updateSnowflakes(float deltaTime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        
        if (particles[i].stage == UP) {

            float upSpeed = 0.5f + ((float)(rand() % 100) / 100.0f) * 1.5f;
            particles[i].elevation += upSpeed * deltaTime;
            
            particles[i].x = (0.9 * GLOBE_RADIUS) * cos(particles[i].elevation) * cos(particles[i].azimuth);
            particles[i].y = (0.9 * GLOBE_RADIUS) * sin(particles[i].elevation);
            particles[i].z = (0.9 * GLOBE_RADIUS) * cos(particles[i].elevation) * sin(particles[i].azimuth);

            if (particles[i].y >= GLOBE_RADIUS * 0.8f) {
                particles[i].stage = DRIFT;
                particles[i].driftTimer = ((rand() % 10)/10.0 + 0.1) * 0.3f;
                particles[i].verticalMomentum = 0.4f;
            }
        }
        
        else if (particles[i].stage == DRIFT) {
            
            particles[i].x *= (1.0f - 2.0f * deltaTime); 
            particles[i].z *= (1.0f - 2.0f * deltaTime); 
            
            particles[i].y += particles[i].verticalMomentum * deltaTime;
            particles[i].verticalMomentum -= 0.8f * deltaTime;
            
            particles[i].driftTimer -= deltaTime;
            if (particles[i].driftTimer <= 0.0f) particles[i].stage = DOWN;
        }
        
        else if (particles[i].stage == DOWN) {

           particles[i].y -= particles[i].fallSpeed * deltaTime;
    
            particles[i].flutterAngle += 3.0f * deltaTime;
            
            particles[i].x += sin(particles[i].flutterAngle * particles[i].freqX) * particles[i].ampX * deltaTime;
            particles[i].z += cos(particles[i].flutterAngle * particles[i].freqZ) * particles[i].ampZ * deltaTime;

            float distanceSq = (particles[i].x * particles[i].x) + 
                               (particles[i].y * particles[i].y) + 
                               (particles[i].z * particles[i].z);
                               
            if (distanceSq >= ((GLOBE_RADIUS-0.01f) * (GLOBE_RADIUS-0.01f))) {
                if (particles[i].y < 1.0f)
                    particles[i].stage = GROUND;
                // Para não vazar o vidro, normalizamos o vetor (X,Y,Z) e multiplicamos pelo raio
                float dist = sqrt(distanceSq);
                particles[i].x = (particles[i].x / dist) * GLOBE_RADIUS;
                particles[i].y = (particles[i].y / dist) * GLOBE_RADIUS;
                particles[i].z = (particles[i].z / dist) * GLOBE_RADIUS;
            }
        }
    }
}

void createSnowGlobe(GLUquadricObj *pObj) {

    glScalef(1.2f, 1.2f, 1.2f);

    // Base do Globo

    glColor3f(COLOR(0,0,0));
    glPushMatrix();

        // Tronco de Cone para as laterais
        glPushMatrix();    
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -0.6f);
            glDisable(GL_CULL_FACE);
            gluCylinder(pObj, 1.0f, 0.8f, 0.3f, 40, 40);
            glEnable(GL_CULL_FACE);
        glPopMatrix();

        // Disco para fechar o tronco inferiormente
        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -0.6f);
            glDisable(GL_CULL_FACE);
            gluDisk(pObj, 0.0f, 1.0f, 40, 40);
            glEnable(GL_CULL_FACE);
        glPopMatrix();
    glPopMatrix();

    // Globo

    // Ativa a mistura de cores
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Impede que a esfera escreva no depth buffer
    glDepthMask(GL_FALSE); 

    glColor4f(COLOR(0xFF, 0xFF, 0xFF), 0.3f);
    glPushMatrix();
        glTranslatef(0.0f, 0.6f, 0.0f);
        glutSolidSphere(GLOBE_RADIUS, 70, 70);
    glPopMatrix();

    // Restaura as configurações para o próximo frame
    glDepthMask(GL_TRUE); 
    glDisable(GL_BLEND);

}

void RenderScene(void) {
    GLUquadricObj *pObj = gluNewQuadric();

    // limpa a janela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Libera a escrita no Depth Buffer
    glDepthMask(GL_TRUE);
  
    // move a area pra "desenhar" pra não ficar em cima da câmera
    glPushMatrix();
	glTranslatef(0.0f, yCam, -8.0f);  
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);

    // desenhaEixos();

    createSnowman(pObj);

    glPushMatrix();
        for (float h = 1.15f; h > 0.8f; h -= 0.01f) {
            drawSnow(h);
            
            // Move 0.01 unidades para baixo no eixo Y para a PRÓXIMA camada
            glTranslatef(0.0f, -0.01f, 0.0f); 
        }
    glPopMatrix();

    updateSnowflakes(0.01);
    drawSnowFlakes();

    createSnowGlobe(pObj);

    ////////////////////////////////////////////

    // deleta a quadrica criada
    gluDeleteQuadric(pObj);
    // volta na posicao global
    glPopMatrix();  
    // mostra a tela
    glutSwapBuffers();  
}

void init() {
    // define a cor das luzes em RGBA
    GLfloat  whiteLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };  
    GLfloat  sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };  

    // define a posição da luz
    GLfloat  lightPos[] = { -10.f, 5.0f, 5.0f, 1.0f };  

    // culling - nao desenhe a parte de dentro dos sólidos; isso é feito
    // definindo a parte da frente dos triangulos da malha como o lado onde
    // os pontos dos triângulos giram no sentido anti-horário
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    // ativa as luzes e coloca uma luz ambiente global (whiteLight)
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);

    // define a luz ambiente da LIGHT0 (existem LIGHT0 até LIGHT7)
    glLightfv(GL_LIGHT0,GL_AMBIENT,sourceLight);  
    // define a luz difusa da lampada
    glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);  
    // define a localização da lampada na posição definida anteriormente
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);  
    // ativa a LIGHT0
    glEnable(GL_LIGHT0);

    // define o material  
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // cor que será usada para "apagar" os frames (cor de fundo)
    glClearColor(0.25f, 0.25f, 0.50f, 1.0f);
    
    createSnow(1.15f);
}

void timer(int value) {
    glutPostRedisplay();               // Manda redesenhar a tela
    glutTimerFunc(16, timer, 0);       // Agenda a próxima chamada em 16ms
}

int main(int argc, char* argv[]) {

    // inicia o glut e cria a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Boneco de Neve");

    // configurações da câmera
    glutReshapeFunc(ChangeSize);  
    // configuracoes das teclas normais
    glutKeyboardFunc(NormalKeys);
    // configurações das teclas especiais
    glutSpecialFunc(SpecialKeys);
    // configuracoes da cena
    glutDisplayFunc(RenderScene);

    // função chamada pra inicialização de luzes
    // e renderização
    init();

    // Inicia o loop de animação
    glutTimerFunc(0, timer, 0);

    // entra no loop principal
    glutMainLoop();

    return 0;
}

