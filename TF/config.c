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

/*GLuint carregarShader(const char *vertexShaderSource, const char *fragShaderSource, const char *libSource) {

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexStrings[2] = { "#version 330 core\n", vertexShaderSource };
    glShaderSource(vertexShader, 2, vertexStrings, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragStrings[3];
    fragStrings[0] = "#version 330 core\n";
    fragStrings[1] = libSource ? libSource : ""; // Se não houver biblioteca, envia vazio
    fragStrings[2] = fragShaderSource;
    
    glShaderSource(fragmentShader, 3, fragStrings, NULL);
    glCompileShader(fragmentShader);

    // --- LINKAGEM ---
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}*/

GLuint carregarShader(const char *vertexShaderSource, const char *fragShaderSource, const char *libSource) {
    
    int success;
    char infoLog[512];

    // --- VERTEX SHADER ---
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexStrings[2] = { "#version 330 core\n", vertexShaderSource };
    glShaderSource(vertexShader, 2, vertexStrings, NULL);
    glCompileShader(vertexShader);
    
    // VERIFICA ERROS NO VERTEX
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERRO FATAL NO VERTEX SHADER:\n%s\n", infoLog);
    }

    // --- FRAGMENT SHADER ---
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragStrings[3];
    fragStrings[0] = "#version 330 core\n";
    fragStrings[1] = libSource ? libSource : ""; 
    fragStrings[2] = fragShaderSource;
    
    glShaderSource(fragmentShader, 3, fragStrings, NULL);
    glCompileShader(fragmentShader);

    // VERIFICA ERROS NO FRAGMENT
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERRO FATAL NO FRAGMENT SHADER:\n%s\n", infoLog);
    }

    // --- LINKAGEM DO PROGRAMA ---
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // VERIFICA ERROS NA LINKAGEM
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERRO FATAL NA LINKAGEM DO SHADER:\n%s\n", infoLog);
    }

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