#include "libs/config.h"

extern int width;
extern int height;

GLuint shadowShaderProgram = -1;

void carregarShaderSombras() {

    char *shadowVS = lerArquivo("shaders/sombra/shadowVertexShader.vs");
    char *shadowFS = lerArquivo("shaders/sombra/shadowFragShader.vs");
    shadowShaderProgram = carregarShader(shadowVS, shadowFS, NULL);
    free(shadowVS);
    free(shadowFS);
    
}

void setFramebufferSombras(CorpoCeleste *sistema, const int numCorpos) {

    for (int i = 0; i < numCorpos; i++) {
        glGenFramebuffers(1, &sistema[i].depthMapFBO);
        glGenTextures(1, &sistema[i].depthMap);
        
        glBindTexture(GL_TEXTURE_2D, sistema[i].depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, sistema[i].depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sistema[i].depthMap, 0);
        
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void calcularSombras(GLFWwindow *window, CorpoCeleste *sistema, const int numCorpos) {

    glUseProgram(shadowShaderProgram);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    for (int alvo = 0; alvo < numCorpos; alvo++) {
            
        glBindFramebuffer(GL_FRAMEBUFFER, sistema[alvo].depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        vector posVisAlvo = calcularPosicaoVisual(&sistema[alvo]);
        float raioAlvo = calcularRaioVisual(&sistema[alvo]);
        vec3 up = {0.0f, 1.0f, 0.0f};

        float margem = raioAlvo * 5.0f + 2.0f;
        
        mat4 lightProjection, lightView;
        glm_ortho(-margem, margem, -margem, margem, 1.0f, 300.0f, lightProjection); // projeção ortogonal
        glm_lookat(VETOR_NULO.raw, posVisAlvo.raw, up, lightView);
        
        glm_mat4_mul(lightProjection, lightView, sistema[alvo].lightSpaceMatrix);

        // 3. Desenha TODOS os corpos (exceto o Sol) nesta fotografia
        for (int obj = 0; obj < numCorpos; obj++) {
            mat4 modelShadow;
            glm_mat4_identity(modelShadow);
            vector pVis = calcularPosicaoVisual(&sistema[obj]);
            float rVis = calcularRaioVisual(&sistema[obj]);
            glm_translate(modelShadow, pVis.raw);
            glm_scale(modelShadow, (vec3){rVis, rVis, rVis});

            glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "model"), 1, GL_FALSE, (float*)modelShadow);
            glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, (float*)sistema[alvo].lightSpaceMatrix);

            glBindBuffer(GL_ARRAY_BUFFER, sistema[obj].VBO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SPHERE_INFO * sizeof(float), (void*)0);
            glDrawArrays(GL_TRIANGLES, 0, sistema[obj].totalVertices);
            glDisableVertexAttribArray(0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

}
