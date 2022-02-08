#pragma once
#include "wondermake-utility/Typedefs.h"

struct SRenderTargetSettings
{
	SVector2u Size;

	bool UseDepthStencilBuffer = false;

	bool operator==(const SRenderTargetSettings& aOther) const { return Size == aOther.Size && UseDepthStencilBuffer == aOther.UseDepthStencilBuffer; }

	bool operator!=(const SRenderTargetSettings& aOther) const { return !(*this == aOther); }
};

class RenderTarget : NonCopyable
{
public:
	RenderTarget(const SRenderTargetSettings& Settings);

	RenderTarget(RenderTarget&&);
	
	RenderTarget& operator=(RenderTarget&&);

	~RenderTarget();

	void BindAsTarget();
	void BindAsTexture(const u32 aIndex = 0);

	inline u32 GetTexture() const noexcept { return myRenderTexture; }

	const SRenderTargetSettings& GetSettings() const { return mySettings; }

	void Reinitialize(const SRenderTargetSettings& aSettings);
private:
	void Initialize(const SRenderTargetSettings& aSettings);
	void Destroy();

	SRenderTargetSettings mySettings;
	u32 myFrameBufferObject = std::numeric_limits<u32>::max();
	u32 myRenderTexture = std::numeric_limits<u32>::max();
	u32 myDepthStencilRbo = std::numeric_limits<u32>::max();
};

