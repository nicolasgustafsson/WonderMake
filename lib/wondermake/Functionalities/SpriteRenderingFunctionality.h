#pragma once

#include "Components/SpriteComponent.h"
#include "Functionalities/TransformFunctionality.h"

#include "wondermake-base/Functionality.h"

#include "wondermake-utility/FilePath.h"

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
			PAdd<TransformFunctionality2D, PRead>,
			PAdd<SSpriteComponent, PWrite>>,
		SystemTraits::Set<
			STGui>>
{
public:
	void Tick();

	void SetTexture(std::string_view aAssetLink);
	void SetTexture(const FilePath& aFilePath);

	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void SetColor(const SColor aColor);

	void Hide() noexcept;
	void Show() noexcept;

private:
	void SetTexture(ResourceProxy<Texture>&& aTexture);

};
