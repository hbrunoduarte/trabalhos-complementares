#include <GL/glut.h>

#define COLOR(r,g,b) r/255.0,g/255.0,b/255.0

static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;
static GLfloat yCam = -1.0f;

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
    gluPerspective(50.0f, fAspect, 1.0, 40.0);

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

// Função para desenhar os eixos XYZ
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

// Função para desenhar prisma retangular

void desenharPrisma(float xCentro, float yCentro, float zCentro, float largura, float altura, float profundidade) {
    // Dividimos por 2 para desenhar a partir do centro (origem)
    float x = largura / 2.0f;
    float y = altura / 2.0f;
    float z = profundidade / 2.0f;

    glBegin(GL_QUADS);

        // 1. FACE FRONTAL (z positivo)
        glVertex3f(-x+xCentro, -y+yCentro,  z+zCentro); // Inferior esquerdo
        glVertex3f( x+xCentro, -y+yCentro,  z+zCentro); // Inferior direito
        glVertex3f( x+xCentro,  y+yCentro,  z+zCentro); // Superior direito
        glVertex3f(-x+xCentro,  y+yCentro,  z+zCentro); // Superior esquerdo

        // 2. FACE TRASEIRA (z negativo) - Note que a ordem inverte ao olhar de trás
        glVertex3f(-x+xCentro, -y+yCentro, -z+zCentro); // Inferior esquerdo
        glVertex3f(-x+xCentro,  y+yCentro, -z+zCentro); // Superior esquerdo
        glVertex3f( x+xCentro,  y+yCentro, -z+zCentro); // Superior direito
        glVertex3f( x+xCentro, -y+yCentro, -z+zCentro); // Inferior direito

        // 3. FACE SUPERIOR (y positivo)
        glVertex3f(-x+xCentro,  y+yCentro, -z+zCentro); // Canto superior esquerdo (fundo)
        glVertex3f(-x+xCentro,  y+yCentro,  z+zCentro); // Canto inferior esquerdo (frente)
        glVertex3f( x+xCentro,  y+yCentro,  z+zCentro); // Canto inferior direito (frente)
        glVertex3f( x+xCentro,  y+yCentro, -z+zCentro); // Canto superior direito (fundo)

        // 4. FACE INFERIOR (y negativo)
        glVertex3f(-x+xCentro, -y+yCentro, -z+zCentro); 
        glVertex3f( x+xCentro, -y+yCentro, -z+zCentro); 
        glVertex3f( x+xCentro, -y+yCentro,  z+zCentro); 
        glVertex3f(-x+xCentro, -y+yCentro,  z+zCentro); 

        // 5. FACE DIREITA (x positivo)
        glVertex3f( x+xCentro, -y+yCentro, -z+zCentro);
        glVertex3f( x+xCentro,  y+yCentro, -z+zCentro);
        glVertex3f( x+xCentro,  y+yCentro,  z+zCentro);
        glVertex3f( x+xCentro, -y+yCentro,  z+zCentro);

        // 6. FACE ESQUERDA (x negativo)
        glVertex3f(-x+xCentro, -y+yCentro, -z+zCentro);
        glVertex3f(-x+xCentro, -y+yCentro,  z+zCentro);
        glVertex3f(-x+xCentro,  y+yCentro,  z+zCentro);
        glVertex3f(-x+xCentro,  y+yCentro, -z+zCentro);

    glEnd();
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

	desenhaEixos();

	////////////////////////////////////////////

	// Chão

	glColor3f(COLOR(0x68, 0x6c, 0x54));
	glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, -5.0f, 0.0f);
		gluDisk(pObj, 0.0f, 10.0f, 20, 10);
	glPopMatrix();

	// Torre 1

	glColor3f(COLOR(0xd9, 0xbc, 0xa3));
	glPushMatrix();
		glTranslatef(-2.5f, 0.0f, -2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.5f, 0.5f, 1.0f, 100, 13);
	glPopMatrix();

    // Torre 2

    glColor3f(COLOR(0xd9, 0xbc, 0xa3));
	glPushMatrix();
		glTranslatef(-2.5f, 0.0f, 2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.5f, 0.5f, 1.0f, 100, 13);
	glPopMatrix();

    // Torre 3

    glColor3f(COLOR(0xd9, 0xbc, 0xa3));
	glPushMatrix();
		glTranslatef(2.5f, 0.0f, 2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.5f, 0.5f, 1.0f, 100, 13);
	glPopMatrix();

    // Torre 4

    glColor3f(COLOR(0xd9, 0xbc, 0xa3));
	glPushMatrix();
		glTranslatef(2.5f, 0.0f, -2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.5f, 0.5f, 1.0f, 100, 13);
	glPopMatrix();

    // Torre Central

    glColor3f(COLOR(0xa6, 0xa6, 0xa6));
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 1.5f, 1.5f, 2.5f, 26, 13);
    glPopMatrix();

    // Paredes

    // Parede 1-4

    glColor3f(COLOR(0x7d, 0x3c, 0x07));
    glPushMatrix();
        glTranslatef(0.0f, -0.2f, 0.0f);
        desenharPrisma(0.0f, 0.4f, 2.5f, 5.0f, 0.6f, 0.2f);
    glPopMatrix();

    // Parede 2-3

    glColor3f(COLOR(0x7d, 0x3c, 0x07));
    glPushMatrix();
        glTranslatef(0.0f, -0.2f, 0.0f);
        desenharPrisma(0.0f, 0.4f, -2.5f, 5.0f, 0.6f, 0.2f);
    glPopMatrix();

    // Parede 1-2

    glColor3f(COLOR(0x7d, 0x3c, 0x07));
    glPushMatrix();
        glTranslatef(0.0f, -0.2f, 0.0f);
        desenharPrisma(-2.5f, 0.4f, 0.0f, 0.2f, 0.6f, 5.0f);
    glPopMatrix();


    // Parede 3-4

    glColor3f(COLOR(0x7d, 0x3c, 0x07));
    glPushMatrix();
        glTranslatef(0.0f, -0.2f, 0.0f);
        desenharPrisma(2.5f, 0.4f, 0.0f, 0.2f, 0.6f, 5.0f);
    glPopMatrix();

    // Telhados das Torres

    // Telhado 1

    glColor3f(COLOR(0x44, 0x2a, 0x25));
    glPushMatrix();
        glTranslatef(-2.5f, 1.0f, -2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.7f, 0.0f, 0.5f, 26, 13);
        glPushMatrix();
            gluDisk(pObj, 0.5f, 0.7f, 100, 100);
        glPopMatrix();
    glPopMatrix();

    // Telhado 2

    glColor3f(COLOR(0x44, 0x2a, 0x25));
    glPushMatrix();
        glTranslatef(-2.5f, 1.0f, 2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.7f, 0.0f, 0.5f, 26, 13);
        glPushMatrix();
            gluDisk(pObj, 0.5f, 0.7f, 100, 100);
        glPopMatrix();
    glPopMatrix();

    // Telhado 3

    glColor3f(COLOR(0x44, 0x2a, 0x25));
    glPushMatrix();
        glTranslatef(2.5f, 1.0f, 2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.7f, 0.0f, 0.5f, 26, 13);
        glPushMatrix();
            gluDisk(pObj, 0.5f, 0.7f, 100, 100);
        glPopMatrix();
    glPopMatrix();

    // Telhado 4

    glColor3f(COLOR(0x44, 0x2a, 0x25));
    glPushMatrix();
        glTranslatef(2.5f, 1.0f, -2.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 0.7f, 0.0f, 0.5f, 26, 13);

        glPushMatrix();
            gluDisk(pObj, 0.5f, 0.7f, 100, 100);
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
    glClearColor(0x77/255.0, 0xb9/255.0, 0xf2/255.0, 1.0f);  
}

int main(int argc, char* argv[]) {

    // inicia o glut e cria a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Castelo");

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
