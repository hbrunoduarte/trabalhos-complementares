out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texNuvens; 
uniform vec3 lightPosView;

uniform float tempo; 

void main() {
    vec2 uvAnimada = vec2(TexCoord.x + (tempo * 0.02), TexCoord.y);

    vec4 corDaTextura = texture(texNuvens, uvAnimada);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosView - FragPos);

    // Luz difusa
    float diff = max(dot(norm, lightDir), 0.0);
    float ambient = 0.02;

    vec3 corFinal = (ambient + diff) * corDaTextura.rgb;

    // Transparência
    float alpha = dot(corDaTextura.rgb, vec3(0.4, 0.587, 0.114));

    FragColor = vec4(corFinal, alpha * (diff + 0.1));
}