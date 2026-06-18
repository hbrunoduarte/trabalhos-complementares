out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// A variável de textura (tem que bater com o C)
uniform sampler2D texNuvens; 
uniform vec3 lightPosView;

// A MÁGICA DO MOVIMENTO: O tempo que vem do C
uniform float tempo; 

void main() {
    // Pegamos a coordenada original e somamos um pouquinho no eixo X baseado no tempo
    // O valor 0.02 é a velocidade. Se quiser mais rápido, aumente!
    vec2 uvAnimada = vec2(TexCoord.x + (tempo * 0.02), TexCoord.y);

    // Lemos a textura usando a coordenada nova que está se movendo
    vec4 corDaTextura = texture(texNuvens, uvAnimada);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosView - FragPos);

    // Luz difusa
    float diff = max(dot(norm, lightDir), 0.0);
    float ambient = 0.02;

    vec3 corFinal = (ambient + diff) * corDaTextura.rgb;

    // Transparência
    float alpha = dot(corDaTextura.rgb, vec3(0.299, 0.587, 0.114));

    FragColor = vec4(corFinal, alpha * (diff + 0.1));
}