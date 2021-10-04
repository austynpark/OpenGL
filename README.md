/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.md
Purpose: Load files and render with OpenGL4
Language: C++ / Visual Studio 2019
Platform: Visual Studio 2019, OpenGL4.5, Windows 10 x64 
Project: kyungook.park_CS330_1
Author: Austyn park / kyungook.park / 180001621
Creation date: 10/01/21
End Header --------------------------------------------------------*/

# CS300 Assignment1

## README REQUIREMENTS

### (a) User Interface Description 

- Move Camera with Key W,A,S,D to move foward, left, right, backward
	* W : Move Foward
	* A : Move Left
	* S : Move Right
	* D : Move Backward
- Rotate Camera with Arrow Keys
	* ← , → : Modify Yaw
	* ↑ , ↓ : Modify Pitch

### (b) How To Use

- Change the central object model through the imgui
- Toggle drawing normal option, if it is not turned on, normal will not be drawn

### (c) Completed Part

1. Setting up an OpenGL application
2. Reading data from an OBJ file without errors
3. Implementing vertex and fragment shaders corresponding to Phong Lighting
4. Scene setup for the assignment is as per specification – Central object + rotating spheres
	- Central object (OBJ file – drawn using GL_TRIANGLES)
	- Spheres (Generate procedurally – drawn using GL_TRIANGLES)
	- Orbit of the spheres (drawn using line segments – GL_LINES)
5. Face & vertex normals calculation and display

### (d) Uncompleted Part

- None

### (e) Relevant Source Code Location
1. OpenGL application

	- file path 	: OGEngine\OG\src
	- file name 	: Graphics.cpp
	- function name : Init

2. OBJ file Loader
	- file path 	: OGEngine\OG\src
	- file name 	: OBJReader.cpp
	- function name : ReadOBJFile

3. Shader
	- file path 	: OGEngine\OG\src
	- file name 	: PKO.frag, PKO.vert, shader.cpp
	- function name : LoadShaders

4. Scene
	- file path 	: OGEngine\OG\src
	- file name 	: Scene_Assignment.cpp,  Mesh.cpp
	- function name : SetBuffer, SetOrbit, DrawOrbit / CreateSphere, calcVertexPositionForBoundingBox, Draw

5. Face & vertex normal
	- file path 	: OGEngine\OG\src
	- file name 	: Mesh.cpp
	- function name : calcVertexNormals, calcVertexNormalsForDisplay, DrawNormal

### (f) Machine Tested

- OS : Windows 10 x64
- GPU Vendor : ATI Technologies Inc
- GL Renderer : AMD Ryzen 7 4800HS with Radeon Graphics
- GL Version : 4.5.13596 Core Profile Context 20.10.20.10 27.20.11020.10001
- GL Shader Version: 4.60
- GL Major Version: 4
- GL Minor Version: 5
- Current OpenGL Context is double buffered
- Maximum Vertex Count: 536870911
- Maximum Vertex Count: 536870911
- GL Maximum texture size: 16384
- GL Maximum Viewport Dimensions: 16384x16384
- Maximum vertex attributes: 29

### (g) Hours spent on the Assignment (On a Weekly basis)

- 4 Hour

### (h) Extra Info

- None