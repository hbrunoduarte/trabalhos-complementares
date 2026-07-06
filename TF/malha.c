#include "libs/config.h"

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

    array[(*index)++] = 1.0f - theta / (2.0f * M_PI); // U
    array[(*index)++] = 1.0f - phi / M_PI;            // V

    array[(*index)++] = -sin(theta);
    array[(*index)++] = 0.0f;
    array[(*index)++] = cos(theta);
}

EsferaMesh* criarEsferaArray(float r, unsigned int pTheta, unsigned int pPhi) {
    EsferaMesh *malha = malloc(sizeof(EsferaMesh));
    
    // Cada "quadrado" na malha da esfera precisa de 2 triângulos
    // 2 triângulos * 3 vértices cada = 6 vértices por quadrado
    malha->numVertices = pPhi * pTheta * 6;
    
    // Cada vértice ocupa 11 floats na memória (3 de posição + 3 de normal + 2 de textura + 3 de normal mapping)
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

DiscoMesh* criarDiscoArray(float raioInterno, float raioExterno, int segmentos) {
    int numVertices = segmentos * 6; 
    // Uso de 8 floats por vértice (3 Posição, 3 Normal, 2 UV)
    float *dados = malloc(numVertices * DISK_INFO * sizeof(float)); 
    
    float anguloPasso = (2.0f * M_PI) / segmentos;
    int index = 0;
    
    for (int i = 0; i < segmentos; i++) {
        float ang = i * anguloPasso;
        float proxAng = (i + 1) * anguloPasso;
        
        float x1_in = raioInterno * cos(ang), z1_in = raioInterno * sin(ang);
        float x1_out = raioExterno * cos(ang), z1_out = raioExterno * sin(ang);
        float x2_in = raioInterno * cos(proxAng), z2_in = raioInterno * sin(proxAng);
        float x2_out = raioExterno * cos(proxAng), z2_out = raioExterno * sin(proxAng);
        
        float u_in = 0.0f;
        float u_out = 1.0f;
        float v1 = (float)i / segmentos;
        float v2 = (float)(i + 1) / segmentos;

        float nx = 0.0f, ny = 1.0f, nz = 0.0f; 

        // Triângulo 1 (8 dados por vértice)
        dados[index++] = x1_in;  dados[index++] = 0.0f; dados[index++] = z1_in;  dados[index++] = nx; dados[index++] = ny; dados[index++] = nz; dados[index++] = u_in;  dados[index++] = v1;
        dados[index++] = x1_out; dados[index++] = 0.0f; dados[index++] = z1_out; dados[index++] = nx; dados[index++] = ny; dados[index++] = nz; dados[index++] = u_out; dados[index++] = v1;
        dados[index++] = x2_in;  dados[index++] = 0.0f; dados[index++] = z2_in;  dados[index++] = nx; dados[index++] = ny; dados[index++] = nz; dados[index++] = u_in;  dados[index++] = v2;
        
        // Triângulo 2 (8 dados por vértice)
        dados[index++] = x1_out; dados[index++] = 0.0f; dados[index++] = z1_out; dados[index++] = nx; dados[index++] = ny; dados[index++] = nz; dados[index++] = u_out; dados[index++] = v1;
        dados[index++] = x2_out; dados[index++] = 0.0f; dados[index++] = z2_out; dados[index++] = nx; dados[index++] = ny; dados[index++] = nz; dados[index++] = u_out; dados[index++] = v2;
        dados[index++] = x2_in;  dados[index++] = 0.0f; dados[index++] = z2_in;  dados[index++] = nx; dados[index++] = ny; dados[index++] = nz; dados[index++] = u_in;  dados[index++] = v2;
    }
    
    DiscoMesh *mesh = malloc(sizeof(DiscoMesh));
    mesh->dados = dados;
    mesh->numVertices = numVertices;
    return mesh;
}