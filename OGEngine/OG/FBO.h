#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>

#include <Texture.h>

namespace OG {
	class FBO {

	public:
		FBO(int width, int height);
		~FBO();

		void bind() const;
		void unbind() const;
		void setTextureAttachment(GLuint numSlot, Texture* texture, GLint level = 0);
		void setDepthBuffer();

	private:
		int m_width;
		int m_height;

		GLuint id_;

		GLuint m_depthID;

		// Attachment slot, texture Handler
		std::unordered_map<GLenum, Texture*> m_attachments;
	};
}


#endif // !FBO_H
