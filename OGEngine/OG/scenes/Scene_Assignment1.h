/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene_Assignment1.h
Purpose: Scene1, Draw Lines for orbit, load and render models, and process Input for camera movement
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef SCENE_ASSIGNMENT1_H
#define SCENE_ASSIGNMENT1_H

#include "Scene.h"
#include "shader.hpp"
#include "Model.h"

namespace OG 
{
	class Camera;
	class Object;
	class UniformBuffer;

	class Scene_Assignment1 : public Scene
	{
	public:
		Scene_Assignment1() = default;
		Scene_Assignment1(int windowWidth, int windwoHeight);
		virtual ~Scene_Assignment1();

	private:
		int Init() override;
		void CleanUp() override;

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

		std::unique_ptr<Shader> shader;

		// data members
		GLuint  vertexbuffer;
		GLuint  VertexArrayID;

		// Light Info for ambient + diffuse
		glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 0.0f);
		glm::vec3 diffuseColor = glm::vec3(1.0f);
		glm::vec3 lightColor = glm::vec3(1.0f, 0.5f, 0.0f);
		float ambientStrength = 0.1f;

		std::unique_ptr<Camera> pCamera_;

		
		std::vector<std::unique_ptr<Object>> objects_;
		std::vector<std::string> modelName_;
		std::unordered_map<std::string, std::unique_ptr<Model>> models_;
	
		const char* current_item = nullptr;

		/* Functions & Variables for Orbit */

		std::vector<std::unique_ptr<Object>> spheres_;
		float angleOfRotation = 0.0f;
		
		std::vector<glm::vec3> orbit_points;
		std::vector<GLuint> orbit_indices;

		std::unique_ptr<ArrayBufferObject> pVAO;
		std::unique_ptr<BufferObject> pVBO;
		std::unique_ptr<BufferObject> pEBO;

		void SetOrbit(float radius, int numberOfPoints);
		void DrawOrbit();

		bool is_normal_vector_on;
		bool drawFaceNormal;

		// Uniform Block Object
		std::unique_ptr<UniformBuffer> pUBO_transform;
	};

}


#endif // !SCENE_ASSIGNMENT1_H

