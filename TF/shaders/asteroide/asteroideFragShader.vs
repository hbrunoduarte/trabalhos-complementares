out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform sampler2D colorMap;
uniform sampler2D normalGLMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {

   vec3 albedo = texture(colorMap, TexCoords).rgb;
   float roughness = texture(roughnessMap, TexCoords).r;
   float ao = texture(aoMap, TexCoords).r;

   // Extraindo normal do mapa e movendo de [0,1] para [-1,1]
   vec3 normal = texture(normalGLMap, TexCoords).rgb;
   normal = normalize(normal * 2.0 - 1.0);
   normal = normalize(TBN * normal); // Passando pro espaço do mundo

   // Vetores de direção
   vec3 lightDir = normalize(lightPos - WorldPos);
   vec3 viewDir = normalize(viewPos - WorldPos);
   vec3 halfwayDir = normalize(lightDir + viewDir);

   // Ambiente (afetada pela Oclusão Ambiente)
   vec3 ambient = 0.3 * albedo * ao;

   // Difusa
   float diff = max(dot(normal, lightDir), 0.0);
   vec3 diffuse = diff * albedo;

   // Especular (afetada pelo Roughness)
   float shininess = (1.0 - roughness) * 128.0; // Inverte roughness para obter brilho
   shininess = max(shininess, 0.001);

   float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
   vec3 specular = vec3(0.5) * spec; // Cor da luz especular (branca)

   float shadow = calcularSombra(FragPosLightSpace, normal, lightDir, false);

   // A sombra afeta apenas as luzes Direta (Difusa) e Especular
   vec3 finalLighting = ambient + (1.0 - shadow) * (diffuse + specular);

   FragColor = vec4(finalLighting, 1.0);
}