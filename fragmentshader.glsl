#version 330 core

uniform sampler2D tex;

in vec2 st;

out vec4 color;

void main() {
     color = texture(tex, st);
}

