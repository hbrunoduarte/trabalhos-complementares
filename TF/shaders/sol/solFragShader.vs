in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texSol;
uniform float tempo; // Recebe o relógio do jogo

void main() {
   vec2 uv1 = TexCoord + vec2(tempo * 0.005, 0.0);
   vec2 uv2 = TexCoord + vec2(-tempo * 0.0003, tempo * 0.002);

   vec3 cor1 = texture(texSol, uv1).rgb;
   vec3 cor2 = texture(texSol, uv2).rgb;

   vec3 corBase = (cor1 + cor2) * 0.8 * 1.5;

   vec3 norm = normalize(Normal);
   vec3 viewDir = normalize(-FragPos); 

   float visaoNormal = max(dot(viewDir, norm), 0.0);

   float fatorEscurecimento = mix(0.3, 1.0, visaoNormal);

   fatorEscurecimento = pow(fatorEscurecimento, 0.8); 

   vec3 corFinal = corBase * fatorEscurecimento;

   FragColor = vec4(corFinal, 1.0);
}