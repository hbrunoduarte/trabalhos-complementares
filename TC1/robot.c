// gcc robot.c -lglut -lGL -lGLU -lm -o robot.out && ./robot.out
// para compilar e executar

#include <GL/glut.h>

static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;
static GLfloat yCam = -1.0f;

static GLfloat angOmbroDirX = 0.0f;
static GLfloat angCotoveloDirX = 0.0f;
static GLfloat angOmbroEsqX = 0.0f;
static GLfloat angCotoveloEsqX = 0.0f;

static GLfloat angCoxaDirX = 0.0f;
static GLfloat angJoelhoDirX = 0.0f;
static GLfloat angCoxaEsqX = 0.0f;
static GLfloat angJoelhoEsqX = 0.0f;

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
        yCam += 0.1f;

    if(key == 's' || key == 'S')
        yCam -= 0.1f;

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

void RenderScene(void) {
    GLUquadricObj *pObj = gluNewQuadric();

    // limpa a janela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
  
    // move a area pra "desenhar" pra não ficar em cima da câmera
    glPushMatrix();
	glTranslatef(0.0f, yCam, -5.0f);  
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);

    ////////////////////////////////////////////

    glColor3f(0.7f, 0.1f, 0.1f);

    glPushMatrix();
        // centro do tronco
        glTranslatef(0.0f, 1.05f, 0.0f);

        // tronco
        glPushMatrix();
            glTranslatef(0.0f, -0.25f, 0.0f);
            // glScalef(largura_x, altura_y, profundidade_z)
            glScalef(0.45f, 0.4f, 0.4f);
            // glutSolidCube(tamanho da base)
            glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, 0.20f, 0.0f);
            glScalef(0.6f, 0.5f, 0.4f);
            glutSolidCube(1.0f);
        glPopMatrix();

        // reator
        glPushMatrix();
            glTranslatef(0.0f, 0.2f, 0.21f);
            
            glColor3f(0.2f, 0.8f, 1.0f);
            // gluDisk(obj, raio_interno, raio_externo, fatias, loops)
            gluDisk(pObj, 0.05f, 0.15f, 20, 1);
            
            glColor3f(1.0f, 1.0f, 1.0f);
            // gluDisk(obj, raio_interno, raio_externo, fatias, loops)
            gluDisk(pObj, 0.0f, 0.05f, 20, 1);
        glPopMatrix();

        // braço direito
        glPushMatrix();
            // ombro
            glTranslatef(0.38f, 0.35f, 0.0f);
            // ponto de articulacao !!!
            glRotatef(angOmbroDirX, 1.0f, 0.0f, 0.0f);
            glColor3f(0.8f, 0.6f, 0.1f);
            glutSolidSphere(0.1f, 20, 20);

            // biceps
            glColor3f(0.7f, 0.1f, 0.1f);
            glPushMatrix();
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                gluCylinder(pObj, 0.07f, 0.06f, 0.35f, 20, 10);
            glPopMatrix();

            // cotovelo
            glTranslatef(0.0f, -0.35f, 0.0f);
            glPushMatrix();
                // ponto de articulacao !!!
                glRotatef(angCotoveloDirX, 1.0f, 0.0f, 0.0f);
                glColor3f(0.8f, 0.6f, 0.1f);
                glutSolidSphere(0.07f, 20, 20);

                // antebraco
                glColor3f(0.7f, 0.1f, 0.1f);
                glPushMatrix();
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.05f, 0.04f, 0.35f, 20, 10);
                glPopMatrix();

                // maozinha
                glTranslatef(0.0f, -0.32f, 0.0f);
                glColor3f(0.7f, 0.1f, 0.1f);
                glutSolidSphere(0.07f, 15, 15);
            glPopMatrix();
        glPopMatrix();

        // braço esquerdo
        glPushMatrix();
            // ombro
            glTranslatef(-0.38f, 0.35f, 0.0f);
            // ponto de articulacao !!!
            glRotatef(angOmbroEsqX, 1.0f, 0.0f, 0.0f);
            glColor3f(0.8f, 0.6f, 0.1f);
            glutSolidSphere(0.1f, 20, 20);

            // biceps
            glColor3f(0.7f, 0.1f, 0.1f);
            glPushMatrix();
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                gluCylinder(pObj, 0.07f, 0.06f, 0.35f, 20, 10);
            glPopMatrix();

            // cotovelo
            glTranslatef(0.0f, -0.35f, 0.0f);
            glPushMatrix();
                // ponto de articulacao !!!
                glRotatef(angCotoveloEsqX, 1.0f, 0.0f, 0.0f);
                glColor3f(0.8f, 0.6f, 0.1f);
                glutSolidSphere(0.07f, 20, 20);

                // antebraco
                glColor3f(0.7f, 0.1f, 0.1f);
                glPushMatrix();
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.05f, 0.04f, 0.35f, 20, 10);
                glPopMatrix();

                // maozinha
                glTranslatef(0.0f, -0.32f, 0.0f);
                glColor3f(0.7f, 0.1f, 0.1f);
                glutSolidSphere(0.07f, 15, 15);
            glPopMatrix();
        glPopMatrix();

        // quadril
        glPushMatrix();
            // posicao da base do tronco
            glTranslatef(0.0f, -0.45f, 0.0f);
            glColor3f(0.8f, 0.6f, 0.1f);

            glPushMatrix();
                glScalef(0.6f, 0.1f, 0.4f);
                glutSolidCube(1.04f);
            glPopMatrix();

            glTranslatef(0.0f, -0.1f, 0.0f);
            glPushMatrix();
                glScalef(0.2f, 0.1f, 0.4f);
                glutSolidCube(1.04f);
            glPopMatrix();
        glPopMatrix();

        // perna direita
        glPushMatrix();
            // base do torso
            glTranslatef(0.18f, -0.55f, 0.0f);
            // ponto de articulacao !!!
            glRotatef(angCoxaDirX, 1.0f, 0.0f, 0.0f);

            glColor3f(0.8f, 0.6f, 0.1f);
            // bola da coxa
            glutSolidSphere(0.12f, 20, 20);

            // coxa
            glColor3f(0.7f, 0.1f, 0.1f);
            glPushMatrix();
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                gluCylinder(pObj, 0.08f, 0.07f, 0.45f, 20, 10);
            glPopMatrix();

            glTranslatef(0.0f, -0.45f, 0.0f);
            glPushMatrix();
                // ponto de articulacao !!!
                glRotatef(angJoelhoDirX, 1.0f, 0.0f, 0.0f);

                // joelho
                glColor3f(0.8f, 0.6f, 0.1f);
                glutSolidSphere(0.09f, 20, 20);

                // canela
                glColor3f(0.7f, 0.1f, 0.1f);
                glPushMatrix();
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.07f, 0.06f, 0.40f, 20, 10);
                glPopMatrix();

                // pé
                glTranslatef(0.0f, -0.40f, 0.05f);
                glColor3f(0.8f, 0.6f, 0.1f);
                glPushMatrix();
                    glScalef(1.0f, 0.5f, 1.5f);
                    glutSolidSphere(0.08f, 15, 15);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        // perna esquerda
        glPushMatrix();
            // base do torso
            glTranslatef(-0.18f, -0.55f, 0.0f);
            // ponto de articulacao !!!
            glRotatef(angCoxaDirX, 1.0f, 0.0f, 0.0f);

            glColor3f(0.8f, 0.6f, 0.1f);
            // bola da coxa
            glutSolidSphere(0.12f, 20, 20);

            // coxa
            glColor3f(0.7f, 0.1f, 0.1f);
            glPushMatrix();
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                gluCylinder(pObj, 0.08f, 0.07f, 0.45f, 20, 10);
            glPopMatrix();

            glTranslatef(0.0f, -0.45f, 0.0f);
            glPushMatrix();
                // ponto de articulacao !!!
                glRotatef(angJoelhoDirX, 1.0f, 0.0f, 0.0f);

                // joelho
                glColor3f(0.8f, 0.6f, 0.1f);
                glutSolidSphere(0.09f, 20, 20);

                // canela
                glColor3f(0.7f, 0.1f, 0.1f);
                glPushMatrix();
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.07f, 0.06f, 0.40f, 20, 10);
                glPopMatrix();

                // pé
                glTranslatef(0.0f, -0.40f, 0.05f);
                glColor3f(0.8f, 0.6f, 0.1f);
                glPushMatrix();
                    glScalef(1.0f, 0.5f, 1.5f);
                    glutSolidSphere(0.08f, 15, 15);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        // cabeca
        glTranslatef(0.0f, 0.7f, 0.0f);

        glColor3f(0.7f, 0.1f, 0.1f);
        glutSolidSphere(0.25f, 15, 15);

        // mascara
        glPushMatrix();
            // parte de cima + olhos
            glPushMatrix();
                glColor3f(0.8f, 0.6f, 0.1f);
                glTranslatef(0.0f, 0.05f, 0.2f);
                glPushMatrix();
                    glScalef(0.78f, 0.28f, 0.2f);
                    glutSolidCube(0.5f);
                glPopMatrix();

                glColor3f(0.2f, 0.8f, 1.0f);
                glTranslatef(0.0f, 0.0f, 0.06f);

                // olho direito
                glPushMatrix();
                    glTranslatef(0.12f, 0.0f, 0.0f);
                    glScalef(0.12f, 0.05f, 0.01f);
                    glutSolidCube(1.0f);
                glPopMatrix();

                // olho esquerdo
                glPushMatrix();
                    glTranslatef(-0.12f, 0.0f, 0.0f); 
                    glScalef(0.12f, 0.05f, 0.01f);
                    glutSolidCube(1.0f);
                glPopMatrix();
            glPopMatrix();
            
            // parte de baixo
            glPushMatrix();
                glColor3f(0.8f, 0.6f, 0.1f);
                glTranslatef(0.0f, -0.1f, 0.2f);
                glScalef(0.5f, 0.4f, 0.2f);
                glutSolidCube(0.5f);

                glPushMatrix();
                    glColor3f(0.1f, 0.1f, 0.1f);
                    glTranslatef(0.0f, 0.0f, 0.2f);
                    glScalef(0.8f, 0.15f, 0.8f);
                    glutSolidCube(0.50);
                glPopMatrix();


            glPopMatrix();

        glPopMatrix();

    glPopMatrix();

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
}

int main(int argc, char* argv[]) {

    // inicia o glut e cria a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Robo Humanoide");

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

    // entra no loop principal
    glutMainLoop();

    return 0;
}

