/* Start Header -------------------------------------------------------
File Name: BufferObject.cpp
Purpose: Handler for VBO (vertex array object)
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/

#include "ogpch.h"
#include "BufferObject.h"

namespace OG
{
	BufferObject::BufferObject(const void* data, GLuint size, GLenum target_buffer /* = GL_ARRAY_BUFFER */, GLenum draw_mode /*= GL_STATIC_DRAW*/)
		: target_buffer_(target_buffer)
	{
		glGenBuffers(1, &id_);
		glBindBuffer(target_buffer_, id_);
		glBufferData(target_buffer_, size, data, draw_mode);
	}
	
	BufferObject::~BufferObject()
	{
		glDeleteBuffers(1, &id_);
	}

	void BufferObject::Bind() const
	{
		glBindBuffer(target_buffer_, id_);
	}

	void BufferObject::UnBind() const
	{
		glBindBuffer(target_buffer_, 0);
	}
}
