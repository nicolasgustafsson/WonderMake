#pragma once
#include "EffectBlackboard.h"

class EffectAsset : public Resource
{

};

class Effect final
{
public:
	void Tick(const f32 aDeltaTime) {};
	bool IsAlive() const { return true; };

	ResourceProxy<EffectAsset> myEffect;
};

//class EffectInstance
//{
//	ResourceProxy<Effect> myEffect;
//	EffectData myEffectData;
//};
