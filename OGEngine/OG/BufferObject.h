/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BufferObject.h
Purpose: Handler for VBO (vertex array object)
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include <glad/glad.h>

namespace OG
{
	class BufferObject
	{
	public:
		BufferObject() = delete;
		/// <summary>
		/// BufferObject to pass
		/// </summary>
		/// <param name="data">data to pass into shader</param>
		/// <param name="size">size of data passed in</param>
		/// <param name="target_buffer">buffer to bind (EBO must pass GL_ELEMENT_BUFFER)</param>
		BufferObject(const void* data, GLuint size, GLenum target_buffer = GL_ARRAY_BUFFER, GLenum draw_mode = GL_STATIC_DRAW);
		~BufferObject();

		virtual void Bind() const;
		virtual void UnBind() const;
	protected:
		GLuint id_;
		GLenum target_buffer_;
	};
}

#endif // !BUFFEROBJECT_H

