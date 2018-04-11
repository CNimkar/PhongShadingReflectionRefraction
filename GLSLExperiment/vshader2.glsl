#version 430

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
in  vec4 vPosition;
out vec2 TextCoord;

void main() {
gl_Position = projection_matrix*model_matrix*vPosition;
TextCoord = vPosition.xy/3;
}