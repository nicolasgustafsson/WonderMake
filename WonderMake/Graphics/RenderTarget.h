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
	RenderTarget(const SRenderTargetSettings& Settings) noexcept;
	~RenderTarget();

	void BindAsTarget();
	void BindAsTexture();

	inline u32 GetTexture() const noexcept { return myRenderTexture; }

private:
	u32 myFrameBufferObject = std::numeric_limits<u32>::max();
	u32 myRenderTexture = std::numeric_limits<u32>::max();
	u32 myDepthStencilRbo = std::numeric_limits<u32>::max();
};

