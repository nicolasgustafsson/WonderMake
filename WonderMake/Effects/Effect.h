#pragma once
#include "EffectData.h"

class EffectAsset : public Resource
{

};

class Effect
{
public:
	virtual void Tick(const f32 aDeltaTime) {};
	virtual bool IsAlive() const = 0;
};

//class EffectInstance
//{
//	ResourceProxy<Effect> myEffect;
//	EffectData myEffectData;
//};
