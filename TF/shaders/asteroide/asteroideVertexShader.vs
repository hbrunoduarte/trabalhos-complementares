layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 instanceMatrix;

out vec2 TexCoords;
out vec3 WorldPos;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix; 
uniform sampler2D displacementMap;
uniform float dispScale;
uniform float tempo;
uniform float velAngular;

void main() {
   TexCoords = aTexCoords;

   // Lendo o displacement e alterando a posição local do vértice
   float height = texture(displacementMap, TexCoords).r;
   vec3 displacedPos = aPos + (aNormal * height * dispScale);
   
   // ========================================================
   // 1. CÁLCULO DA ÓRBITA (ROTAÇÃO NO EIXO Y)
   // ========================================================
   float angulo = tempo * velAngular;
   float c = cos(angulo);
   float s = sin(angulo);

   // Constrói a matriz de rotação em Y. 
   // Nota: O GLSL lê matrizes no formato coluna-maior (column-major)
   mat4 rotacaoOrbita = mat4(
       c,   0.0, -s,  0.0,  // Coluna 0
       0.0, 1.0,  0.0, 0.0,  // Coluna 1
       s,   0.0,  c,  0.0,  // Coluna 2
       0.0, 0.0,  0.0, 1.0   // Coluna 3
   );

   // Multiplica a matriz estática da instância pela nova rotação
   mat4 matrizAnimada = rotacaoOrbita * instanceMatrix;
   // ========================================================

   // Aplica a matriz animada para definir a Posição no Mundo
   WorldPos = vec3(matrizAnimada * vec4(displacedPos, 1.0));

   FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);

   // Aplica a mesma matriz animada nos cálculos de Normal e Tangente
   // Isso garante que a luz bata corretamente enquanto as pedras giram
   vec3 T = normalize(vec3(matrizAnimada * vec4(aTangent, 0.0)));
   vec3 N = normalize(vec3(matrizAnimada * vec4(aNormal, 0.0)));
   T = normalize(T - dot(T, N) * N); 
   vec3 B = cross(N, T);
   TBN = mat3(T, B, N);
   
   gl_Position = projection * view * vec4(WorldPos, 1.0);
}