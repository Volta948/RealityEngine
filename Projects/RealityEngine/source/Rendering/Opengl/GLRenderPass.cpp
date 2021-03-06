// Copyright Reality Engine. All Rights Reserved.

#include "Rendering/Opengl/GLRenderPass.h"

#include <glad/glad.h>

#include "Rendering/Opengl/GLShaderHelper.h"
#include "Rendering/Opengl/GLMeshHelper.h"
#include "Rendering/Opengl/GLCubeMap.h"
#include "Core/Tools/Logger.h"

Reality::GLRenderPass::GLRenderPass(GLRenderPassSettings settings) {
	glGenFramebuffers(1, &m_Handle.Id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle.Id);

	const auto hasColorAttachment{ bool(settings.FrameBufferTexture & GLRenderPassFramebufferTexture::ColorAttachment) };
	const auto hasDepthAttachment{ bool(settings.FrameBufferTexture & GLRenderPassFramebufferTexture::DepthAttachment) };
	const auto hasStencilAttachment{ bool(settings.FrameBufferTexture & GLRenderPassFramebufferTexture::StencilAttachment) };

	if (hasColorAttachment) {
		std::vector<GLuint> colorIndices;
		for (GLuint i{}; i < settings.ColorAttachments.size(); ++i) {
			const auto& color{ m_Handle.ColorAttachments.emplace_back(settings.ColorAttachments[i]) };
			colorIndices.emplace_back(GL_COLOR_ATTACHMENT0 + i);
			glFramebufferTexture2D(GL_FRAMEBUFFER, colorIndices[i], color.GetHandle().Target, color.GetHandle().Id, 0);
		}
		glDrawBuffers((GLsizei)colorIndices.size(), colorIndices.data());
	}

	if (hasDepthAttachment || hasStencilAttachment) {
		m_Handle.DepthStencilAttachment = GLTexture{ settings.DepthStencilAttachment };
		glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(hasDepthAttachment && hasStencilAttachment ?
			GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT), m_Handle.DepthStencilAttachment.GetHandle().Target,
			m_Handle.DepthStencilAttachment.GetHandle().Id, 0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		RE_LOG_ERROR("FrameBuffer is not complete");
	}
}

Reality::GLRenderPass::~GLRenderPass() {
	glDeleteFramebuffers(1, &m_Handle.Id);
}

void Reality::GLRenderPass::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle.Id);
}

void Reality::GLRenderPass::Clear() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle.Id);

	constexpr GLenum modes[]{ GL_FILL, GL_LINE, GL_POINT };
	glPolygonMode(GL_FRONT_AND_BACK, modes[(int)Rasterization]);
	glViewport((GLsizei)Viewport.Pos.X, (GLsizei)Viewport.Pos.Y, (GLsizei)Viewport.Size.X, (GLsizei)Viewport.Size.Y);
	glLineWidth(1.f);
	glPointSize(1.f);

	if (Capacity & GLRenderPassCapacity::ClearColor) {
		glClearColor(ClearColor.X, ClearColor.Y, ClearColor.Z, ClearColor.W);
	}

	glClear(GLenum(ClearBuffer & GLRenderPassClearBuffer::ColorBuffer ? GL_COLOR_BUFFER_BIT : GL_NONE) |
		GLenum(ClearBuffer & GLRenderPassClearBuffer::DepthBuffer ? GL_DEPTH_BUFFER_BIT : GL_NONE) |
		GLenum(ClearBuffer & GLRenderPassClearBuffer::StencilBuffer ? GL_STENCIL_BUFFER_BIT : GL_NONE));

	(Capacity & GLRenderPassCapacity::Stencil ? glEnable : glDisable)(GL_STENCIL_TEST);
	(Capacity & GLRenderPassCapacity::Depth ? glEnable : glDisable)(GL_DEPTH_TEST);
	(Capacity & GLRenderPassCapacity::Culling ? glEnable : glDisable)(GL_CULL_FACE);
}

void Reality::GLRenderPass::Blit(const GLRenderPass& dest) const {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Handle.Id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.m_Handle.Id);

	glViewport((GLsizei)dest.Viewport.Pos.X, (GLsizei)dest.Viewport.Pos.Y, (GLsizei)dest.Viewport.Size.X, (GLsizei)dest.Viewport.Size.Y);

	// This check is for empty framebuffers like the default framebuffer who doesn't have any color attachment binded
	if (dest.m_Handle.ColorAttachments.empty()) {
		glBlitFramebuffer((GLint)Viewport.Pos.X, (GLint)Viewport.Pos.Y, (GLint)Viewport.Size.X, (GLint)Viewport.Size.Y,
			(GLint)dest.Viewport.Pos.X, (GLint)dest.Viewport.Pos.Y, (GLint)dest.Viewport.Size.X, (GLint)dest.Viewport.Size.Y,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	else {
		for (std::size_t i{}; i < dest.m_Handle.ColorAttachments.size(); ++i) {
			glReadBuffer(GL_COLOR_ATTACHMENT0 + (GLenum)i);
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + (GLenum)i);
			glBlitFramebuffer((GLint)Viewport.Pos.X, (GLint)Viewport.Pos.Y, (GLint)Viewport.Size.X, (GLint)Viewport.Size.Y,
				(GLint)dest.Viewport.Pos.X, (GLint)dest.Viewport.Pos.Y, (GLint)dest.Viewport.Size.X, (GLint)dest.Viewport.Size.Y,
				GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
	}
}

void Reality::GLRenderPass::Resize(const struct Viewport& viewport) {
	Viewport = viewport;
	for (const auto& texture : m_Handle.ColorAttachments) {
		texture.Resize(viewport);
	}

	if (m_Handle.DepthStencilAttachment.GetHandle().Id) {
		m_Handle.DepthStencilAttachment.Resize(viewport);
	}
}