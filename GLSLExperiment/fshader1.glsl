#version 430

out vec4  fColor;
in vec4 color;
in vec3 pos;
uniform samplerCube skybox;
in vec3 fNormal;
uniform float reflectionToggle;
uniform float shadowToggle;
in vec3 R;

void main() {
if (reflectionToggle == 0.0f)
	fColor = color;
else
	fColor = vec4(texture(skybox, R).rgb, 1.0);

if(shadowToggle == 1.0f)
	fColor = vec4(0.3,0.3,0.3,1.0);
}
