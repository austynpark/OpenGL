/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ArrayBufferObject.h
Purpose: Handler for VAO (vertex array object)
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef ARRAYBUFFEROBJECT_H
#define ARRAYBUFFEROBJECT_H

#include <glad/glad.h>

namespace OG
{
	class ArrayBufferObject
	{
	public:
		ArrayBufferObject();
		~ArrayBufferObject();

		void Bind();
		void UnBind();
	private:

		GLuint id_;
	};
}

#endif // !ARRAYBUFFEROBJECT_H

