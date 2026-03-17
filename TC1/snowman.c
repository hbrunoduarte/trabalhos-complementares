// gcc cc snowman.c -lglut -lGL -lGLU -lm -o snowman.out && ./snowman
// para compilar e executar

#include <GL/glut.h>

static GLfloat yRot = 0.0f;
static GLfixed xRot = 0.0f;

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
    // pois os valores xRot e yRot mudaram
    glutPostRedisplay();  
}

void RenderScene(void) {
    //
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
    glutCreateWindow("Boneco de Neve");

    // configurações da câmera
    glutReshapeFunc(ChangeSize);  
    // configurações das teclas
    glutSpecialFunc(SpecialKeys);
    //  ...
    //RenderScene

    // função chamada pra inicialização de luzes
    // e renderização
    init();

    // entra no loop principal
    glutMainLoop();

    return 0;
}

