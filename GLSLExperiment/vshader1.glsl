#version 430

out vec4 color;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct; 
uniform vec4 LightPosition;
uniform float Shininess;
uniform mat4 projection_matrix;
uniform mat4 model_matrix;
in  vec4 vPosition;
in  vec4 vColor;
in	vec3 vNormal;
out vec3 fNormal;
out vec3 pos;
uniform float reflectionToggle;
out vec3 R;

void main() {
gl_Position = projection_matrix*model_matrix*vPosition;

pos = (model_matrix * vPosition).xyz;
vec3 L = normalize( LightPosition.xyz - pos );		// light Vector
vec3 E = normalize( -pos );                         // view Vector 
vec3 H = normalize( L + E );                        // halfway Vector

vec3 N = normalize( model_matrix*vec4(vNormal, 0.0) ).xyz;
vec4 ambient = AmbientProduct;
float cos_theta = max( dot(L, N), 0.0 );
vec4  diffuse = cos_theta * DiffuseProduct;
float cos_phi = pow( max(dot(N, H), 0.0), Shininess ); 
vec4  specular = cos_phi * SpecularProduct; 
if( dot(L, N) < 0.0 )  specular = vec4(0.0, 0.0, 0.0, 1.0); 

color = ambient + diffuse + specular;
vec4 temp = model_matrix* vec4(vNormal.xyz,1.0);
fNormal = temp.xyz;
float ratio = 1.00 / 1.52;

if(reflectionToggle == 1.0f)
R = reflect(pos, N);
else if(reflectionToggle == 2.0f)
R = refract(pos, N, 0.011);
else
R = reflect(pos, N);

color.a = 1.0;
}