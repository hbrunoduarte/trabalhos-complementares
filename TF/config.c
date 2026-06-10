#include "libs/config.h"
#include "libs/stb_image.h"

char* lerArquivo(const char *caminho) {

    FILE *file = fopen(caminho, "rb");
    if (!file) {
        fprintf(stderr, "Erro ao abrir arquivo %s\n", caminho);
        exit(-1);
    }

    fseek(file, 0, SEEK_END);
    long tamArquivo = ftell(file);
    rewind(file);

    char *conteudo = malloc(tamArquivo + 1);
    if (!conteudo) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        fclose(file);
        return NULL;
    }

    size_t elementosLidos = fread(conteudo, sizeof(char), tamArquivo, file);
    conteudo[elementosLidos] = '\0';

    fclose(file);
    return conteudo;
}

GLuint carregarShader(const char *vertexShaderSource, const char *fragShaderSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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