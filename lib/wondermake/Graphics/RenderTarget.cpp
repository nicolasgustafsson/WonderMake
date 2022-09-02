#include "pch.h"
#include "RenderTarget.h"
#include "OpenGLFacade.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

std::string ToString(const SRenderTargetSettings& aSettings)
{
	return "{ \"Size\": { \"X\": " + std::to_string(aSettings.Size.X) + ", \"Y\": " + std::to_string(aSettings.Size.Y) + " }, \"UseDepthStencilBuffer\": " + std::to_string(aSettings.UseDepthStencilBuffer) + " }";
}

RenderTarget::RenderTarget(const SRenderTargetSettings& aSettings)
{
	Initialize(aSettings);
}

RenderTarget::RenderTarget(RenderTarget&& aOther) noexcept
{
	mySettings			= aOther.mySettings;
	myFrameBufferObject	= aOther.myFrameBufferObject;
	myRenderTexture		= aOther.myRenderTexture;
	myDepthStencilRbo	= aOther.myDepthStencilRbo;

	aOther.mySettings			= SRenderTargetSettings();
	aOther.myFrameBufferObject	= std::numeric_limits<u32>::max();
	aOther.myRenderTexture		= std::numeric_limits<u32>::max();
	aOther.myDepthStencilRbo	= std::numeric_limits<u32>::max();
}

RenderTarget::~RenderTarget()
{
	Destroy();
}

RenderTarget& RenderTarget::operator=(RenderTarget&& aOther) noexcept
{
	Destroy();
	
	mySettings			= aOther.mySettings;
	myFrameBufferObject	= aOther.myFrameBufferObject;
	myRenderTexture		= aOther.myRenderTexture;
	myDepthStencilRbo	= aOther.myDepthStencilRbo;

	aOther.mySettings			= SRenderTargetSettings();
	aOther.myFrameBufferObject	= std::numeric_limits<u32>::max();
	aOther.myRenderTexture		= std::numeric_limits<u32>::max();
	aOther.myDepthStencilRbo	= std::numeric_limits<u32>::max();

	return *this;
}

void RenderTarget::BindAsTarget()
{
	if (myFrameBufferObject == std::numeric_limits<u32>::max())
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Tried to bind uninitialized RenderTarget.");

		return;
	}

	SystemPtr<OpenGLFacade> openGL;

	const auto status = openGL->CheckNamedFramebufferStatus(GL_FRAMEBUFFER, myFrameBufferObject);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		WmLogWarning(TagWonderMake << TagWmOpenGL << "Could not bind RenderTarget as Frame buffer object is incomplete, status: " << status << '.');

		return;
	}

	openGL->BindFramebuffer(GL_FRAMEBUFFER, myFrameBufferObject);
	openGL->SetViewportSize(SVector2i(static_cast<i32>(mySettings.Size.X), static_cast<i32>(mySettings.Size.Y)));
}

void RenderTarget::BindAsTexture(const u32 aIndex /*= 0*/)
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->SetActiveTextureSlot(aIndex);

	openGL->BindTexture(GL_TEXTURE_2D, myRenderTexture);
}

void RenderTarget::Reinitialize(const SRenderTargetSettings& aSettings)
{
	Destroy();
	Initialize(aSettings);
}

void RenderTarget::Initialize(const SRenderTargetSettings& aSettings)
{
	if (aSettings.Size == SVector2u::Zero())
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Tried to create RenderTarget with zero size, settings: " << ToString(aSettings) << '.');

		return;
	}

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
		WmLogWarning(TagWonderMake << TagWmOpenGL << "Frame buffer object is incomplete, status: " << status << ", settings: " << ToString(aSettings) << '.');

	//reset framebuffer
	openGL->BindFramebuffer(GL_FRAMEBUFFER, 0);
	mySettings = aSettings;
}

void RenderTarget::Destroy()
{
	SystemPtr<OpenGLFacade> openGL;
	
	if(myFrameBufferObject != std::numeric_limits<u32>::max())
		openGL->DeleteFramebuffer(myFrameBufferObject);
	if (myRenderTexture != std::numeric_limits<u32>::max())
		openGL->DeleteTexture(myRenderTexture);
	if (myDepthStencilRbo != std::numeric_limits<u32>::max())
		openGL->DeleteRenderbuffer(myDepthStencilRbo);

	mySettings			= SRenderTargetSettings();
	myFrameBufferObject	= std::numeric_limits<u32>::max();
	myRenderTexture		= std::numeric_limits<u32>::max();
	myDepthStencilRbo	= std::numeric_limits<u32>::max();
}

