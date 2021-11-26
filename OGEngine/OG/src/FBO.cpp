#include "ogpch.h"
#include "FBO.h"

namespace OG {
	FBO::FBO(int width, int height) : m_width(width), m_height(height)
	{
		glGenFramebuffers(1, &id_);

		glBindFramebuffer(GL_FRAMEBUFFER, id_);
	}

	FBO::~FBO()
	{
		// Deallocate Texture Mem
		for (auto& itr : m_attachments) {
			delete itr.second;
			itr.second = nullptr;
		}
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

	void FBO::setTextureAttachment(GLuint numSlot, Texture* texture, GLint level)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + numSlot, GL_TEXTURE_2D, texture->getHandler(), level);

		m_attachments.emplace(numSlot, texture);
	}

	void FBO::setDepthBuffer()
	{
		glGenRenderbuffers(1, &m_depthID);

		glBindRenderbuffer(GL_RENDERBUFFER, m_depthID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthID);
	}

}
