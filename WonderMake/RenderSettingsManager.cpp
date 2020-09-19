#include "pch.h"
#include "RenderSettingsManager.h"

void SRenderSettings::Append(const SRenderSettings& aOther)
{
	if (aOther.BlendMode)
		BlendMode = aOther.BlendMode;
}

SRenderSettings SRenderSettings::GetDiff(const SRenderSettings& aOther) const
{
	SRenderSettings diff;

	if (aOther.BlendMode != BlendMode)
		diff.BlendMode = BlendMode;

	return diff;
}

bool SRenderSettings::operator==(const SRenderSettings& aOther) const
{
	return GetDiff(aOther).IsEmpty();
}

bool SRenderSettings::IsEmpty() const
{
	if (BlendMode.has_value())
		return false;

	return true;
}

bool SRenderSettings::operator!=(const SRenderSettings& aOther) const
{
	return !(*this == aOther);
}

RenderSettingsManager::RenderSettingsManager()
{
	myDefaultSettings.BlendMode = EBlendMode::Multiplicative;
}

void RenderSettingsManager::PushSettings(const SRenderSettings& aSettings)
{
	myRenderSettingsStack.push_back(aSettings);

	BuildRenderSettings();
}

void RenderSettingsManager::SetBlendMode(const EBlendMode aBlendMode)
{
	switch(aBlendMode)
	{
	case EBlendMode::Multiplicative:
		SystemPtr<OpenGLFacade>()->SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case EBlendMode::Additive:
		SystemPtr<OpenGLFacade>()->SetBlendFunction(GL_SRC_ALPHA, GL_ONE);
		break;
	default:
		break;
	}
}

void RenderSettingsManager::ApplyRenderSettings(const SRenderSettings& aRenderSettings)
{
	if (aRenderSettings.BlendMode)
		SetBlendMode(*aRenderSettings.BlendMode);
}

void RenderSettingsManager::BuildRenderSettings()
{
	SRenderSettings finalSettings = myDefaultSettings;

	for (auto&& renderSettings : myRenderSettingsStack)
	{
		finalSettings.Append(renderSettings);
	}

	ApplyRenderSettings(finalSettings.GetDiff(myPreviousSettings));

	myPreviousSettings = finalSettings;
}

void RenderSettingsManager::PopSettings()
{
	myRenderSettingsStack.pop_back();
	BuildRenderSettings();
}

