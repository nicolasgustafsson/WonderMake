#pragma once

enum class EBlendMode
{
	Multiplicative,
	Additive
};

struct SRenderSettings final
{
	std::optional<EBlendMode> BlendMode;

	void Append(const SRenderSettings& aOther);

	[[nodiscard]] SRenderSettings GetDiff(const SRenderSettings& aOther) const;
};
class RenderSettingsManager
{
public:
	friend class SRenderSettingHandle;

	void PushSettings(const SRenderSettings& aSettings);
	void PopSettings();

private:
	void SetBlendMode(const EBlendMode aBlendMode);
	void ApplyRenderSettings(const SRenderSettings& aRenderSettings);
	void BuildRenderSettings();

	SRenderSettings myDefaultSettings;
	std::vector<SRenderSettings> myRenderSettingsStack;

	SRenderSettings myPreviousSettings;

};

