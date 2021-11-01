#include "ogpch.h"
#include "UniformBuffer.h"

OG::UniformBuffer::UniformBuffer(GLuint size, GLuint index,GLenum mode) : BufferObject(nullptr, size, GL_UNIFORM_BUFFER, mode)
{
	index_ = index;
	glBindBufferBase(target_buffer_, index_, id_);
}

void OG::UniformBuffer::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, id_);
}

void OG::UniformBuffer::UnBind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OG::UniformBuffer::BindRange(GLuint offset, GLsizeiptr size) const
{
	glBindBufferRange(target_buffer_, index_, id_, offset, size);
}

void OG::UniformBuffer::AddSubData(GLsizei offset, GLsizei baseAlignment, const GLvoid* data)
{
	Bind();

	glBufferSubData(GL_UNIFORM_BUFFER, offset, baseAlignment, data);

	UnBind();
}

void OG::UniformBuffer::AddData(GLsizei size, const GLvoid* data)
{
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
}
