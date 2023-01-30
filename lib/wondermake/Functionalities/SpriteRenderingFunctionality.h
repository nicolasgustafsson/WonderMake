#pragma once

#include "Components/SpriteComponent.h"
#include "Functionalities/TransformFunctionality.h"

#include "wondermake-base/Functionality.h"

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/Rotation.h"

template<typename TResource>
class ResourceProxy;
template<typename TResource>
class ResourceSystem;
class Texture;

class ConfigurationSystem;

class SpriteRenderingFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<ConfigurationSystem, PRead>,
			PAdd<AssetDatabase<Texture>, PWrite>,
			PAdd<ResourceSystem<Texture>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Vertex>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Fragment>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Geometry>>, PWrite>,
			PAdd<TransformFunctionality2D, PRead>,
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

	void Initialize(SInitializationInfo aInfo);

	void Tick();

	void SetTexture(std::string_view aAssetLink);
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
	void SetTexture(ResourceProxy<Texture>&& aTexture);

};
