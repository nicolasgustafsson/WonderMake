#pragma once

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
class RenderSettingsManager : public System<>
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

