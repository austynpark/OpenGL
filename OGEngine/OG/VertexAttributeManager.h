/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VertexAttributeMananger.h
Purpose: Helper class that calculates a layout of the buffer
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef VERTEXATTRIBUTEMANAGER_H
#define VERTEXATTRIBUTEMANAGER_H

#include <glad/glad.h>
#include <vector>

namespace OG
{
	struct VertexBufferElement
	{
		VertexBufferElement(GLuint count, GLuint type, GLsizei typeSize, GLuint isNormalized)
		{
			count_ = count;
			type_ = type;
			typeSize_ = typeSize;
			isNormalized_ = isNormalized;
		}

		GLuint count_;
		GLuint type_;
		GLsizei typeSize_;
		GLuint isNormalized_;
	};

	class VertexAttributeManager
	{
	public:
		VertexAttributeManager() {
			stride_ = 0;
		}
		
		/// <summary>
		/// Calculate stride and type of each attribute to make easy when setting buffer attribute
		/// </summary>
		/// <param name="count"> : number of components per attribute element (should be 1,2,3,4) </param>
		/// <param name="type"> : type of the element i.e) GL_Float,GL_INT, GL_UNSIGNED_INT. etc </param>
		/// <param name="typeSize"> : size of type</param>
		/// <param name="isNormalized"> : Specifies whether data should be normalized (Clamp signed value as -1 to 1, unsigned as 0 to 1 </param>
		void AddAttribute(GLuint count, GLuint type, GLsizei typeSize, GLuint isNormalized = false)
		{
			layout_.emplace_back(count, type, typeSize, isNormalized);
			stride_ += count * typeSize;
		}

		inline GLsizei GetStride() const { return stride_; }
		inline std::vector<VertexBufferElement> GetLayout() const { return layout_; }

	private:

		std::vector<VertexBufferElement> layout_;

		GLsizei stride_;
	};
}


#endif // !VERTEXATTRIBUTEMANAGER_H

