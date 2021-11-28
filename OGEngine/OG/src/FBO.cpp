#include "ogpch.h"
#include "FBO.h"
#include "Texture.h"

namespace OG {
	FBO::FBO(int width, int height) : m_width(width), m_height(height)
	{
		glGenFramebuffers(1, &id_);

		glBindFramebuffer(GL_FRAMEBUFFER, id_);
	}

	FBO::~FBO()
	{
		glDeleteFramebuffers(1, &id_);
	}

	void FBO::bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id_);
	}

	void FBO::unbind() const
	{
		// Set to Default Frame Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FBO::setTextureAttachment(const char* texture_name, std::unique_ptr<Texture>& texture, GLint level)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLint)m_attachments.size(), GL_TEXTURE_2D, texture->getHandler(), level);

		m_attachments.emplace(texture_name, std::move(texture));
	}

	void FBO::setDepthBuffer()
	{
		glGenRenderbuffers(1, &m_depthID);

		glBindRenderbuffer(GL_RENDERBUFFER, m_depthID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthID);
	}

	Texture* FBO::getAttachment(const char* texture_name)
	{
		if (m_attachments.find(texture_name) != m_attachments.end())
			return m_attachments[texture_name].get();

		return nullptr;
	}

}
