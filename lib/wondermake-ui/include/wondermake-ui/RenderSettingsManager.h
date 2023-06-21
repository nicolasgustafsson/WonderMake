#pragma once

#include "wondermake-base/System.h"

#include <optional>
#include <vector>

class OpenGLFacade;

enum class EBlendMode
{
	Alpha,
	Additive
};

enum class EDepthMode
{
	Greater,
	GreaterEqual,
	Equal,
	NotEqual,
	LessEqual,
	Less,
	Never,
	Always
};

struct SRenderSettings final
{
	bool operator==(const SRenderSettings& aOther)const;
	bool operator!=(const SRenderSettings& aOther)const;
	std::optional<EBlendMode> BlendMode;
	std::optional<EDepthMode> DepthMode;

	void Append(const SRenderSettings& aOther);

	[[nodiscard]] SRenderSettings GetDiff(const SRenderSettings& aOther) const;

	bool IsEmpty() const;
};

class RenderSettingsManager
	: public System<
		Policy::Set<
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	RenderSettingsManager();
	void PushSettings(const SRenderSettings& aSettings);
	void PopSettings();

private:
	void SetBlendMode(const EBlendMode aBlendMode);
	void SetDepthMode(const EDepthMode aDepthMode);
	void ApplyRenderSettings(const SRenderSettings& aRenderSettings);
	void BuildRenderSettings();

	SRenderSettings myDefaultSettings;
	std::vector<SRenderSettings> myRenderSettingsStack;

	SRenderSettings myPreviousSettings;

};

