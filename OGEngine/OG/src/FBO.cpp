#include "ogpch.h"
#include "FBO.h"
#include "Texture.h"

namespace OG {
	FBO::FBO() : m_width(0), m_height(0)
	{
	}

	FBO::~FBO()
	{
		glDeleteRenderbuffers(1, &m_depthID);
		glDeleteFramebuffers(1, &id_);
	}

	void FBO::init(int width, int height)
	{
		m_width = width;
		m_height = height;

		glGenFramebuffers(1, &id_);

		glBindFramebuffer(GL_FRAMEBUFFER, id_);
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
		if (m_attachments.find(texture_name) != m_attachments.end())
			m_attachments.clear();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLint)m_attachments.size(), GL_TEXTURE_2D, texture->getHandler(), level);

		m_attachments[texture_name] = std::move(texture);
	}

	void FBO::bind_attachments()
	{
		for (const auto& pair : m_attachments) {
			pair.second->Bind();
		}
	}

	/*
	* OpenGL by default only renders to a framebuffer's first color attachment.
	* Must explicitly tell OpenGL rendering multiple colorbuffers
	*/
	void FBO::setDrawBuffers()
	{
		std::vector<GLuint> texIds(m_attachments.size());

		for (const auto& pair : m_attachments) {
			texIds.push_back(pair.second->getHandler());
		}

		glDrawBuffers((GLsizei)texIds.size(), texIds.data());
	}

	void FBO::setDepthBuffer()
	{
		glGenRenderbuffers(1, &m_depthID);

		glBindRenderbuffer(GL_RENDERBUFFER, m_depthID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthID);
	}

	void FBO::cleanUp() const
	{
		glDeleteFramebuffers(1, &id_);
	}

	Texture* FBO::getAttachment(const char* texture_name)
	{
		if (m_attachments.find(texture_name) != m_attachments.end())
			return m_attachments[texture_name].get();

		return nullptr;
	}

}
