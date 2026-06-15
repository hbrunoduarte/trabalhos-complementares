uniform sampler2D shadowMap;

float calcularSombra(vec4 posLuz, vec3 normal, vec3 lightDir) {
    vec3 projCoords = posLuz.xyz / posLuz.w;
    
    // Transforma a coordenada para o padrão do OpenGL (0.0 a 1.0)
    projCoords = projCoords * 0.5 + 0.5;

    // 1. CORREÇÃO DE VAZAMENTO: Se estiver fora do holofote, ignora a sombra
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // 2. CORREÇÃO DE VAZAMENTO NOTURNO: Se for de noite neste pixel, o eclipse é invisível
    if(dot(normal, lightDir) < 0.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    // 3. PCF (Percentage Closer Filtering) - Resolve o Pixelado criando a Penumbra!
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); // O tamanho de 1 pixel da textura
    
    // Lê um quadrado de 5x5 pixéis em torno da sombra e faz a média
    for(int x = -2; x <= 2; ++x) {
        for(int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    
    // Divide por 25 (o total de amostras colhidas) para obter a suavidade
    shadow /= 25.0; 

    return shadow;
}