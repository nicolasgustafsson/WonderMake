#pragma once

#include "Camera/CameraManager.h"
#include "Debugging/DebugLineDrawer.h"
#include "Graphics/RenderObject.h"
#include "Graphics/SpriteRenderObject.h"
#include "Graphics/ScreenPassRenderObject.h"
#include "Graphics/RenderCommandProcessor.h"
#include "Utilities/Debugging/Debugged.h"

#include "wondermake-ui/OpenGLFacade.h"
#include "wondermake-ui/RenderTarget.h"
#include "wondermake-ui/Window.h"

#include "wondermake-base/System.h"

class Display;
class EngineUniformBuffer;
template<typename TResource>
class ResourceSystem;

class DebugSystem;

class ConfigurationSystem;

template class ResourceSystem<Shader<EShaderType::Vertex>>;
template class ResourceSystem<Shader<EShaderType::Fragment>>;
template class ResourceSystem<Shader<EShaderType::Geometry>>;

class GlfwFacade;

class Renderer
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<DebugSystem, PRead>,
			PAdd<ResourceSystem<Shader<EShaderType::Vertex>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Fragment>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Geometry>>, PWrite>,
			PAdd<EngineUniformBuffer, PWrite>,
			PAdd<Window, PRead>,
			PAdd<DebugLineDrawer, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<CameraManager, PWrite>,
			PAdd<RenderCommandProcessor, PWrite>,
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
	, public Debugged
{
public:
	Renderer() noexcept;

	void StartFrame();

	void FinishFrame();

	void SwapBuffers();

private:
	virtual void Debug(bool& aIsOpen) override;
	void RenderDisplay(const std::shared_ptr<Display>& aDisplay);

	[[nodiscard]] SVector2f GetDisplaySize(Display& aDisplay, const SVector2f aWindowSize);

	ScreenPassRenderObject myCopyPass;

};
