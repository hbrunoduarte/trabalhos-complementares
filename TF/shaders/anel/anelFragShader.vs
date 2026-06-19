out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texAnel;

void main() {
    vec4 corDaTextura = texture(texAnel, TexCoord);
    if (corDaTextura.a < 0.1) discard; 
    FragColor = vec4(corDaTextura.rgb * 1.5, corDaTextura.a);
}