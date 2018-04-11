# This project demonstrates:
1.	Calculating face and vertex normals.
2.	Phong shading.
3.	Reflection.
4.	Refraction.
5.	Cube maps.
6.	Texture maps.
7.	Solid textures.
8.	Projection shadows.

## 1.	Background with solid textures.


![cow_no_texture](https://user-images.githubusercontent.com/7290739/38602958-534de68a-3d3a-11e8-88b8-b93093fb36c4.png)

## 2.	Cow model with Phong Shading.


![cow_phong](https://user-images.githubusercontent.com/7290739/38602994-6ff57316-3d3a-11e8-9f0f-371ea4663766.png)

## 3.	Cow model with Reflection of cube map.


![cow_reflection](https://user-images.githubusercontent.com/7290739/38603029-7b635218-3d3a-11e8-9c4d-8a37c8cb4010.png)

## 4.	Cow model with Refraction of cube map.

![cow_refraction](https://user-images.githubusercontent.com/7290739/38603056-8a92d8bc-3d3a-11e8-9ccc-69a46496746f.png)


## Files to view:

For core OpenGL functionality see following files

modelDraw.cpp inside GLSLExperiment directory.

for the model:

fshader1.glsl
vshader1.glsl

for the background:

fshader2.glsl
vshader2.glsl


# Instructions on how to run:
<pre>
Open a GLSLExperiment.sln in Visual Studio 18
Please run multiple times if it crashes 
(Angel's library causes shader compilation crashes)

Key 'a': Toggle shadows ON/OFF.
Shadows are shown using projection.
The toggle is passed to fragment shader for the second object drawn.
That decides the color of the shadow.
The code exists in fragment and vertex shader 1.


Key 'b': Toggle ON/OFF between a scene with grass texture on floor and stone texture on the walls and a plain wall (no texturing). 
uses program 2 
fragment shader and vertex shader 2 have this code.
I create textures from bitmap and then in fragmentshader 2 I apply texture.
I scale the textures by diving by 3 in vertex shader 2.
TextCoord = vPosition.xy/3;
vPosition is used for s,t


Key 'c': Toggle reflection ON/OFF. When ON, the hierarchy/cow PLY is drawn with reflection.
 When OFF, the hierarchy/cow PLY objects is drawn with no reflection 
 (rendered as a solid model(s) with per-fragment lighting).

reflection code exists in vertex shader 1.
It checks a toggle
if(reflectionToggle == 1.0f)
R = reflect(pos, N);
else if(reflectionToggle == 2.0f)
R = refract(pos, N, 0.011);
else
R = reflect(pos, N);

proper R is then given to fragment shader.


Key 'd': Toggle refraction ON/OFF. When ON, the hierarchy/cow PLY is drawn with refraction. 
When OFF, the hierarchy/cow PLY file is drawn with no refraction 
( rendered as a solid model(s) with per-fragment lighting). 

same as above
R = reflect(pos, N);
else if(reflectionToggle == 2.0f)
R = refract(pos, N, 0.011);
else
R = reflect(pos, N);

The lower refraction ratio draws cow well.
</pre>
