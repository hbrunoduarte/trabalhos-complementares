in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;
out vec4 FragColor;

uniform sampler2D texNuvens;
uniform vec3 lightPosView;
uniform float tempo; // O relógio do jogo

void main() {
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPosView - FragPos);

   float luzDaRotacao = dot(norm, lightDir);
   float valorSombra = calcularSombra(FragPosLightSpace, norm, lightDir);
   
   float impactoLuz = luzDaRotacao > 0.0 ? luzDaRotacao * (1.0 - valorSombra) : luzDaRotacao;  
   // Desliza a textura suavemente para o lado
   vec2 uvAnimada = TexCoord + vec2(tempo * 0.02, 0.0);
   
   // Lê a textura usando o UV animado!
   vec4 corNuvem = texture(texNuvens, uvAnimada);
   
   vec3 corFinal = vec3(1.0) * impactoLuz;
   float alpha = corNuvem.r;
   
   FragColor = vec4(corFinal, alpha);
}