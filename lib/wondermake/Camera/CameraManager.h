#pragma once

#include "Camera/Camera.h"
#include "Camera/Display.h"
#include "Utilities/Debugging/Debugged.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/plf_colony.h"
#include "wondermake-utility/Rotation.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/Vector.h"

#include <memory>
#include <optional>

class GlfwFacade;
class OpenGLFacade;
class RenderNodeGraph;
template<typename TResource>
class ResourceSystem;
class Window;

class DebugSystem;

class ConfigurationSystem;

class CameraManager
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<OpenGLFacade, PWrite>,
			PAdd<ResourceSystem<RenderNodeGraph>, PWrite>,
			PAdd<Window, PWrite>,
			PAdd<DebugSystem, PRead>>,
		STrait::Set<
			STGui,
			STWonderMake>>
	, public Debugged
{
public:
	CameraManager();

	/**
	 * Sets the Main Display, which will automatically will be rendered every frame and displayed on the screen.
	 *
	 * @param aDisplay Shared Pointer to the Display, CameraManager takes ownership of this Display
	 * @return void
	 */
	inline void SetMainDisplay(std::shared_ptr<Display> aDisplay) noexcept { myMainDisplay = std::move(aDisplay); }
	inline [[nodiscard]] std::shared_ptr<Display> GetMainDisplay() const noexcept { return myMainDisplay; }
	
	/**
	 * Creates a Display.
	 *
	 * @param aName Name used for debug purposes
	 * @param aSettings Settings used by the Display, will use the engine-set configuration if not set
	 * @param aRenderGraphPath Path to the Render Graph to be used by this Display, will use the default node graph if not set
	 * @return The created Display
	 */
	std::shared_ptr<Display> CreateDisplay(
		std::string							aName,
		std::optional<Display::SSettings>	aSettings = std::nullopt,
		std::optional<FilePath>				aRenderGraphPath = std::nullopt);
	std::shared_ptr<Camera> CreateCamera(
		std::string							aName,
		SVector2f							aPosition = SVector2f::Zero(),
		SRadianF32							aRotation = 0.f,
		f32									aScale = 1.f);

	inline void SetInputRegion(const SVector4f aRect) noexcept { myInputRegionRect = aRect; }
	inline [[nodiscard]] SVector4f GetInputRegion() const noexcept { return myInputRegionRect; }

	inline void SetInputDisplayMouse(std::weak_ptr<Display> aDisplay) noexcept { myInputDisplayMouse = std::move(aDisplay); };
	inline [[nodiscard]] std::weak_ptr<Display> GetInputDisplayMouse() const noexcept { return myInputDisplayMouse; };
	inline void SetInputDisplayKeyboard(std::weak_ptr<Display> aDisplay) noexcept { myInputDisplayKeyboard = std::move(aDisplay); };
	inline [[nodiscard]] std::weak_ptr<Display> GetInputDisplayKeyboard() const noexcept { return myInputDisplayKeyboard; };

protected:
	void Debug() override;

private:
	void InspectDisplays();
	void InspectCameras();

	plf::colony<std::weak_ptr<Display>>	myDisplays;
	plf::colony<std::weak_ptr<Camera>>	myCameras;

	plf::colony<std::shared_ptr<Display>>	myAddedDisplays;
	plf::colony<std::shared_ptr<Camera>>	myAddedCameras;

	std::shared_ptr<Display>	myMainDisplay;

	std::weak_ptr<Display>		myInputDisplayMouse;
	std::weak_ptr<Display>		myInputDisplayKeyboard;
	SVector4f					myInputRegionRect;

};
