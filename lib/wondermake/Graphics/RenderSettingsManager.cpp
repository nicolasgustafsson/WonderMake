#include "pch.h"
#include "RenderSettingsManager.h"

REGISTER_SYSTEM(RenderSettingsManager);

void SRenderSettings::Append(const SRenderSettings& aOther)
{
	if (aOther.BlendMode)
		BlendMode = aOther.BlendMode;

	if (aOther.DepthMode)
		DepthMode = aOther.DepthMode;
}

SRenderSettings SRenderSettings::GetDiff(const SRenderSettings& aOther) const
{
	SRenderSettings diff;

	if (aOther.BlendMode != BlendMode)
		diff.BlendMode = BlendMode;

	if (aOther.DepthMode != DepthMode)
		diff.DepthMode = DepthMode;

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

	if (DepthMode.has_value())
		return false;

	return true;
}

bool SRenderSettings::operator!=(const SRenderSettings& aOther) const
{
	return !(*this == aOther);
}

RenderSettingsManager::RenderSettingsManager()
{
	myDefaultSettings.BlendMode = EBlendMode::Alpha;
	myDefaultSettings.DepthMode = EDepthMode::GreaterEqual;
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
	case EBlendMode::Alpha:
		Get<OpenGLFacade>().SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case EBlendMode::Additive:
		Get<OpenGLFacade>().SetBlendFunction(GL_SRC_ALPHA, GL_ONE);
		break;
	}
}

void RenderSettingsManager::SetDepthMode(const EDepthMode aDepthMode)
{
	switch (aDepthMode)
	{
	case EDepthMode::Greater:
		Get<OpenGLFacade>().SetDepthFunction(GL_GREATER);
		break;
	case EDepthMode::GreaterEqual:
		Get<OpenGLFacade>().SetDepthFunction(GL_GEQUAL);
		break;
	case EDepthMode::Equal:
		Get<OpenGLFacade>().SetDepthFunction(GL_EQUAL);
		break;
	case EDepthMode::NotEqual:
		Get<OpenGLFacade>().SetDepthFunction(GL_NOTEQUAL);
		break;
	case EDepthMode::LessEqual:
		Get<OpenGLFacade>().SetDepthFunction(GL_LEQUAL);
		break;
	case EDepthMode::Less:
		Get<OpenGLFacade>().SetDepthFunction(GL_LESS);
		break;
	case EDepthMode::Never:
		Get<OpenGLFacade>().SetDepthFunction(GL_NEVER);
		break;
	case EDepthMode::Always:
		Get<OpenGLFacade>().SetDepthFunction(GL_ALWAYS);
		break;
	}
}

void RenderSettingsManager::ApplyRenderSettings(const SRenderSettings& aRenderSettings)
{
	if (aRenderSettings.BlendMode)
		SetBlendMode(*aRenderSettings.BlendMode);

	if (aRenderSettings.DepthMode)
		SetDepthMode(*aRenderSettings.DepthMode);
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

