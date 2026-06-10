#version 330 core
in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;
out vec4 FragColor;

uniform sampler2D texDia;
uniform sampler2D texNoite;
uniform sampler2D texNuvens;
uniform sampler2D texNormal; // Textura roxa/azul das montanhas
uniform vec3 lightPosView;

void main() {
   // 1. LÊ O RELEVO E DISTORCE A LUZ
   vec3 normalTextura = texture(texNormal, TexCoord).rgb;
   normalTextura = normalTextura * 2.0 - 1.0; // Converte de cor (0 a 1) para vetor (-1 a 1)
   vec3 norm = normalize(TBN * normalTextura); // Montanha distorce a normal da esfera!
   
   vec3 lightDir = normalize(lightPosView - FragPos);
   float impactoLuz = dot(norm, lightDir); // Pode ser negativo (Noite)
   
   // O smoothstep cria a penumbra do crepúsculo
   float mixDiaNoite = smoothstep(-0.1, 0.2, impactoLuz);
   
   // 2. LÊ AS OUTRAS IMAGENS
   vec4 corDia = texture(texDia, TexCoord);
   vec4 corNoite = texture(texNoite, TexCoord);
   vec4 corNuvens = texture(texNuvens, TexCoord);
   
   // 3. MISTURA TUDO
   // Nuvens fazem sombra sobre o dia
   vec3 diaComNuvens = mix(corDia.rgb, vec3(1.0), corNuvens.r);
   
   // Nuvens escondem as luzes das cidades
   vec3 noiteComNuvens = mix(corNoite.rgb, vec3(0.0), corNuvens.r);
   
   // Escolhe entre o Dia ou a Noite baseado na luz e no relevo
   vec3 corFinal = mix(noiteComNuvens, diaComNuvens, mixDiaNoite);
   
   FragColor = vec4(corFinal, 1.0);
}