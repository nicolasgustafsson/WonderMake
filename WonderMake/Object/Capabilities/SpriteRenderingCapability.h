#pragma once
#include "../Capability.h"
#include "../Components/SpriteComponent.h"
#include "../ComponentDependency.h"
#include "../Components/TransformComponent.h"
#include "../CapabilityDependency.h"
class SpriteRenderingCapability
	: public Capability
{
public:
	SpriteRenderingCapability(Object* aOwner);
	~SpriteRenderingCapability();

	virtual void Tick() override;
	void SetTexture(const std::filesystem::path& aTexturePath);
	void SetScale(const SVector2f aScale);

	ComponentDependency<STransformComponent> myTransformComponent;
	ComponentDependency<SSpriteComponent> mySpriteComponent;
};

