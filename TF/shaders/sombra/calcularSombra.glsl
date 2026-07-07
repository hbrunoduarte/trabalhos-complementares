uniform sampler2D shadowMap;

float calcularSombra(vec4 posLuz, vec3 normal, vec3 lightDir, bool ehTranslucido) {
    vec3 projCoords = posLuz.xyz / posLuz.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float dotNormalLight = dot(normal, lightDir);

    if(dotNormalLight < 0.0 && !ehTranslucido) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    
    float bias = max(0.005 * (1.0 - abs(dotNormalLight)), 0.001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); 
    
    for(int x = -2; x <= 2; ++x) {
        for(int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 25.0; 

    return shadow;
}