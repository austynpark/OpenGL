/* Start Header -------------------------------------------------------
File Name: ArrayBufferObject.h
Purpose: Handler for VAO (vertex array object)
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
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

