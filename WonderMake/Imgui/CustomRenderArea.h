#pragma once

class CustomRenderArea : 
	public Debugged
{
protected:
	CustomRenderArea(std::string aName);
	virtual ~CustomRenderArea() {}

	virtual void Debug() override;

	//Called before creating the custom canvas for toolbar use and such
	virtual void PreCustomArea() noexcept {}
	virtual void Render() = 0;

	SVector2f myOffset = SVector2f(0.f, 0.f);
	SVector2f myScale = SVector2f(1.0f, 1.0f);
	SVector2f myScrolling;

	void DrawSquare(const SVector2f aPosition, const float aSize, const bool bScale, const SColor& aColor);
	void DrawLine(const SVector2f aFirst, const SVector2f aSecond, const SColor& aColor, const f32 aThickness = 1.0f);
	void DrawLines(const std::vector<SVector2f>& aPoints, const SColor& aColor, const f32 aThickness = 1.0f);

	constexpr SVector2f TransformPoint(SVector2f aPoint) const noexcept;

	SVector2f GetMouseOnCanvas() const noexcept;
	SVector4f GetCanvas() const noexcept;

	void SetCenter(const SVector2f aPosition) noexcept { myWantedPosition = aPosition; }

private:
	void Begin();
	void End();
	void HandleScrolling();
	void DrawContextMenu();
	void DrawGrid();

	std::optional<SVector2f> myWantedPosition;

	SVector2f myWindowSize; 
	std::string myName;
	bool myOpenContextMenu = false;
};
