layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN; // Matriz de Rotação do Relevo
out vec4 FragPosLightSpace;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
   FragPos = vec3(modelView * vec4(aPos, 1.0));
   TexCoord = aTexCoord;
   
   vec3 T = normalize(mat3(modelView) * aTangent);
   vec3 N = normalize(mat3(modelView) * aNormal);
   
   T = normalize(T - dot(T, N) * N);
   
   // Calcula o terceiro eixo (Bitangente) e monta a matriz
   vec3 B = cross(N, T);
   TBN = mat3(T, B, N);

   FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
   
   gl_Position = projection * modelView * vec4(aPos, 1.0);
}