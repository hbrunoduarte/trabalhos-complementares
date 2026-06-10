#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent; // Recebe a Tangente do C

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN; // Matriz de Rotação do Relevo

uniform mat4 modelView;
uniform mat4 projection;

void main() {
   FragPos = vec3(modelView * vec4(aPos, 1.0));
   TexCoord = aTexCoord;
   
   // Transforma a Normal e a Tangente para o ponto de vista da câmera
   vec3 T = normalize(mat3(modelView) * aTangent);
   vec3 N = normalize(mat3(modelView) * aNormal);
   
   // Garante que elas estão perfeitamente a 90 graus uma da outra (Gram-Schmidt)
   T = normalize(T - dot(T, N) * N);
   
   // Calcula o terceiro eixo (Bitangente) e monta a matriz
   vec3 B = cross(N, T);
   TBN = mat3(T, B, N);
   
   gl_Position = projection * modelView * vec4(aPos, 1.0);
}