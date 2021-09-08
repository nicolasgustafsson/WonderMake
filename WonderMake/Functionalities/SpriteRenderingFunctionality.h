#pragma once
#include "Functionality.h"

#include "Components/SpriteComponent.h"
#include "Functionalities/TransformFunctionality.h"

class SpriteRenderingFunctionality
	: public Functionality<TransformFunctionality2D, SSpriteComponent>
{
public:
	void Tick();
	void SetTexture(const std::string_view aAssetLink);
	void SetFragmentShader(const std::filesystem::path& aAssetPath);
	void SetScale(const SVector2f aScale);
	void SetScale(const f32 aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void SetColor(const SColor aColor);
	void SetRenderOrder(const i32 aOrder);

	SVector2f GetScale() const;

	template<typename TProperty>
	void SetProperty(std::string_view aName, TProperty aProperty);

	void Hide() noexcept;
	void Show() noexcept;
	
};

template <typename TProperty>
void SpriteRenderingFunctionality::SetProperty(std::string_view aName, TProperty aProperty)
{
	auto& spriteComponent = Get<SSpriteComponent>();
	spriteComponent.RenderObject->SetProperty(aName, aProperty);
}
