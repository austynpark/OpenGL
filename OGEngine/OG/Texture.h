#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>

namespace OG
{
	class Texture
	{ 
	public:
		Texture() = default;
		Texture(Texture&&) = default;
		Texture(Texture&) = default;
		Texture(const char* fileName, GLuint texNum);

		Texture(int width, int height, GLuint texNum);
		~Texture();

		Texture& operator=(const Texture&) = default;
		Texture& operator=(Texture&&) = default;

		void Bind() const;
		void UnBind() const;
		GLuint getHandler() const;

		static void flip_vertically();

		GLuint texNum_;
	protected:
		GLuint id_;

		int m_width;
		int m_height;
		int m_nrChannels;
	};

	class SkyboxTexture : public Texture
	{
	public:
		SkyboxTexture(const char* fileName, GLuint texNum);
		~SkyboxTexture() = default;
	};

	class GBufferTexture : public Texture
	{
	public:
		GBufferTexture(int width, int height, GLenum texture_format, GLuint texNum);
		~GBufferTexture() = default;
		
	};

}
#endif // TEXTURE_H
