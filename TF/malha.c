#include "malha.h"

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
    
    // Cada vértice ocupa 6 floats na memória (3 de posição + 3 de normal + 2 de textura + 3 de normal mapping)
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
    
    return malha;
}