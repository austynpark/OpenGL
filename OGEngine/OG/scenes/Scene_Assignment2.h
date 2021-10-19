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

namespace OG 
{
	class Camera;
	class Object;
	class UniformBuffer;
	class ArrayBufferObject;
	class BufferObject;

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

		std::unique_ptr<Shader> PhongLighting;
		std::unique_ptr<Shader> PhongShading;

		// Light Info for ambient + diffuse
		glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 0.0f);
		glm::vec3 diffuseColor = glm::vec3(1.0f);
		glm::vec3 lightColor = glm::vec3(1.0f, 0.5f, 0.0f);
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		std::unique_ptr<Camera> pCamera_;

		std::vector<std::unique_ptr<Object>> objects_;
	
		const char* current_item = nullptr;

		/* Functions & Variables for Orbit */
		std::vector<std::unique_ptr<Object>> spheres_;
		float angleOfRotation = 0.0f;
		
		bool is_normal_vector_on;
		bool drawFaceNormal;

		// Uniform Block Object
		std::unique_ptr<UniformBuffer> pUBO_transform;
		std::unique_ptr<UniformBuffer> pUBO_light;

		std::unique_ptr<Texture> pDiffuseTexture;
		std::unique_ptr<Texture> pSpecularTexture;

		std::unique_ptr<Orbit> pOrbit;
	};
}


#endif // !SCENE_ASSIGNMENT2_H

