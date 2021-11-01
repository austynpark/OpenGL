#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include "BufferObject.h"

namespace OG
{
	class UniformBuffer : BufferObject
	{
	public:
		UniformBuffer() = delete;
		UniformBuffer(GLuint size, GLuint index, GLenum mode);

		void Bind() const override;
		void UnBind() const override;
		void BindRange(GLuint offset, GLsizeiptr size) const;

		/// <summary>
		/// Add Data to Uniform Buffer
		/// </summary>
		/// <param name="offset"></param>
		/// <param name="baseAlignment">Any Scalar such as float, int, bool should have 4 byte as a base alignment
		/// vec3 must be treated as vec4, which means its base alignment must be 16 byte
		/// </param>
		/// <param name="data"></param>
		void AddSubData(GLsizei offset, GLsizei baseAlignment, const GLvoid* data);
		void AddData(GLsizei size, const GLvoid* data);

	private:
		GLuint index_;
	};
}

#endif // !UNIFORMBUFFER_H

