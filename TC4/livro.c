// gcc robot.c -lglut -lGL -lGLU -lm -o robot.out && ./robot.out
// para compilar e executar

#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>

#define FAST_OBJ_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "fast_obj.h"
#include "stb_image.h"

#define PI 3.14159f

static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;
static GLfloat yCam = -1.0f;
static GLfloat zCam = -5.0f;

fastObjMesh* livroMesh = NULL;
GLuint texturas[20];

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
    gluPerspective(35.0f, fAspect, 0.01, 40.0);

    // voltamos pro modo de mexer nos objetos ao
    // invés da câmera
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity();
}

void NormalKeys(unsigned char key, int x, int y) {
    // 27 é o código ascii para a tecla esc
    if (key == 27) exit(0);
    // controle da câmera original
    if(key == 'w' || key == 'W') yCam -= 0.1f;
    if(key == 's' || key == 'S') yCam += 0.1f;
    // controle do zoom
    if(key == '+' || key == '=') zCam += 0.5f; // zoom in
    if(key == '-' || key == '_') zCam -= 0.5f; // zoom out

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
  
    glPushMatrix();
    glTranslatef(0.0f, yCam, zCam);  
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);

    // DESENHANDO O LIVRO
    glColor3f(1.0f, 1.0f, 1.0f); 

    if (livroMesh != NULL) {
        // variável para rastrear onde estamos no array de índices
        unsigned int index_offset = 0;
        
        // percorre todas as faces do modelo
        for (unsigned int i = 0; i < livroMesh->face_count; ++i) {
            
            // Descobre qual o índice do material desta face específica
            unsigned int material_index = livroMesh->face_materials[i];

            // Prende a textura correta na placa de vídeo
            glBindTexture(GL_TEXTURE_2D, texturas[material_index]);    

            // quantidade de vértices nessa face (3)
            unsigned int num_vertices = livroMesh->face_vertices[i];
            
            glBegin(GL_TRIANGLES);
            
            // loop para os 3 vértices deste triângulo
            for (unsigned int j = 0; j < num_vertices; ++j) {
                // descobre o índice exato desse vértice
                fastObjIndex indice = livroMesh->indices[index_offset + j];
                
                // 1. aplica a normal (para a luz refletir corretamente)
                if (indice.n) {
                    float nx = livroMesh->normals[3 * indice.n + 0];
                    float ny = livroMesh->normals[3 * indice.n + 1];
                    float nz = livroMesh->normals[3 * indice.n + 2];
                    glNormal3f(nx, ny, nz);
                }
                
                // 2. Aplica a COORDENADA UV (A textura!)
                // Descomentaremos isso no próximo passo, quando a foto estiver carregada
                
                if (indice.t) {
                    float u = livroMesh->texcoords[2 * indice.t + 0];
                    float v = livroMesh->texcoords[2 * indice.t + 1];
                    // O OpenGL inverte o eixo V da imagem em relação ao Blender, então fazemos 1.0 - v
                    glTexCoord2f(u, v); 
                }
                

                // 3. aplica a posicao XYZ do vértice
                if (indice.p) {
                    float vx = livroMesh->positions[3 * indice.p + 0];
                    float vy = livroMesh->positions[3 * indice.p + 1];
                    float vz = livroMesh->positions[3 * indice.p + 2];
                    glVertex3f(vx, vy, vz);
                }
            }
            glEnd();
            
            // pula para os índices do próximo triângulo
            index_offset += num_vertices; 
        }
    }

    glPopMatrix();  
    glutSwapBuffers();
}

void carregaTexturas() {
    // Avisa o OpenGL que vamos usar texturas 2D
    glEnable(GL_TEXTURE_2D);

    // Percorre todos os materiais que vieram do arquivo .mtl
    for (unsigned int i = 0; i < livroMesh->material_count; i++) {
        
        // Em fast_obj, map_Kd é um ÍNDICE (número) que aponta para a textura
        unsigned int tex_idx = livroMesh->materials[i].map_Kd;
        
        // Se for diferente de 0, significa que esse material tem uma foto!
        if (tex_idx != 0) {
            
            // Agora sim pegamos o texto (nome da foto) do lugar certo
            char* nome_foto = livroMesh->textures[tex_idx].name;
            
            // Como combinamos de colocar tudo na pasta assets, vamos juntar o caminho
            char caminho_completo[256];
            sprintf(caminho_completo, "%s", nome_foto);
            
            int width, height, channels;
            // O OpenGL inverte o eixo Y das imagens por padrão, isso corrige:
            stbi_set_flip_vertically_on_load(true); 
            
            // Lê os pixels da foto
            unsigned char *data = stbi_load(caminho_completo, &width, &height, &channels, 4);
            
            if (data) {
                // Pede pro OpenGL criar uma ID para essa textura
                glGenTextures(1, &texturas[i]);
                // "Prende" a textura para configurá-la
                glBindTexture(GL_TEXTURE_2D, texturas[i]);
                
                // Configura como a imagem deve esticar ou encolher
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                // Envia os pixels para a placa de vídeo
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                
                // Limpa a memória RAM (a imagem já está na GPU agora)
                stbi_image_free(data);
                printf("Carregou a textura: %s\n", caminho_completo);
            } else {
                printf("Erro ao carregar a textura: %s\n", caminho_completo);
            }
        }
    }
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
    // glEnable(GL_CULL_FACE);

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

    livroMesh = fast_obj_read("assets/livro_fundacao.obj");

    if(livroMesh == NULL) {
        printf("Erro ao carregar o arquivo .obj\n");
        return -1;
    } else {
        printf("Arquivo carregado com sucesso\n");
    }


    // inicia o glut e cria a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Livro Fundacao");

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
    // carrega as texturas do livro
    carregaTexturas();
    // entra no loop principal
    glutMainLoop();
    // libera a memória
    fast_obj_destroy(livroMesh);

    return 0;
}

