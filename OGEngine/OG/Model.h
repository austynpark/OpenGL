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

namespace OG
{
	class Model
	{
	public:
		Model(const std::string &filepath);
		Model(Mesh* mesh);

		void LoadModel(const std::string& filepath);
		
		void Draw(GLenum mode);

		// Determine if normal should be drawn
		bool drawNormal = false;
		// If true, draw Face Normal or draw Vertex Normal
		bool drawFaceNormal = true;


	private:
		std::vector<std::unique_ptr<Mesh>> pMeshes_;
	};
}

#endif // !MODEL_H

