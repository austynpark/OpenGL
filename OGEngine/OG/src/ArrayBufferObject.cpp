/* Start Header -------------------------------------------------------
File Name: ArrayBufferObject.cpp
Purpose: Handler for VAO (vertex array object)
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/

#include "ogpch.h"
#include "ArrayBufferObject.h"

namespace OG
{
	ArrayBufferObject::ArrayBufferObject()
	{
		glGenVertexArrays(1, &id_);
		glBindVertexArray(id_);
	}
	ArrayBufferObject::~ArrayBufferObject()
	{
		glDeleteVertexArrays(1, &id_);
	}
	void ArrayBufferObject::Bind()
	{
		glBindVertexArray(id_);
	}
	void ArrayBufferObject::UnBind()
	{
		glBindVertexArray(0);
	}
}
