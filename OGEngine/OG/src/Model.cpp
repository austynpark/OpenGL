/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Load Model and put in vector mesh that contains rendering info
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "Model.h"

namespace OG
{
	Model::Model(const std::string& filepath) : uvType(UVType::PLANAR_UV), isNormMapping(true)
	{
		LoadModel(filepath);
	}

	Model::Model(Mesh* mesh) : uvType(UVType::PLANAR_UV)
	{
		pMeshes_.emplace_back(std::move(mesh));
		mesh->SetBuffer();
	}

	void Model::LoadModel(const std::string& filepath)
	{
		OBJReader::getInstance()->ReadOBJFile(filepath, pMeshes_, false);

		// This is unnecessary since Mesh constructor calls setbuffer 
		for (const auto& mesh : pMeshes_)
		{
			mesh->SetBuffer();
		}
	}

	void Model::Draw(Shader* shader)
	{
		for (const auto& mesh : pMeshes_)
		{
			shader->SetUniform1f("zMin", mesh->boundingBox[0].z);
			shader->SetUniform1f("zMax", mesh->boundingBox[1].z);

			shader->SetUniform1b("isNormMapping", isNormMapping);
			mesh->Draw();
			if (drawNormal) {
				mesh->DrawNormal(drawFaceNormal);
			}
		}
	}
	void Model::RemapUV()
	{
		for (const auto& mesh : pMeshes_)
		{
			mesh->calcUVs(uvType, isNormMapping);
			mesh->MapVertexBuffer();
			mesh->SetBuffer();
		}
	}
}
