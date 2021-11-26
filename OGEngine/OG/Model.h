/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Model file that contains meshes
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef MODEL_H
#define MODEL_H

#include "OBJReader.h"
#include "shader.hpp"

namespace OG
{
	class Model
	{
	public:
		Model(const std::string &filepath);
		Model(Mesh* mesh);

		void LoadModel(const std::string& filepath);
		
		void Draw(Shader* shader, bool drawNormal = false, bool drawFaceNormal = true);

		void RemapUV();
		

		UVType uvType;
		bool isNormMapping;

		// TODO : make vector of mesh and store it separately & private
		std::unique_ptr<Mesh> pMesh_;

	private:
	};
}

#endif // !MODEL_H

