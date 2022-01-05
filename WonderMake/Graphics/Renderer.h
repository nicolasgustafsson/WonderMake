#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "Camera/CameraManager.h"
#include "Program/Window.h"
#include "Message/MessageSubscriber.h"
#include "Utilities/Debugging/Debugged.h"
#include "Debugging/DebugLineDrawer.h"
#include "OpenGLFacade.h"
#include "Graphics/RenderCommandProcessor.h"

template<typename TResource>
class ResourceSystem;

template class ResourceSystem<Shader<EShaderType::Vertex>>;
template class ResourceSystem<Shader<EShaderType::Fragment>>;
template class ResourceSystem<Shader<EShaderType::Geometry>>;

class GlfwFacade;

class Renderer
	: public System<
		Policy::Set<
			PAdd<ResourceSystem<Shader<EShaderType::Vertex>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Fragment>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Geometry>>, PWrite>,
			PAdd<EngineUniformBuffer, PWrite>,
			PAdd<Window, PWrite>,
			PAdd<DebugLineDrawer, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<CameraManager, PWrite>,
			PAdd<RenderCommandProcessor, PWrite>,
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui>>
	, public Debugged
{
public:
	Renderer() noexcept;

	void SetViewportSize(const SVector2<int> WindowSize);

	void StartFrame();

	void FinishFrame();

	bool DebugWindowHasFocus() const noexcept { return myDebugWindowHasFocus; };

private:
	virtual void Debug() override;

	ScreenPassRenderObject myCopyPass;

	bool myDebugWindowHasFocus = true;
};
