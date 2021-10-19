#include "ogpch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

OG::Texture::Texture(const std::string& fileName, GLuint texNum)
{
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::string path = "OG/images/" + fileName;

	unsigned char* buffer = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannels, 0);
	if (buffer)
	{
		std::cout << "Texture : " << path << "loaded" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(buffer);
	}
	else
	{
		std::cout << "Error on texture" << path << "loading" << std::endl;
	}

	texNum_ = texNum;
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

void OG::Texture::flip_vertically()
{
	stbi_set_flip_vertically_on_load(true);
}
