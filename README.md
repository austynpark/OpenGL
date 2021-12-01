/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.md
Purpose: Load files and render with OpenGL4
Language: C++ / Visual Studio 2019
Platform: Visual Studio 2019, OpenGL4.5, Windows 10 x64 
Project: kyungook.park_CS330
Author: Austyn park / kyungook.park / 180001621
Creation date: 10/01/21
End Header --------------------------------------------------------*/

# CS300 Assignment3

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

Central Object
- Change the central object model through the imgui
- Toggle drawing normal option, if it is not turned on, normal will not be drawn
- Choose the texture entity and projection mode
- 'Calculate UV' button should be pressed to refresh UV calculation on the CPU side.

Shader
- 'Reload Shader' will recompile the shader

Light
- Select specific lights te be used in the scene by changing the number of active lights
- Select the light types. Default light type is a point light
- Toggle between the three scenarios
- Toggle 'Rotate' to pause/start the light rotation

Reflect / Refract
- Turning Reflect & Refract will enable Fresnel


### (c) Completed Part

1. Sky box rendering
2. Scene & Environment Map generation Setup
3. Environment Map application in shaders
4. User Interactivity - Rendering

### (d) Uncompleted Part
- None

### (e) Relevant Source Code Location
1. FBO
	- file path 	: OGEngine\OG\src
	- file name 	: FBO.cpp, Scene_Assignment2.cpp
	- function name : setFramebuffer, updateFramebuffer

2. Reflect / Refract
	- file path 	: OGEngine\OG\shaders
	- file name 	: PhongShading.frag, PhongShading.vert
	- function name : CalcRefract / CalcCubeMap / CalcReflectMap

3. Skybox
	- file path 	: OGEngine\OG\src
	- file name 	: CubeMap.cpp

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

- 8 Hour

### (h) Extra Info

- None