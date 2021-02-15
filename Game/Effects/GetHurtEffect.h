#pragma once
#include "Effects/Effect.h"

class GetHurtEffect
	: public Effect
{
public:
	virtual void Tick(const f32 aDeltaTime) override;

	bool IsAlive() const override;

private:
	f32 myTimeToLive = 1.0f;
};
