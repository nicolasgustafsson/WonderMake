#pragma once
#include "Character/Buffs/Buff.h"


enum class EBuffIntensity
{
	VeryMild,		// ~120s
	Mild,			// ~30s
	Average,		// ~10s
	Intense,		// ~5s
	VeryIntense,	// ~1s
};

struct SBuffRequirements
{
	std::optional<EBuffType> Type;

	//[Nicos]: Arbitrary number, think 1-1000
	std::optional<f32> Strength;
	std::optional<f32> Intensity;
};

struct SBuffDesign
{
	EBuffType Type = EBuffType::Buff;
	f32 Strength = 10.f;
	f32 NumericIntensity = 1.f; //[Nicos]: converted from the buff intensity enum
	f32 Duration = 1.f;
	plf::colony<std::unique_ptr<BuffBlueprintProperty>> Properties;
};

class BuffDesigner : public System, public Debugged
{
public:
	BuffDesigner();
	BuffBlueprint& DesignBuff(SBuffRequirements aBuffRequirements = {});

private:

	EBuffType DecideBuffType() const;
	f32 DecideBuffStrength() const;
	f32 DecideBuffIntensity() const;

	void MakeBuffBetter(const f32 aHowMuch, SBuffDesign& aBuffDesign) const;
	void MakeBuffWorse(const f32 aHowMuch, SBuffDesign& aBuffDesign) const;
	
	void AddStatProperty(bool aIncrease, const f32 aStrength, SBuffDesign& aBuffDesign) const;

	void AddEffectOverTimeProperty(const f32 aEffectStrength, SBuffDesign& aBuffDesign) const;

	//[Nicos]: let the designer have ownership of the blueprints for now so we don't get a bunch of lifetime problems
	BuffBlueprint ConstructBlueprintFromDesign(SBuffDesign& aBuffDesign) const;
protected:
	virtual void Debug() override;

	plf::colony<BuffBlueprint> myBlueprints;
};

