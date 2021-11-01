/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene_Assignment2.h
Purpose: Scene2, Implementing Phong Illumination model with texture & different light
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/10/17
End Header --------------------------------------------------------*/

#ifndef SCENE_ASSIGNMENT2_H
#define SCENE_ASSIGNMENT2_H

#include "Scene.h"
#include "shader.hpp"
#include "Texture.h"

#include "UVType.h"

namespace OG 
{
	class Camera;
	class Object;
	class UniformBuffer;
	class ArrayBufferObject;
	class BufferObject;

	enum lightType : int
	{
		E_POINT = 0,
		E_SPOT = 1,
		E_DIR = 2
	};

	struct Light
	{
		GLfloat cutOff = 12.5f;
		GLfloat outerCutOff = 17.5f;
		GLfloat fallOut = 1.0f;
		GLint type = E_POINT;
		glm::vec4 ambient{ 1.0f, 1.0f, 1.0f, 0.0f};
		glm::vec4 diffuse{ 1.0f, 1.0f, 1.0f, 0.0f};
		glm::vec4 specular{ 1.0f, 1.0f, 1.0f, 0.0f};
		glm::vec4 position{ 0.0f,0.0f,0.0f, 1.0f};
		glm::vec4 direction{ 0.0f, 0.0f, 0.0f, 0.0f};
	};

	struct Orbit
	{
		Orbit() = delete;
		Orbit(float radius, int numberOfPoints);
		~Orbit();

		std::unique_ptr<ArrayBufferObject> pVAO;
		std::unique_ptr<BufferObject> pVBO;
		std::unique_ptr<BufferObject> pEBO;

		void SetOrbit();
		void DrawOrbit();
		void SetupBuffer();

		std::vector<glm::vec3> orbit_points;
		std::vector<GLuint> orbit_indices;

		float radius;
		int numberOfPoints;
		float prev_radius;
	};


	class Scene_Assignment2 : public Scene
	{
	public:
		Scene_Assignment2() = default;
		Scene_Assignment2(int windowWidth, int windowHeight);
		virtual ~Scene_Assignment2();

	private:
		int Init() override;
		void CleanUp() override;

		int preRender(double dt) override;
		int Render(double dt) override;
		int postRender(double dt) override;

		void keyboardInput(GLFWwindow* pWindow, float dt) override;

	private:
		// This is the non-software engineered portion of the code
		// Go ahead and modularize the VAO and VBO setup into
		// BufferManagers and ShaderManagers
		void SetupBuffers();

		void SetupImGui(GLFWwindow* pWindow) override;
		void DrawImGui(GLFWwindow* pWindow) override;
		void CleanupImGui() override;

		//std::unique_ptr<Shader> PhongLighting;
		//std::unique_ptr<Shader> PhongShading;

		std::unordered_map<std::string, std::unique_ptr<Shader>> shaders_;
		
		const char* current_shader = nullptr;
		std::unique_ptr<Shader> lightShader;

		// Light Info
		Light light[16];

		std::unique_ptr<Camera> pCamera_;

		std::vector<std::unique_ptr<Object>> objects_;
		std::unique_ptr<Object> plane_;
		// Draw Normal
		bool is_normal_vector_on;
		bool drawFaceNormal;

		const char* current_item = nullptr;

		/* Functions & Variables for Orbit */
		std::vector<std::unique_ptr<Object>> spheres_;
		std::unique_ptr<Orbit> pOrbit;
		float angleOfRotation = 0.0f;
		bool isRotating;
		
		// Uniform Block Object
		std::unique_ptr<UniformBuffer> pUBO_transform;
		std::unique_ptr<UniformBuffer> pUBO_light;

		// Global Uniform Block Var //
		glm::vec3 att{ 1.0f,0.09f,0.032f };
		glm::vec3 global_ambient{0.0f, 0.0f, 0.1f};
		glm::vec3 fog_color{ 0.2f, 0.4f, 0.55f };
		float near = 0.1f;
		float far = 20.0f;
		int num_of_lights;
		//////////////////////////////

		std::unique_ptr<Texture> pDiffuseTexture;
		std::unique_ptr<Texture> pSpecularTexture;

		bool bVisualizeTex;
		bool bCalcOnCPU;
		std::unique_ptr<Texture> pVisualizeTexture;
	};
}


#endif // !SCENE_ASSIGNMENT2_H

