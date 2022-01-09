/* Start Header -------------------------------------------------------
File Name: Model.h
Purpose: Load Model and put in vector mesh that contains rendering info
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
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
		pMesh_.reset(mesh);
		mesh->SetBuffer();
	}

	void Model::LoadModel(const std::string& filepath)
	{
		OBJReader::getInstance()->ReadOBJFile(filepath, pMesh_, false);

		// This is unnecessary since Mesh constructor calls setbuffer 
		pMesh_->SetBuffer();
	}

	void Model::Draw(Shader* shader)
	{
		pMesh_->Draw();
	}

	void Model::DrawNormal(bool drawFaceNormal)
	{
		pMesh_->DrawNormal(drawFaceNormal);
	}

	void Model::RemapUV()
	{
		pMesh_->calcUVs(uvType, isNormMapping);
		pMesh_->MapVertexBuffer();
		pMesh_->SetBuffer();
	}
}
