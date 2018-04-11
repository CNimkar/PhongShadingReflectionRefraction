#version 430

out vec4  fColor;
in vec2 TextCoord;
uniform sampler2D textResult;
uniform bool textureToggle;

void main() {
fColor = texture(textResult, TextCoord);
}