out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texCeu;

void main() {
    FragColor = texture(texCeu, TexCoord);
}