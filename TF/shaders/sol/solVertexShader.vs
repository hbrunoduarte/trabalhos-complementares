#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord; // Lemos a UV do VBO

out vec2 TexCoord;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
   TexCoord = aTexCoord;
   gl_Position = projection * modelView * vec4(aPos, 1.0);
}