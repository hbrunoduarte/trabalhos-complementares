out vec4 FragColor;

in vec2 TexCoord; // Recebe o UV do Vertex Shader genérico

uniform sampler2D texCeu;

void main() {
    // Apenas a cor pura da imagem JPG, sem cálculos de luz!
    FragColor = texture(texCeu, TexCoord);
}