layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    // Posição base no mundo
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // 1. Posição no View Space (Câmera) - Combina com lightPosView!
    FragPos = vec3(view * worldPos);
    
    // 2. Normal no View Space
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    
    // 3. Repassa as coordenadas de textura
    TexCoord = aTexCoord;
    
    // 4. Sombras precisam do World Space (Igual ao planetaVertexShader)
    FragPosLightSpace = lightSpaceMatrix * worldPos;
    
    // 5. Posição final na tela
    gl_Position = projection * view * worldPos;
}