#pragma once

#include "Functionalities/TransformFunctionality.h"

#include "wondermake-io/FileResourceProxy.h"

#include "wondermake-base/Functionality.h"

#include "wondermake-utility/Color.h"
#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/Rotation.h"

struct SSpriteComponent;

class ShaderResourceSystem;
class TextureResource;
class TextureResourceSystem;

class ConfigurationSystem;

class SpriteRenderingFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<ConfigurationSystem, PRead>,
			PAdd<ShaderResourceSystem, PWrite>,
			PAdd<TransformFunctionality2D, PRead>,
			PAdd<TextureResourceSystem, PWrite>,
			PAdd<SSpriteComponent, PWrite>>,
		SystemTraits::Set<
			STGui,
			STWonderMake>>
{
public:
	struct SInitializationInfo
	{
		FilePath			FilePath;
		SVector2f			Scale		= SVector2f::One();
		SVector2f			Origin;
		SColor				Color;
		std::string_view	RenderLayer;
		i32					RenderOrder	= 0;
	};

	SpriteRenderingFunctionality();

	void Initialize(SInitializationInfo aInfo);

	void Tick();

	void SetTexture(const FilePath& aFilePath);

	[[nodiscard]] SVector2u GetTextureSize() const noexcept;

	void SetScale(const SVector2f aScale);
	void SetOrigin(const SVector2f aOrigin);
	void SetColor(const SColor aColor);
	void SetRenderLayer(std::string_view aRenderLayer);
	void SetRenderOrder(i32 aRenderOrder);

	void Hide() noexcept;
	void Show() noexcept;

private:
	void SetTexture(FileResourceRef<TextureResource>&& aTexture);

};
