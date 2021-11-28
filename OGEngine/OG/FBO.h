#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>

namespace OG {
	class Texture;

	class FBO {

	public:
		FBO(int width, int height);
		~FBO();

		void bind() const;
		void unbind() const;
		void setTextureAttachment(const char* texture_name, std::unique_ptr<Texture>& texture, GLint level = 0);
		void setDepthBuffer();
		Texture* getAttachment(const char* texture_name);

	private:
		int m_width;
		int m_height;

		GLuint id_;

		GLuint m_depthID;

		// Attachment (Texture) Name, texture Handler
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_attachments;
	};
}


#endif // !FBO_H