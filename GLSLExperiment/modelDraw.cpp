// Draws colored cube  

#include "Angel.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib> 
#include <ctime> 
#include "bmpread.h"

//----------------------------------------------------------------------------


using namespace std;
int width = 0;
int height = 0;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

vec3 *faceNormals;
vec3 *vertexNormals;
vec3 *pointsNormals;

GLuint program;
GLuint program2;
GLuint program4;

GLuint buffer;
GLuint walls_buffer;

GLubyte wall_solid[64][64][3];
GLubyte floor_solid[64][64][3];

const int NumVertices = 90000;
const int NUMFACES = 60000;

point4 points[NumVertices];
color4 colors[NumVertices];
vec4 edgeList[NUMFACES];
int vertices;
int faces;
vec3 vertexList3[NumVertices];
vec4 vertexList[NumVertices];
GLuint pointsIndex = 0;
string fileName = "";
float min_x = 10000000.0F;
float max_x = 0.0F;
float min_y = 10000000.0F;
float max_y = 0.0;
float min_z = 10000000.0F;
float max_z = 0.0;


float mean_x = 0.0F;
float mean_y = 0.0F;
float mean_z = 0.0F;

float translate_scale = 1.2f;



float y_rotation = 0.0f;

GLuint vao;
GLuint walls_vao;
unsigned int cubemapID;

vec4 walls[18];

bmpread_t stone_bmp;
bmpread_t grass_bmp;

GLuint  stone_texture = 0;
GLuint  grass_texture = 0;

GLuint walls_solid_texture = 0;
GLuint floor_solid_texture = 0;

vec4 light0_pos = vec4(3.0, 5.0, 5.0, 1.0);

char* textures_faces[6] = { "nvposx.bmp","nvnegx.bmp","nvposy.bmp","nvnegy.bmp","nvposz.bmp","nvnegz.bmp" };
bool shadow_show_toggle = true;
bool texture_toggle = true;
bool reflection_toggle = true;
bool refraction_toggle = false;


color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

//functions
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void parsePly(string plyFile, point4* arr, int color);
void createFaceTriangles(point4 * arr, int color);
void idle(void);
void initLightingAndMaterials(int color);
void loadSolidTextures() {
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++) {
			wall_solid[i][j][0] = 0;
			wall_solid[i][j][1] = 0;
			wall_solid[i][j][2] = 255;
			floor_solid[i][j][0] = 211; 
			floor_solid[i][j][1] = 211;
			floor_solid[i][j][2] = 211;
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &walls_solid_texture);
	glBindTexture(GL_TEXTURE_2D, walls_solid_texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, wall_solid);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &floor_solid_texture);
	glBindTexture(GL_TEXTURE_2D, floor_solid_texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, floor_solid);


}
void loadTextures() {
	//stone texture
	if (!bmpread("stones.bmp", 0, &stone_bmp))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "stone.bmp");
		exit(1);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &stone_texture);
	glBindTexture(GL_TEXTURE_2D, stone_texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stone_bmp.width, stone_bmp.height, 0, GL_RGB, GL_UNSIGNED_BYTE, stone_bmp.rgb_data);
	bmpread_free(&stone_bmp);
	
	//grass texture
	if (!bmpread("grass.bmp", 0, &grass_bmp))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "usain_bolt.bmp");
		exit(1);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &grass_texture);
	glBindTexture(GL_TEXTURE_2D, grass_texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grass_bmp.width, grass_bmp.height, 0, GL_RGB, GL_UNSIGNED_BYTE, grass_bmp.rgb_data);
	bmpread_free(&grass_bmp);
}
void loadCubeMap() {
	
	glGenTextures(1, &cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	int width, height, nrChannels;
	unsigned char *data;
	for (GLuint i = 0; i < 6; i++)
	{
		bmpread(textures_faces[i], 0, &grass_bmp);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, grass_bmp.width, grass_bmp.height, 0, GL_RGB, GL_UNSIGNED_BYTE, grass_bmp.rgb_data);
		bmpread_free(&grass_bmp);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}



void parsePly(string plyFile, point4* arr, int color) {

	ifstream plyFileStream;
	plyFileStream.open(plyFile);
	string plyFileLine;
	string chk;
	string garbage;
	plyFileStream >> plyFileLine;
	if (plyFileLine != "ply") {

		return;
	}

	getline(plyFileStream, garbage);//ends line 1
	getline(plyFileStream, garbage); //ascii line skip

	getline(plyFileStream, garbage, ' '); //"element"
	getline(plyFileStream, garbage, ' '); //"vertex"
	getline(plyFileStream, garbage);
	vertices = stoi(garbage);

	getline(plyFileStream, garbage);//property
	getline(plyFileStream, garbage);//lines
	getline(plyFileStream, garbage);//skipped
	getline(plyFileStream, garbage, ' '); //"element"
	getline(plyFileStream, garbage, ' '); //"face"
	getline(plyFileStream, garbage);
	faces = stoi(garbage);

	getline(plyFileStream, garbage);//property
	getline(plyFileStream, garbage);//end_header

	min_x = 10000000.0F;
	max_x = 0.0F;
	min_y = 10000000.0F;
	max_y = 0.0;
	min_z = 10000000.0F;
	max_z = 0.0;

	mean_x = 0.0F;
	mean_y = 0.0F;
	mean_z = 0.0F;

	for (int i = 0; i < vertices; i++) {
		plyFileStream >> vertexList3[i];

		if (vertexList3[i].x < min_x) {
			min_x = vertexList3[i].x;
		}
		if (vertexList3[i].x > max_x) {
			max_x = vertexList3[i].x;
		}
		if (vertexList3[i].y < min_y) {
			min_y = vertexList3[i].y;
		}
		if (vertexList3[i].y > max_y) {
			max_y = vertexList3[i].y;
		}
		if (vertexList3[i].z < min_z) {
			min_z = vertexList3[i].z;
		}
		if (vertexList3[i].z > max_z) {
			max_z = vertexList3[i].z;
		}

	}

	mean_x = (min_x + max_x) / 2;
	mean_y = (min_y + max_y) / 2;
	mean_z = (min_z + max_z) / 2;

	for (int i = 0; i < vertices; i++) {
		vertexList3[i].x -= mean_x; //center at x
		vertexList3[i].y -= mean_y; //center at y
		vertexList3[i].z -= mean_z; //center at z
		vertexList[i] = vec4(vertexList3[i], 1.0F);
	}

	for (int i = 0; i < faces; i++) {
		plyFileStream >> edgeList[i];
	}
	plyFileStream.clear();
	plyFileStream.close();

createFaceTriangles(arr, color);

}


void initLightingAndMaterials(int color) {
	//lights
	vec4 diffuse0 = vec4(0.7, 0.6, 0.8, 1.0);
	vec4 ambient0 = vec4(0.3, 0.4, 0.3, 1.0);
	vec4 specular0 = vec4(0.5, 0.4, 0.3, 1.0);

	vec4 light0_pos = vec4(3.0, 3.0, 5.0, 1.0);

	//materials
	vec4 ambient = vertex_colors[color];
	vec4 diffuse = vec4(1.0, 0.8, 0.0, 1.0);
	vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);
	GLfloat shine = 100.0;

	//products
	vec4 ambientProduct = ambient0 * ambient;
	vec4 specularProduct = specular0 * specular;
	vec4 diffuseProduct = diffuse0 * diffuse;


	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambientProduct);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specularProduct);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light0_pos);
	glUniform1f(glGetUniformLocation(program, "Shininess"), shine);


}

void createFaceTriangles(point4 * arr, int color) {
	pointsIndex = 0;

	faceNormals = (vec3 *)malloc(faces * sizeof(vec3));
	//vertexNormals
	for (int i = 0; i < faces; i++) {
		int vert1 = edgeList[i].y;
		arr[pointsIndex] = vertexList[vert1];
		colors[pointsIndex] = vertex_colors[color]; //randomly assign one of 8 colors
		pointsIndex++;
		int vert2 = edgeList[i].z;
		arr[pointsIndex] = vertexList[vert2];
		colors[pointsIndex] = vertex_colors[color];
		pointsIndex++;
		int vert3 = edgeList[i].w;
		arr[pointsIndex] = vertexList[vert3];
		colors[pointsIndex] = vertex_colors[color];
		pointsIndex++;

		vec4 seg1 = vertexList[vert2] - vertexList[vert1];
		vec4 seg2 = vertexList[vert3] - vertexList[vert1];
		faceNormals[i] = Angel::normalize(Angel::cross(seg1, seg2));
	}

	vertexNormals = (vec3 *)malloc(vertices * sizeof(vec3));

	for (int i = 0; i < vertices; i++) {
		vec3 vNormal = vec3(0, 0, 0);
		for (int j = 0; j < faces; j++) {
			if (i == edgeList[j].y || i == edgeList[j].z || i == edgeList[j].w)
				vNormal += faceNormals[j];
		}

		vertexNormals[i] = Angel::normalize(vNormal);
	}


	pointsNormals = (vec3 *)malloc(faces * 3 * sizeof(vec3));
	int pointsNormalIndex = 0;
	for (int i = 0; i < faces; i++) {
		int vert1 = edgeList[i].y;
		pointsNormals[pointsNormalIndex] = vertexNormals[vert1];
		pointsNormalIndex++;
		int vert2 = edgeList[i].z;
		pointsNormals[pointsNormalIndex] = vertexNormals[vert2];
		pointsNormalIndex++;
		int vert3 = edgeList[i].w;
		pointsNormals[pointsNormalIndex] = vertexNormals[vert3];
		pointsNormalIndex++;
	}



}

void drawWalls() {

	//face 1
	walls[0] = vec4(0.0, 0.0, 0.0, 1.0);
	walls[1] = vec4(0.0, 10.0, 0.0, 1.0);
	walls[2] = vec4(10.0, 0.0, 7.0, 1.0);
	
	walls[3] = vec4(10.0, 0.0, 7.0, 1.0);
	walls[4] = vec4(10.0, 10.0, 7.0, 1.0);
	walls[5] = vec4(0.0, 10.0, 0.0, 1.0);

	//face left
	walls[6] = vec4(0.0, 0.0, 0.0, 1.0);
	walls[7] = vec4(0.0, 10.0, 0.0, 1.0);
	walls[8] = vec4(-10.0, 0.0, 7.0, 1.0);

	walls[9] = vec4(-10.0, 0.0, 7.0, 1.0);
	walls[10] = vec4(-10.0, 10.0, 7.0, 1.0);
	walls[11] = vec4(0.0, 10.0, 0.0, 1.0);

	//face bottom
	walls[12] = vec4(-10.0, 0.0, 7.0, 1.0);
	walls[13] = vec4(0.0, 0.0, 0.0, 1.0);
	walls[14] = vec4(0.0, -2.0, 14.0, 1.0);


	walls[15] = vec4(0.0, 0.0, 0.0, 1.0);
	walls[16] = vec4(10.0, 0.0, 7.0, 1.0);
	walls[17] = vec4(0.0, -2.0, 14.0, 1.0);

}



void generateGeometry( void )
{	
  
	parsePly("cow.ply",points,1);
	loadSolidTextures();
	loadTextures();
	loadCubeMap();


	//VAO generation
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	
	//VBO 
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*faces * 3 + sizeof(point4)*faces * 3 + sizeof(vec3) * faces * 3 , //points + colors + normals 
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*faces * 3, points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*faces * 3, sizeof(point4)*faces * 3, colors);

	glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(point4)*faces * 3, sizeof(vec3) * faces * 3, pointsNormals);

	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);

	//adds lights and materials 
	//argument is the color
	initLightingAndMaterials(3);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*faces * 3));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(2 * sizeof(point4)*faces * 3));



	// VAO for walls
	glGenVertexArrays(1, &walls_vao);
	glBindVertexArray(walls_vao);

	// VBO for walls
	
	glGenBuffers(1, &walls_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, walls_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * 18, //faces + texture
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * 18, walls);

	program2 = InitShader("vshader2.glsl", "fshader2.glsl");
	glUseProgram(program2);
	GLuint vP = glGetAttribLocation(program2, "vPosition");
    glEnableVertexAttribArray( vP );
    glVertexAttribPointer( vP, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

	
	program4 = InitShader("vshader_refraction.glsl", "fshader_refraction.glsl");
	glUseProgram(program4);
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//especially for shadow creation
void drawCow() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
	glUseProgram(program);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	GLuint reflection_toggle_info = glGetUniformLocation(program, "reflectionToggle");
	if (reflection_toggle)
		glUniform1f(reflection_toggle_info, 1.0f);
	else
		glUniform1f(reflection_toggle_info, 0.0f);

	if (refraction_toggle)
		glUniform1f(reflection_toggle_info, 2.0f);


	glDrawArrays(GL_TRIANGLES, 0, faces * 3);
}

void draw(void)
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_DEPTH_TEST );
	glBindVertexArray(vao);
	glUseProgram(program);
	
	//load cube map
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	
	//passing reflection toggle to the shader
	//switch between reflection and refraction
	GLuint reflection_toggle_info = glGetUniformLocation(program, "reflectionToggle");
	
	if(reflection_toggle)
	glUniform1f(reflection_toggle_info, 1.0f);
	else
	glUniform1f(reflection_toggle_info, 0.0f);

	if(refraction_toggle)
		glUniform1f(reflection_toggle_info, 2.0f);


	glDrawArrays(GL_TRIANGLES, 0, faces * 3);
	glBindVertexArray(walls_vao);
	
	//draw walls and floors
	glUseProgram(program2);
	if (texture_toggle) 
		glBindTexture(GL_TEXTURE_2D, stone_texture);
	else
		glBindTexture(GL_TEXTURE_2D, walls_solid_texture);

	
		glDrawArrays(GL_TRIANGLES, 0, 12);

		if (texture_toggle)
		glBindTexture(GL_TEXTURE_2D, grass_texture);
		else
		glBindTexture(GL_TEXTURE_2D, floor_solid_texture);

		glDrawArrays(GL_TRIANGLES, 12, 18);
	
	

	glDisable( GL_DEPTH_TEST ); 
}





void display( void )
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat)5000.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];


	Angel::mat4 modelMat = Angel::identity();
	Angel::mat4 modelMat2 = Angel::identity();


	
	modelMat = modelMat * Angel::Translate(0, 3, -25) * Angel::RotateY(y_rotation);
	modelMat2 = modelMat2 * Angel::Translate(0, 0, -35);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];

	float modelMatrixf2[16];
	modelMatrixf2[0] = modelMat2[0][0]; modelMatrixf2[4] = modelMat2[0][1];
	modelMatrixf2[1] = modelMat2[1][0]; modelMatrixf2[5] = modelMat2[1][1];
	modelMatrixf2[2] = modelMat2[2][0]; modelMatrixf2[6] = modelMat2[2][1];
	modelMatrixf2[3] = modelMat2[3][0]; modelMatrixf2[7] = modelMat2[3][1];

	modelMatrixf2[8] = modelMat2[0][2]; modelMatrixf2[12] = modelMat2[0][3];
	modelMatrixf2[9] = modelMat2[1][2]; modelMatrixf2[13] = modelMat2[1][3];
	modelMatrixf2[10] = modelMat2[2][2]; modelMatrixf2[14] = modelMat2[2][3];
	modelMatrixf2[11] = modelMat2[3][2]; modelMatrixf2[15] = modelMat2[3][3];
	

	glUseProgram(program);
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);
	glUseProgram(program2);
	modelMatrix = glGetUniformLocationARB(program2, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf2);
	viewMatrix = glGetUniformLocationARB(program2, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);

	draw();
	
	//shadow stuff
	mat4 shadowMat = Angel::identity();
	shadowMat[3][1] = -1.0 / light0_pos.y;

	glUseProgram(program);
	GLuint shadow_toggle = glGetUniformLocation(program, "shadowToggle");
	glUniform1f(shadow_toggle, 1.0f);

	modelMat = Angel::identity();
	modelMat = modelMat *Angel::Translate(0, -3, -25)
		*Angel::Translate(light0_pos.x, light0_pos.y, light0_pos.z) 
		*shadowMat * Angel::Translate(-light0_pos.x, -light0_pos.y, -light0_pos.z)
		* Angel::RotateY(y_rotation) ;

	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];
	glUseProgram(program);
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);
	
	if(shadow_show_toggle)
	drawCow();
	shadow_toggle = glGetUniformLocation(program, "shadowToggle");
	glUniform1f(shadow_toggle, 0.0f);
	glutSwapBuffers();
}

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	switch (key) {
	case 'b':
		if (texture_toggle)
			texture_toggle = false;
		else
			texture_toggle = true;

		glutPostRedisplay();
		break;
	case'c':
		if (reflection_toggle)
		{
			reflection_toggle = false;
			refraction_toggle = false;
		}
		else
			reflection_toggle = true;

		glutPostRedisplay();
		break;
	case'd':
		if (refraction_toggle) {
			refraction_toggle = false;
			reflection_toggle = false;
		}
		else
			refraction_toggle = true;

		glutPostRedisplay();

		break;
	case'a':
		if (shadow_show_toggle)
			shadow_show_toggle = false;
		else
			shadow_show_toggle = true;
	}
}

void idle(void) {
	y_rotation += 2.0f;
	
	if (y_rotation > 360.0f )
		y_rotation = 0.0f;
			
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 1024, 1024);
	width = 1024;
	height = 1024;
	glutCreateWindow("Cow Phong / Reflection / Refraction");
    glewInit();
	drawWalls();

    generateGeometry();

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
