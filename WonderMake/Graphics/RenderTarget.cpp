#include "stdafx.h"
#include "RenderTarget.h"


RenderTarget::RenderTarget(const SRenderTargetSettings& aSettings)
{
	glGenFramebuffers(1, &myFrameBufferObject);

	glBindFramebuffer(GL_FRAMEBUFFER, myFrameBufferObject);

	glGenTextures(1, &myRenderTexture);
	glBindTexture(GL_TEXTURE_2D, myRenderTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, aSettings.Size.X, aSettings.Size.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myRenderTexture, 0);

	if (aSettings.UseDepthStencilBuffer)
	{
		glGenRenderbuffers(1, &myDepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, myDepthStencilRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, aSettings.Size.X, aSettings.Size.Y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, myDepthStencilRBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		WmLog(TagWarning, "Frame buffer object is incomplete!");

	//reset framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffers(1, &myFrameBufferObject);
	glDeleteTextures(1, &myRenderTexture);
	glDeleteRenderbuffers(1, &myDepthStencilRBO);
}

void RenderTarget::BindAsTarget()
{
	if (glCheckNamedFramebufferStatus(myFrameBufferObject, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		WmLog(TagWarning, "Could not bind RenderTarget as Frame buffer object is incomplete!");
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, myFrameBufferObject);
}

void RenderTarget::BindAsTexture()
{
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, myRenderTexture);
}

