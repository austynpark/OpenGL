#include "ogpch.h"
#include "CubeMap.h"
#include "Texture.h"
#include "Model.h"
#include "shader.hpp"
#include "Camera.h"

OG::CubeMap::CubeMap() : skybox(std::make_unique<Model>("OG/models/cube.obj")), shader(std::make_unique<Shader>("CubeMap.vert", "CubeMap.frag"))
{
	camera_lookat.resize(6);

	// Camera is upside down since cubemap algorithm assumes that image is vertically flipped
	camera_lookat.at(0) = glm::lookAt(glm::vec3(0.0f), glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)); // RIGHT
	camera_lookat.at(1) = glm::lookAt(glm::vec3(0.0f), glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)); // LEFT
	camera_lookat.at(2) = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)); // TOP
	camera_lookat.at(3) = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)); // BOTTOM
	camera_lookat.at(4) = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)); // FRONT
	camera_lookat.at(5) = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)); // BACK
	
}

OG::CubeMap::~CubeMap()
{
}

bool OG::CubeMap::Init(const char** faces_file_path, int first_tex_Num)
{
	faces.resize(6);

	for (int i = 0; i < 6; ++i)
	{
		faces[i] = std::make_unique<SkyboxTexture>(faces_file_path[i], first_tex_Num + i);
	}

	return true;
}

void OG::CubeMap::Render(const glm::mat4& view, const glm::mat4& projection)
{
	shader->Use();

	for (int i = 0; i < 6; ++i) {
		faces[i]->Bind();
	}
	
	shader->SetUniform1i("right", faces[0]->texNum_);
	shader->SetUniform1i("left", faces[1]->texNum_);
	shader->SetUniform1i("top", faces[2]->texNum_);
	shader->SetUniform1i("bottom", faces[3]->texNum_);
	shader->SetUniform1i("front", faces[4]->texNum_);
	shader->SetUniform1i("back", faces[5]->texNum_);

	shader->SetUniformMatrix4fv("view", view);
	shader->SetUniformMatrix4fv("projection", projection);
	skybox->Draw(shader.get());

	for (int i = 0; i < 6; ++i) {
		faces[i]->UnBind();
	}
}
