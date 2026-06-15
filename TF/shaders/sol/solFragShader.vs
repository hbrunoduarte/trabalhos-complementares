in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texSol;
uniform float tempo; // Recebe o relógio do jogo

void main() {
   // Camada 1: Desliza a textura rapidamente para a direita
   vec2 uv1 = TexCoord + vec2(tempo * 0.005, 0.0);
   
   // Camada 2: Desliza na diagonal contrária, um pouco mais devagar
   vec2 uv2 = TexCoord + vec2(-tempo * 0.0003, tempo * 0.002);
   
   // Lê os pixels da imagem nas duas posições distorcidas
   vec3 cor1 = texture(texSol, uv1).rgb;
   vec3 cor2 = texture(texSol, uv2).rgb;
   
   // MÁGICA: Mistura as duas texturas e multiplica por 1.5 para estourar o brilho!
   // Como a tela de LCD não mostra cores acima de 1.0, o amarelo vira quase branco 
   // no centro, parecendo incandescente.
   vec3 corFinal = (cor1 + cor2) * 0.8 * 1.5;
   
   FragColor = vec4(corFinal, 1.0);
}