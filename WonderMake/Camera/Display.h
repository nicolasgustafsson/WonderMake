#pragma once
#include "Graphics/RenderNodeGraph/RenderNodeGraph.h"
#include "Graphics/EngineUniformBuffer.h"

class RenderTarget;
class Camera;


//[Nicos]: A Camera has multiple displays; these allow you to show the contents of a camera in different ways(such as debug views, or with a different Post Process setup etc)
class Display
{
public:
	Display(const std::string& aName, Camera& aCamera);

	void FinishDebugFrame();
	void FinishFrame(); 
	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept;

	[[nodiscard]] bool HasFocus() const { return myHasFocus; }

	void Focus();
	
	void Inspect();
	void SetViewportSize(const SVector2i aViewportSize) noexcept;
	SVector2f GetViewportSize() const noexcept;
	SVector2f GetImguiWindowOffset() const noexcept;

private:
	void Update();
	void SetImguiWindowOffset(const SVector2f aImguiOffset) noexcept;

	std::string myName;
	std::filesystem::path myPath;

	SMatrix33f myProjectionMatrix;
	SMatrix33f myProjectionMatrixInverse;

	RenderTarget* myResultTexture;

	SVector2f myImguiWindowOffset;
	SVector2f myViewportSize;

	ResourceProxy<RenderNodeGraph> myRenderGraph;

	DisplayUniformBuffer myUniformBuffer;

	Camera& myCamera;

	const SColor ClearColor = SColor::Grey();

	bool myHasFocus = false;
};

