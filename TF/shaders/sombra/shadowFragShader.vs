void main() {
    // Como nós criamos o FBO anexando apenas um GL_DEPTH_ATTACHMENT
    // e usamos glDrawBuffer(GL_NONE) no seu main.c, o OpenGL sabe que 
    // não precisa calcular cores RGB.
    
    // Ele grava a distância (Z) do vértice até ao Sol automaticamente!
    // A linha abaixo acontece invisivelmente nos bastidores:
    // gl_FragDepth = gl_FragCoord.z;
}