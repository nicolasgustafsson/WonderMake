#pragma once
#include "../Functionality.h"
#include "../Components/SpriteComponent.h"
#include "../ComponentDependency.h"
#include "../Components/TransformComponent.h"
#include "../FunctionalityDependency.h"
class SpriteRenderingFunctionality
	: public Functionality
{
public:
	SpriteRenderingFunctionality(Object* aOwner);
	~SpriteRenderingFunctionality();

	virtual void Tick() override;
	void SetTexture(const std::filesystem::path& aTexturePath);
	void SetScale(const SVector2f aScale);

	ComponentDependency<STransformComponent> myTransformComponent;
	ComponentDependency<SSpriteComponent> mySpriteComponent;
};

