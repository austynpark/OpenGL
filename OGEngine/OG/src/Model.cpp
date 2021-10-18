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
	Model::Model(const std::string& filepath)
	{
		LoadModel(filepath);
	}

	Model::Model(Mesh* mesh)
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

	void Model::Draw(GLenum mode)
	{
		for (const auto& mesh : pMeshes_)
		{
			mesh->Draw(mode);
			if (drawNormal) {
				for (const auto& mesh : pMeshes_)
				{
					mesh->DrawNormal(drawFaceNormal);
				}
			}

		}
	}
}
