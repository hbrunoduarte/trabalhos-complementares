in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;
in vec4 FragPosLightSpace; // Recebido do Vertex

out vec4 FragColor;

uniform sampler2D texDia;
uniform sampler2D texNoite;
uniform sampler2D texNuvens;
uniform sampler2D texNormal; 
uniform vec3 lightPosView;

void main() {
   // 1. LÊ O RELEVO E DISTORCE A LUZ
   vec3 normalTextura = texture(texNormal, TexCoord).rgb;
   normalTextura = normalTextura * 2.0 - 1.0; 
   vec3 norm = normalize(TBN * normalTextura); 
   
   vec3 lightDir = normalize(lightPosView - FragPos);
   
   float luzDaRotacao = dot(norm, lightDir);
   float valorSombra = calcularSombra(FragPosLightSpace, norm, lightDir);
   
   float impactoLuz = luzDaRotacao > 0.0 ? luzDaRotacao * (1.0 - valorSombra) : luzDaRotacao;
   
   // O smoothstep cria a penumbra do crepúsculo
   float mixDiaNoite = smoothstep(-0.1, 0.2, impactoLuz);
   
   // 2. LÊ AS OUTRAS IMAGENS
   vec4 corDia = texture(texDia, TexCoord);
   vec4 corNoite = texture(texNoite, TexCoord);
   vec4 corNuvens = texture(texNuvens, TexCoord);
   
   // 3. MISTURA TUDO
   vec3 diaComNuvens = mix(corDia.rgb, vec3(1.0), corNuvens.r);
   vec3 noiteComNuvens = mix(corNoite.rgb, vec3(0.0), corNuvens.r);
   vec3 corFinal = mix(noiteComNuvens, diaComNuvens, mixDiaNoite);
   
   FragColor = vec4(corFinal, 1.0);
}