#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>

namespace OG
{
	class Texture
	{ 
	public:
		Texture(const char* fileName, GLuint texNum);

		Texture(int width, int height, GLuint texNum);
		~Texture();

		void Bind() const;
		void UnBind() const;
		GLuint getHandler() const;

		static void flip_vertically();

		GLuint texNum_;
	private:
		GLuint id_;

		int m_width;
		int m_height;
		int m_nrChannels;
	};
}
#endif // TEXTURE_H
