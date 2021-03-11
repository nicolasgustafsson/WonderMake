#pragma once
#include "EffectBlackboard.h"

class EffectModule
{
public:
	virtual ~EffectModule() {};
	virtual void Tick() = 0;
};

class ScreenShakeModule : public EffectModule
{
public:
	virtual void Tick() override
	{
		WmLog("Shake shake shake");
	}
};

class EffectBlueprint : public Resource
{
public:
	EffectBlueprint() = default;

	EffectBlueprint(std::filesystem::path aPath)
	{

	}
	
	void Tick()
	{
		for (auto&& effectModule : myModules)
		{
			effectModule->Tick();
		}
	}

	template<typename TModuleType>
	void AddModule()
	{
		myModules.Add(std::move(std::make_unique<TModuleType>()));
	}
	SEffectBlackboard myBlackboard;
private:
	Container<std::unique_ptr<EffectModule>> myModules; //[Nicos]: How do we serialize this??
};

class EffectInstance final
{
public:
	EffectInstance(EffectBlueprint&& aBlueprint)
		: myEffect(std::move(aBlueprint)) {}

	void Tick(const f32 aDeltaTime) 
	{
		myEffect.Tick();
	};

	bool IsAlive() const { return true; };

	EffectBlueprint myEffect;
};

//class EffectInstance
//{
//	ResourceProxy<Effect> myEffect;
//	EffectData myEffectData;
//};
