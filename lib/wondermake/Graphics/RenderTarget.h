#pragma once
#include "wondermake-utility/Typedefs.h"

struct SRenderTargetSettings
{
	SVector2u Size;

	bool UseDepthStencilBuffer = false;

	constexpr bool operator==(const SRenderTargetSettings&) const noexcept = default;
	constexpr bool operator!=(const SRenderTargetSettings&) const noexcept = default;
};

class RenderTarget : NonCopyable
{
public:
	RenderTarget(const SRenderTargetSettings& Settings);
	RenderTarget(RenderTarget&&) noexcept;
	~RenderTarget();

	RenderTarget& operator=(RenderTarget&&) noexcept;

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

