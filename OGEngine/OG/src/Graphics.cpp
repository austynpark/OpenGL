/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Graphics.cpp
Purpose: Main System Handler, Including GLFW, glad, scene init, and call render function of scene.
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "Graphics.h"

#include "scenes/Scene_Assignment1.h"
#include "Camera.h"

namespace OG
{

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	
	
	Graphics* GRAPHICS = nullptr;

	Graphics::Graphics()
	{
		GRAPHICS = this;
	}

	Graphics::~Graphics()
	{
		if (scene != nullptr)
			delete scene;
		scene = nullptr;
	}

	bool Graphics::Init()
	{
		if (!glfwInit())
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			return false;
		}

		// Setting up OpenGL properties
		glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Open a window and create its OpenGL context
		window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
			"Sample 1 - Simple scene (Quad) with Scene Class", // window title
			nullptr, // which monitor (if full-screen mode)
			nullptr); // if sharing context with another window

		if (window == nullptr)
		{
			fprintf(stderr,
				"Failed to open GLFW window. Check if your GPU is 4.5 compatible.\n");
			glfwTerminate();
			return false;
		}

		// OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
		// context but does NOT make the created context "current". We MUST make it current with the following
		// call
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		// glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}

		/////////////////////////////////////////////////////////////////////////
		GLubyte const* str_ven = glGetString(GL_VENDOR);
		cout << "GPU Vendor" << str_ven << endl;

		GLubyte const* str_renderer = glGetString(GL_RENDERER);
		cout << "GL Renderer: " << str_renderer << endl;

		GLubyte const* str_version = glGetString(GL_VERSION);
		cout << "GL Version: " << str_version << endl;

		GLubyte const* str_shaderVer = glGetString(GL_SHADING_LANGUAGE_VERSION);
		cout << "GL Shader Version: " << str_shaderVer << endl;

		GLint majorVer, minorVer;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVer);
		glGetIntegerv(GL_MINOR_VERSION, &minorVer);
		cout << "GL Major Version: " << majorVer << endl;
		cout << "GL Minor Version: " << minorVer << endl;

		GLboolean isDoubleBuffered;
		glGetBooleanv(GL_DOUBLEBUFFER, &isDoubleBuffered);
		if (isDoubleBuffered) {
			cout << "Current OpenGL Context is double buffered" << endl;
		}

		GLint maxVertCount, maxIndexCount;
		glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertCount);
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndexCount);
		cout << "Maximum Vertex Count: " << maxVertCount << endl;
		cout << "Maximum Vertex Count: " << maxIndexCount << endl;

		GLint tex_size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tex_size);
		cout << "GL Maximum texture size: " << tex_size << endl;

		GLint maxViewports[2];
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewports);
		cout << "GL Maximum Viewport Dimensions: " << maxViewports[0] << "x" << maxViewports[1] << endl;

		GLint max_vtx_attribs;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vtx_attribs);
		std::cout << "Maximum vertex attributes: " << max_vtx_attribs << std::endl;
		/////////////////////////////////////////////////////////////////////////

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		// Initialize the scene
		//scene = new SimpleScene_Quad(windowWidth, windowHeight);
		scene = new Scene_Assignment1(windowWidth, windowHeight);
		
		// Scene::Init encapsulates setting up the geometry and the texture
		// information for the scene
		scene->Init();

		//glfwSetCursorPosCallback(window, mouse_callback);
		//glfwSetScrollCallback(window, scroll_callback);

		return true;
	}

	void Graphics::Update(double dt)
	{
		// Now render the scene
		// Scene::Display method encapsulates pre-, render, and post- rendering operations
		scene->Display(dt);
	}

	void Graphics::CleanUp()
	{
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

}

