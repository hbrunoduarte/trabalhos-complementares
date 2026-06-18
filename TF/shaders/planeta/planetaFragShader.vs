out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texPlaneta;
uniform vec3 lightPosView;

void main() {
    vec4 corDaTextura = texture(texPlaneta, TexCoord);
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosView - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    float ambient = 0.02; 
    
    vec3 corFinal = (ambient + diff) * corDaTextura.rgb;
    
    FragColor = vec4(corFinal, corDaTextura.a);
}