out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform sampler2D texAnel;
uniform vec3 lightPosView;
uniform int recebeSombras;
uniform float alphaMultiplicador;

void main() {
    vec4 corDaTextura = texture(texAnel, TexCoord);
    
    if (corDaTextura.a < 0.1) discard; 

    // Garante que o disco seja iluminado corretamente mesmo visto por baixo
    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) {
        norm = -norm; 
    }

    vec3 lightDir = normalize(lightPosView - FragPos);
    
    // Usamos abs() em vez de max() porque anéis são feitos de pó e gelo.
    // Isso simula a luz translúcida passando pelo anel e iluminando o lado de trás.
    float NdotL = abs(dot(norm, lightDir)); 

    float valorSombra = 0.0;
    if (recebeSombras == 1) {
        valorSombra = calcularSombra(FragPosLightSpace, norm, lightDir, true);
    }

    float ambient = 0.3; // Luz ambiente levemente maior para anéis
    float luzDireta = NdotL * (1.0 - valorSombra);
    
    vec3 corIluminada = (ambient + luzDireta) * corDaTextura.rgb;

    FragColor = vec4(corIluminada, corDaTextura.a * alphaMultiplicador);
}