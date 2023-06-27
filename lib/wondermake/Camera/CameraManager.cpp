#include "CameraManager.h"

#include "wondermake-ui/DebugSystem.h"
#include "wondermake-ui/FileSelectSystem.h"
#include "wondermake-ui/GlfwFacade.h"
#include "wondermake-ui/Window.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-io/FileResourceSystem.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(CameraManager);

template<typename T>
inline constexpr auto MakeColonyDeleter(CExecutor auto aExecutor, plf::colony<T>& aColony, typename plf::colony<T>::iterator aIt, auto&&... aMeta)
{
	return [&colony = aColony, executor = std::move(aExecutor), aIt, ...meta = std::forward<decltype(aMeta)>(aMeta)](auto* const aElement) mutable
	{
		executor.Execute([&colony, aIt, aElement, ...meta = std::move(meta)]()
			{
				colony.erase(aIt);

				delete aElement;
			});
	};
};

CameraManager::CameraManager()
	: Debugged("Camera Manager", GetExecutor())
{}

std::shared_ptr<Display> CameraManager::CreateDisplay(
	std::string							aName,
	std::optional<Display::SSettings>	aSettings,
	std::optional<FilePath>				aRenderGraphPath)
{
	auto& configSys = Get<ConfigurationSystem>();

	const auto getScreenDisplay = [this]()
	{
		auto& configSys = Get<ConfigurationSystem>();

		const auto resolutionWidth	= configSys.Get<i32>(ConfigurationEngine::ResolutionWidth, 1280);
		const auto resolutionHeight	= configSys.Get<i32>(ConfigurationEngine::ResolutionHeight, 720);

		const SVector2f	resolution(static_cast<f32>(resolutionWidth), static_cast<f32>(resolutionHeight));

		return Display::SSettings
		{
			resolution,
			configSys.Get<bool>(ConfigurationEngine::ResolutionFitToWindow, true),
			configSys.Get<bool>(ConfigurationEngine::ResolutionFixedAspect, false)
		};
	};

	auto settings = aSettings ? *aSettings : getScreenDisplay();

	auto it = myDisplays.emplace();

	auto display = new Display(Get<OpenGLFacade>(), Get<GlfwFacade>(), std::move(aName), nullptr, settings);

	if (aSettings)
	{
		auto retVal = std::shared_ptr<Display>(display, MakeColonyDeleter(GetExecutor(), myDisplays, it));

		*it = retVal;

		return retVal;
	}

	struct SMeta
	{
		EventSubscriber Sub1;
		EventSubscriber Sub2;
		EventSubscriber Sub3;
		EventSubscriber Sub4;
	};

	const auto onChange = [&display = *display, getScreenDisplay](auto&&...)
	{
		auto settings = getScreenDisplay();

		if (settings.Resolution != display.GetResolution()
			|| settings.Resolution != display.GetViewportSize())
			display.SetResolution(settings.Resolution);
		if (settings.FitToWindow != display.GetFitToWindow())
			display.SetFitToWindow(settings.FitToWindow);
		if (settings.FixedAspect != display.GetFixedAspect())
			display.SetFixedAspect(settings.FixedAspect);
	};
	SMeta meta;

	meta.Sub1 = configSys.OnOverrideChanged<i32>(ConfigurationEngine::ResolutionWidth,			GetExecutor(), onChange);
	meta.Sub2 = configSys.OnOverrideChanged<i32>(ConfigurationEngine::ResolutionHeight,			GetExecutor(), onChange);
	meta.Sub3 = configSys.OnOverrideChanged<bool>(ConfigurationEngine::ResolutionFitToWindow,	GetExecutor(), onChange);
	meta.Sub4 = configSys.OnOverrideChanged<bool>(ConfigurationEngine::ResolutionFixedAspect,	GetExecutor(), onChange);

	auto retVal = std::shared_ptr<Display>(display, MakeColonyDeleter(GetExecutor(), myDisplays, it, std::move(meta)));

	*it = retVal;
	
	{
		std::weak_ptr<Display> wDisplay = retVal;

		auto& fileResSys = Get<FileResourceSystem>();

		fileResSys.GetResource<RenderNodeGraph>(*aRenderGraphPath)
			.OnCancel(GetExecutor(), [this, &configSys, &fileResSys, wDisplay]()
				{
					fileResSys
						.GetResource<RenderNodeGraph>(configSys.Get<FilePath>(ConfigurationEngine::DefaultRenderGraphPath, FilePath()))
						.ThenRun(GetExecutor(), FutureRunResult([wDisplay](FileResourceRef<RenderNodeGraph> aNodeGraph)
							{
								auto display = wDisplay.lock();

								if (!display)
									return;

								aNodeGraph->Load();

								display->SetRenderGraph(std::move(aNodeGraph));
							}))
						.Detach();
				})
			.ThenRun(GetExecutor(), FutureRunResult([wDisplay](FileResourceRef<RenderNodeGraph> aNodeGraph)
				{
					auto display = wDisplay.lock();

					if (!display)
						return;

					aNodeGraph->Load();

					display->SetRenderGraph(std::move(aNodeGraph));
				}))
			.Detach();
	}

	return retVal;
}

std::shared_ptr<Camera> CameraManager::CreateCamera(
	std::string							aName,
	SVector2f							aPosition,
	SRadianF32							aRotation,
	f32									aScale)
{
	auto it = myCameras.emplace();

	auto camera = std::shared_ptr<Camera>(new Camera(Get<OpenGLFacade>(), std::move(aName)), MakeColonyDeleter(GetExecutor(), myCameras, it));

	*it = camera;

	camera->SetPosition(aPosition);
	camera->SetRotation(aRotation);
	camera->SetScale(aScale);

	return camera;
}

void CameraManager::Debug(bool& aIsOpen)
{
	const auto getScreenDisplay = [this]()
	{
		auto& configSys = Get<ConfigurationSystem>();

		const auto resolutionWidth	= configSys.Get<i32>(ConfigurationEngine::ResolutionWidth, 1280);
		const auto resolutionHeight	= configSys.Get<i32>(ConfigurationEngine::ResolutionHeight, 720);

		const SVector2f	resolution(static_cast<f32>(resolutionWidth), static_cast<f32>(resolutionHeight));

		return Display::SSettings
		{
			resolution,
			configSys.Get<bool>(ConfigurationEngine::ResolutionFitToWindow, true),
			configSys.Get<bool>(ConfigurationEngine::ResolutionFixedAspect, false)
		};
	};

	ImGui::Begin("Camera Manager", &aIsOpen);

	ImGui::Text("%s", "Displays");

	ImGui::Separator();

	ImGui::Indent();

	InspectDisplays();

	ImGui::Unindent();

	ImGui::Separator();

	ImGui::Text("%s", "Cameras");

	ImGui::Separator();

	ImGui::Indent();

	InspectCameras();

	ImGui::Unindent();

	ImGui::End();
}

void CameraManager::InspectDisplays()
{
	std::vector<const char*> cameraNames;
	std::vector<std::shared_ptr<Camera>> cameraPtrs;

	cameraNames.reserve(myCameras.size());
	cameraPtrs.reserve(myCameras.size());

	for (const auto& cameraWeak : myCameras)
	{
		if (auto camera = cameraWeak.lock(); camera)
		{
			cameraNames.emplace_back(camera->GetName().c_str());
			cameraPtrs.emplace_back(camera);
		}
	}
	
	cameraNames.emplace_back("None");

	for (auto& displayWeak : myDisplays)
	{
		auto display = displayWeak.lock();

		if (!display)
			continue;

		ImGui::PushID(display.get());

		ImGui::Text("%s", display->GetName().c_str());

		bool isMainDisplay = myMainDisplay == display;

		if (ImGui::Checkbox("Main", &isMainDisplay))
			myMainDisplay = isMainDisplay ? display : nullptr;

		auto settings		= display->GetSettings();
		auto renderGraph	= display->GetRenderGraph();

		if (ImGui::InputFloat2("Resolution", &settings.Resolution.X))
			display->SetResolution(settings.Resolution);

		if (ImGui::Checkbox("Fit to Window", &settings.FitToWindow))
			display->SetFitToWindow(settings.FitToWindow);

		if (ImGui::Checkbox("Fixed Aspect", &settings.FixedAspect))
			display->SetFixedAspect(settings.FixedAspect);

		{
			FilePath path = renderGraph.IsValid() ? renderGraph->GetPath() : FilePath();

			std::stringstream ss;

			WmLogStream(ss, path);

			std::string pathStr = std::move(ss).str();

			if (pathStr.empty())
				ImGui::Text("Please select a file");
			else
				ImGui::Text(pathStr.c_str());

			if (ImGui::Button("Select File"))
			{
				Get<FileSelectSystem>()
					.OpenFileBrowser("Select camera node graph", path)
					.ThenApply(GetExecutor(), FutureApplyResult([this](FilePath aPath)
						{
							return Get<FileResourceSystem>()
								.GetResource<RenderNodeGraph>(aPath);
						}))
					.ThenRun(GetExecutor(), FutureRunResult([this, displayWeak](FileResourceRef<RenderNodeGraph> aRenderNodeGraph)
						{
							auto display = displayWeak.lock();

							if (!display)
								return;

							auto renderGraph = display->GetRenderGraph();

							aRenderNodeGraph->Load();

							display->SetRenderGraph(std::move(aRenderNodeGraph));
						}))
					.Detach();
			}
		}
		
		const bool hasRenderGraph = renderGraph.IsValid();

		if (!hasRenderGraph)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		ImGui::SameLine();

		if (ImGui::Button("Edit render node graph"))
			renderGraph->ShouldBeVisible = true;

		if (renderGraph.IsValid() && renderGraph->ShouldBeVisible)
			WmGui::NodeGraphEditor::NodeGraphEdit(*renderGraph);

		if (!hasRenderGraph)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		static int cameraIndex = 0;

		cameraIndex = 0;

		for (const auto& camera : cameraPtrs)
		{
			if (camera == display->GetCamera())
				break;

			++cameraIndex;
		}

		if (ImGui::Combo("Camera", &cameraIndex, cameraNames.data(), static_cast<int>(cameraNames.size())))
			display->SetCamera((cameraIndex >= cameraPtrs.size()) ? nullptr : cameraPtrs[cameraIndex]);

		const auto it = std::find(myAddedDisplays.begin(), myAddedDisplays.end(), display);

		const bool wasAdded = it != myAddedDisplays.end();

		if (!wasAdded)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("Remove"))
			myAddedDisplays.erase(it);

		if (!wasAdded)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::PopID();

		ImGui::Separator();
	}

	if (ImGui::Button("Add new Display"))
	{
		const Display::SSettings settings
		{
			SVector2f(320, 240),
			true,
			false
		};

		myAddedDisplays.emplace(CreateDisplay("Display " + std::to_string(myDisplays.size() + 1), settings, FilePath()));
	}
}

void CameraManager::InspectCameras()
{
	for (auto& cameraWeak : myCameras)
	{
		auto camera = cameraWeak.lock();

		if (!camera)
			continue;

		ImGui::PushID(camera.get());

		ImGui::Text("%s", camera->GetName().c_str());

		static constexpr f32 maxScale = 100.f;

		SVector2f position = camera->GetPosition();
		f32 scale = camera->GetScale();
		f32 rotation = SDegreeF32(camera->GetRotation()).Rotation;

		const float scalePercentage = (scale / maxScale);

		if (ImGui::DragFloat2("Position", &position.X, .05f / scalePercentage))
			camera->SetPosition(position);
		
		if (ImGui::DragFloat("Scale", &scale, .1f * scalePercentage, .01f, maxScale))
			camera->SetScale(scale);

		if (ImGui::DragFloat("Rotation", &rotation, .1f))
			camera->SetRotation(SDegreeF32(rotation));
		
		const auto it = std::find(myAddedCameras.begin(), myAddedCameras.end(), camera);

		const bool wasAdded = it != myAddedCameras.end();

		if (!wasAdded)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("Remove"))
			myAddedCameras.erase(it);

		if (!wasAdded)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::PopID();

		ImGui::Separator();
	}

	if (ImGui::Button("Add new Camera"))
		myAddedCameras.emplace(CreateCamera("Camera " + std::to_string(myDisplays.size() + 1)));
}
