in vec2 TexCoord;
in vec3 FragPos; // NOVO: Posição do fragmento
in vec3 Normal;  // NOVO: Direção da superfície

out vec4 FragColor;

uniform sampler2D texSol;
uniform float tempo; // Recebe o relógio do jogo

void main() {
   // 1. Camadas de textura originais
   vec2 uv1 = TexCoord + vec2(tempo * 0.005, 0.0);
   vec2 uv2 = TexCoord + vec2(-tempo * 0.0003, tempo * 0.002);

   vec3 cor1 = texture(texSol, uv1).rgb;
   vec3 cor2 = texture(texSol, uv2).rgb;

   // MÁGICA: Mistura as duas texturas e multiplica
   vec3 corBase = (cor1 + cor2) * 0.8 * 1.5;

   // CÁLCULO DO FRESNEL (Visão Normal)
   vec3 norm = normalize(Normal);
   vec3 viewDir = normalize(-FragPos); 

   // visaoNormal é 1.0 no centro exato do Sol e 0.0 na silhueta (borda)
   float visaoNormal = max(dot(viewDir, norm), 0.0);

   // O LIMB DARKENING REALISTA
   // Nós não queremos que a borda fique 100% preta (0.0), mas sim um pouco mais escura, tipo 30% (0.3).
   // O mix vai misturar 0.3 na borda e 1.0 no centro, baseado na curvatura.
   float fatorEscurecimento = mix(0.3, 1.0, visaoNormal);

   // Opcional: Aplicar uma curva suave para deixar a transição mais redonda
   fatorEscurecimento = pow(fatorEscurecimento, 0.8); 

   // Agora MULTIPLICAMOS a cor base por esse fator. 
   // Assim, o amarelo continua amarelo, mas fica mais escuro nas bordas.
   vec3 corFinal = corBase * fatorEscurecimento;

   FragColor = vec4(corFinal, 1.0);
}