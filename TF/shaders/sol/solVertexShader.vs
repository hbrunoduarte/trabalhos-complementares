layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;    // NOVO: Lê a normal do VBO
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos; // NOVO: Envia a posição para o Fragment Shader
out vec3 Normal;  // NOVO: Envia a normal para o Fragment Shader
out vec2 TexCoord;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
   // Calcula a posição do vértice no espaço da câmera
   FragPos = vec3(modelView * vec4(aPos, 1.0));
   
   // Calcula a normal no espaço da câmera (ignorando escalas irregulares)
   Normal = mat3(transpose(inverse(modelView))) * aNormal;
   
   TexCoord = aTexCoord;
   
   gl_Position = projection * modelView * vec4(aPos, 1.0);
}