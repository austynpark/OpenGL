#include "ogpch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

OG::Texture::Texture(const char* fileName, GLuint texNum)
{
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	const char* prefix = "OG/images/";

	std::string path = prefix + std::string(fileName);


	unsigned char* buffer = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannels, 0);
	if (buffer)
	{
		GLenum format;

		if (m_nrChannels == 3)
			format = GL_RGB;
		if (m_nrChannels == 4)
			format = GL_RGBA;


		std::cout << "Texture : " << path << "loaded" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, buffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(buffer);
	}
	else
	{
		std::cout << "Error on texture" << path << "loading" << std::endl;
	}

	texNum_ = texNum;
}

OG::Texture::Texture(int width, int height, GLuint texNum)
{
	m_nrChannels = 3;
	texNum_ = texNum;

	m_width = width;
	m_height = height;

	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}

OG::Texture::~Texture()
{
	glDeleteTextures(1, &id_);
}

void OG::Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + texNum_);
	glBindTexture(GL_TEXTURE_2D, id_);
}

void OG::Texture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint OG::Texture::getHandler() const
{
	return id_;
}

void OG::Texture::flip_vertically()
{
	stbi_set_flip_vertically_on_load(true);
}
