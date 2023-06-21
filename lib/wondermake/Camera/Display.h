#pragma once

#include "Graphics/RenderNodeGraph/RenderNodeGraph.h"
#include "Resources/ResourceProxy.h"

#include "wondermake-ui/EngineUniformBuffer.h"

#include "wondermake-utility/Matrix.h"
#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/Vector.h"

#include <memory>
#include <string>

class Camera;
class GlfwFacade;
class OpenGLFacade;
class RenderTarget;

class Display
	: public NonCopyable
	, public NonMovable
{
public:
	struct SSettings
	{
		SVector2f	Resolution;
		bool		FitToWindow = true;
		bool		FixedAspect = true;
	};

	Display(
		OpenGLFacade&						aOpenGlFacade,
		GlfwFacade&							aGlfwFacade,
		std::string							aName,
		ResourceProxy<RenderNodeGraph>		aRenderGraph,
		const SSettings&					aSettings);

	void Render();

	inline [[nodiscard]] const std::string&				GetName() const noexcept { return myName; }
	inline void											SetCamera(std::shared_ptr<Camera> aCamera) noexcept { myCamera = std::move(aCamera); }
	inline [[nodiscard]] std::shared_ptr<Camera>		GetCamera() const noexcept { return myCamera; }
	void												SetViewportSize(const SVector2f aViewportSize) noexcept;
	inline [[nodiscard]] SVector2f						GetViewportSize() const noexcept { return myViewportSize; }
	void												SetResolution(const SVector2f aResolution) noexcept;
	inline [[nodiscard]] SVector2f						GetResolution() const noexcept { return mySettings.Resolution; }
	inline void											SetFitToWindow(const bool aFlag) noexcept
	{
		mySettings.FitToWindow = aFlag;
		if (!aFlag)
			SetViewportSize(mySettings.Resolution);
	}
	inline [[nodiscard]] bool							GetFitToWindow() const noexcept { return mySettings.FitToWindow; }
	inline void											SetFixedAspect(const bool aFlag) noexcept { mySettings.FixedAspect = aFlag; }
	inline [[nodiscard]] bool							GetFixedAspect() const noexcept { return mySettings.FixedAspect; }
	inline [[nodiscard]] SSettings						GetSettings() const noexcept { return mySettings; }

	void												SetRenderGraph(ResourceProxy<RenderNodeGraph> aRenderGraph);
	inline [[nodiscard]] ResourceProxy<RenderNodeGraph>	GetRenderGraph() const { return myRenderGraph; };
	[[nodiscard]] RenderTarget*							GetRenderTarget() const noexcept;

	[[nodiscard]] SVector2f								ConvertToWorldPosition(const SVector2f aViewPosition, const SVector2f aDefaultPosition = SVector2f::Zero()) const noexcept;

private:
	GlfwFacade&						myGlfwFacade;

	std::string						myName;
	DisplayUniformBuffer			myUniformBuffer;
	ResourceProxy<RenderNodeGraph>	myRenderGraph;
	SSettings						mySettings;

	SVector2f						myViewportSize;
	 // TODO: This should probably be put in Camera.
	SMatrix33f						myProjectionMatrix;
	SMatrix33f						myProjectionMatrixInverse;

	std::shared_ptr<Camera>			myCamera;

};
