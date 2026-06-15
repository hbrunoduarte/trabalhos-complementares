// Recebe a posição do vértice da malha da esfera
layout (location = 0) in vec3 aPos;

// Recebe as matrizes do seu código C (cglm)
uniform mat4 model;             // Coloca o astro na sua posição e rotação reais no espaço
uniform mat4 lightSpaceMatrix;  // A lente do "holofote" do Sol apontado para este sistema

void main() {
    // Multiplica de trás para a frente: Posição Local -> Posição no Mundo -> Posição na Visão do Sol
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}