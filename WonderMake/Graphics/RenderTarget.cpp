#include "pch.h"
#include "RenderTarget.h"
#include "OpenGLFacade.h"


RenderTarget::RenderTarget(const SRenderTargetSettings& aSettings)
{
	SystemPtr<OpenGLFacade> openGL;

	myFrameBufferObject = openGL->GenerateFramebuffer();
	openGL->BindFramebuffer(GL_FRAMEBUFFER, myFrameBufferObject);

	myRenderTexture = openGL->GenerateTexture();
	openGL->BindTexture(GL_TEXTURE_2D, myRenderTexture);

	openGL->UploadTextureData(GL_TEXTURE_2D, 0, GL_RGB, aSettings.Size.X, aSettings.Size.Y, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	openGL->SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	openGL->SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	openGL->BindTexture(GL_TEXTURE_2D, 0);

	openGL->SetFramebufferTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myRenderTexture);

	if (aSettings.UseDepthStencilBuffer)
	{
		myDepthStencilRbo = openGL->GenerateRenderbuffer();
		openGL->BindRenderbuffer(myDepthStencilRbo);
		openGL->SetRenderbufferStorageParameters(GL_DEPTH24_STENCIL8, aSettings.Size.X, aSettings.Size.Y);
		openGL->BindRenderbuffer(0);

		openGL->AttachRenderbufferToFramebuffer(GL_DEPTH_STENCIL_ATTACHMENT, myDepthStencilRbo);
	}

	auto status = openGL->CheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		WmLog(TagWarning, "Frame buffer object is incomplete!");

	//reset framebuffer
	openGL->BindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::RenderTarget(RenderTarget&& aOther)
{
	myFrameBufferObject = aOther.myFrameBufferObject;
	myRenderTexture = aOther.myRenderTexture;
	myDepthStencilRbo = aOther.myDepthStencilRbo;

	aOther.myFrameBufferObject = std::numeric_limits<u32>::max();
	aOther.myRenderTexture     = std::numeric_limits<u32>::max();
	aOther.myDepthStencilRbo   = std::numeric_limits<u32>::max();
}

RenderTarget& RenderTarget::operator=(RenderTarget&& aOther)
{
	myFrameBufferObject = aOther.myFrameBufferObject;
	myRenderTexture = aOther.myRenderTexture;
	myDepthStencilRbo = aOther.myDepthStencilRbo;

	aOther.myFrameBufferObject = std::numeric_limits<u32>::max();
	aOther.myRenderTexture = std::numeric_limits<u32>::max();
	aOther.myDepthStencilRbo = std::numeric_limits<u32>::max();

	return *this;
}

RenderTarget::~RenderTarget()
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->DeleteFramebuffer(myFrameBufferObject);
	openGL->DeleteTexture(myRenderTexture);
	openGL->DeleteRenderbuffer(myDepthStencilRbo);
}

void RenderTarget::BindAsTarget()
{
	SystemPtr<OpenGLFacade> openGL;
	if (openGL->CheckNamedFramebufferStatus(GL_FRAMEBUFFER, myFrameBufferObject) != GL_FRAMEBUFFER_COMPLETE)
	{
		WmLog(TagWarning, "Could not bind RenderTarget as Frame buffer object is incomplete!");
		return;
	}
	openGL->BindFramebuffer(GL_FRAMEBUFFER, myFrameBufferObject);

}

void RenderTarget::BindAsTexture(const u32 aIndex /*= 0*/)
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->SetActiveTextureSlot(aIndex);

	openGL->BindTexture(GL_TEXTURE_2D, myRenderTexture);
}

