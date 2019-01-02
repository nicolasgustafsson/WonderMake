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

	inline u32 GetTexture() const { return myRenderTexture; }

private:
	u32 myFrameBufferObject = std::numeric_limits<u32>::max();
	u32 myRenderTexture = std::numeric_limits<u32>::max();
	u32 myDepthStencilRBO = std::numeric_limits<u32>::max();
};

