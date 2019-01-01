#pragma once
#include "Typedefs.h"

struct SRenderTargetSettings
{
	SVector2u Size;

	bool UseDepthStencilBuffer = false;
};

class RenderTarget : NonCopyable
{
public:
	RenderTarget(const SRenderTargetSettings& Settings);
	~RenderTarget();

	void BindAsTarget();
	void BindAsTexture();

	u32 myFrameBufferObject;
	u32 myRenderTexture;
	u32 myDepthStencilRBO;
};

