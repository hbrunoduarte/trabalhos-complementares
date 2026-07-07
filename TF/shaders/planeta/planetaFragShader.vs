out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform sampler2D texPlaneta;

uniform float opacidadeAnel;
uniform int usarAnel;

uniform vec3 lightPosView;
uniform float alphaMultiplicador;
uniform int isGasoso;
uniform int recebeSombras;
uniform float tempo;

void main() {
    vec4 corDaTextura = texture(texPlaneta, TexCoord);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosView - FragPos);
    vec3 viewDir = normalize(-FragPos);

    float NdotL = dot(norm, lightDir);
    
    float valorSombra = 0.0;
    if (recebeSombras == 1) {
        valorSombra = calcularSombra(FragPosLightSpace, norm, lightDir, false);
        valorSombra *= (usarAnel == 1) ? opacidadeAnel : 1.0;
    }
    
    float luzDireta = smoothstep(0.02, 0.2, NdotL) * (1.0 - valorSombra);

    float ambient = 0.02; 
    vec3 corIluminada = (ambient + luzDireta) * corDaTextura.rgb;
    vec3 corFinal = corIluminada;
    
    if (isGasoso == 1) {
        float visaoNormal = max(dot(viewDir, norm), 0.0);
        float fresnel = 1.0 - visaoNormal;
        fresnel = pow(fresnel, 5.0); 
        
        float iluminacaoFrontal = max(NdotL, 0.0) * (1.0 - valorSombra);
        float mascaraLuz = smoothstep(0.0, 0.2, iluminacaoFrontal);
        
        vec3 brilhoAtmosfera = corDaTextura.rgb * fresnel * 1.5 * mascaraLuz;
        
        corFinal += brilhoAtmosfera; 
    }
    
    FragColor = vec4(corFinal, corDaTextura.a * alphaMultiplicador);
}