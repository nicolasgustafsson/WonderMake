#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"
#include "Camera/Display.h"
#include "Debugging/DebugSettings.h"
#include "Program/GlfwFacade.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Graphics/RenderCommandProcessor.h"
#include "Graphics/RenderTarget.h"

#include "wondermake-debug-ui/DebugSystem.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include <GLFW/glfw3.h>

#include <any>

WM_REGISTER_SYSTEM(Renderer);

void GLAPIENTRY
MessageCallback([[maybe_unused]] GLenum source,
	GLenum type,
	[[maybe_unused]]GLuint id,
	GLenum severity,
	[[maybe_unused]]GLsizei length,
	const GLchar* message,
	[[maybe_unused]]const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	const auto logSeverity = severity == GL_DEBUG_SEVERITY_HIGH ? ELogSeverity::Error : (severity == GL_DEBUG_SEVERITY_MEDIUM) ? ELogSeverity::Warning : ELogSeverity::Info;

	WmLog(TagWonderMake << TagWmOpenGL << " type: " << type << ", severity: " << severity << ", message: {" << message << "}.", logSeverity, ELogLevel::Normal);
}

Renderer::Renderer() noexcept
	: Debugged("Renderer", GetExecutor())
	, myCopyPass(
			Get<ResourceSystem<Shader<EShaderType::Vertex>>>(),
			Get<ResourceSystem<Shader<EShaderType::Fragment>>>(),
			Get<ResourceSystem<Shader<EShaderType::Geometry>>>(),
			std::filesystem::current_path() / "Shaders/Fragment/BackbufferCopy.frag")
{
	Get<OpenGLFacade>().Enable(GL_DEBUG_OUTPUT);
	Get<OpenGLFacade>().Enable(GL_BLEND);

	Get<OpenGLFacade>().Enable(GL_DEPTH_TEST);

	glDepthFunc(GL_GEQUAL);
	glClearDepth(-1000);

	Get<OpenGLFacade>().SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Get<OpenGLFacade>().SetDebugMessageCallback(MessageCallback);

	auto& configSys = Get<ConfigurationSystem>();

	const auto updateVSync = [&glfw = Get<GlfwFacade>(), &configSys](auto&&...)
	{
		glfw.SwapInterval(configSys.Get<bool>(ConfigurationEngine::VSync, false) ? 1 : 0);
	};

	updateVSync();

	configSys.OnOverrideChanged<bool>(ConfigurationEngine::VSync, GetExecutor(), updateVSync)
		.Detach();
}

void Renderer::StartFrame()
{
	Get<OpenGLFacade>().BindFramebuffer(GL_FRAMEBUFFER, 0);

	Get<OpenGLFacade>().SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Get<OpenGLFacade>().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::FinishFrame()
{
	auto& cameraManager = Get<CameraManager>();

	auto display = cameraManager.GetMainDisplay();

	if (display)
		display->Render();

	Get<DebugLineDrawer>().Update();

	Get<RenderCommandProcessor>().Clear();

	Get<OpenGLFacade>().BindFramebuffer(GL_FRAMEBUFFER, 0);

	if (!display
		|| Get<DebugSystem>().IsDebugWindowVisible(WmDebug::RendererWindow))
		return;

	const auto& window = Get<Window>();

	const bool windowHasFocus = window.HasFocus();
	const auto& imguiIo = ImGui::GetIO();

	cameraManager.SetInputDisplayMouse(		(windowHasFocus && !imguiIo.WantCaptureMouse)								? display : std::weak_ptr<Display>());
	cameraManager.SetInputDisplayKeyboard(	(windowHasFocus && !imguiIo.WantCaptureKeyboard && !imguiIo.WantTextInput)	? display : std::weak_ptr<Display>());

	auto renderTarget = display->GetRenderTarget();

	if (!renderTarget)
		return;

	const auto windowSize = window.GetSize();

	const SVector2f windowSizeF(windowSize.X, windowSize.Y);

	const SVector2f renderSizeF = GetDisplaySize(*display, windowSizeF);
	const auto		settings = display->GetSettings();

	if (settings.FitToWindow)
		display->SetViewportSize(renderSizeF);

	const SVector2f positionF((windowSizeF - renderSizeF) / 2.f);
	const SVector2i renderSize(renderSizeF.X, renderSizeF.Y);

	Get<OpenGLFacade>().BindFramebuffer(GL_FRAMEBUFFER, 0);
	Get<OpenGLFacade>().SetViewport(SVector2i(positionF.X, positionF.Y), SVector2i(renderSizeF.X, renderSizeF.Y));
	Get<CameraManager>().SetInputRegion(SVector4f(positionF.X, positionF.Y, renderSizeF.X, renderSizeF.Y));

	renderTarget->BindAsTexture();

	myCopyPass.RenderImmediate();
}

void Renderer::SwapBuffers()
{
	Get<GlfwFacade>().SwapBuffers(Get<Window>().myGlfwWindow);
}

void Renderer::Debug(bool& aIsOpen)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	auto& cameraManager = Get<CameraManager>();

	auto display = cameraManager.GetMainDisplay();

	if (ImGui::Begin("Main Display", &aIsOpen))
		RenderDisplay(display);

	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void Renderer::RenderDisplay(const std::shared_ptr<Display>& aDisplay)
{
	if (!aDisplay)
	{
		ImGui::Text("%s", "Missing Display.");

		return;
	}

	if (!aDisplay->GetCamera())
	{
		ImGui::Text("%s", "Missing Camera.");

		return;
	}

	RenderTarget* const renderTarget = aDisplay->GetRenderTarget();

	if (!renderTarget)
	{
		ImGui::Text("%s", "Missing Render Target.");

		return;
	}

	ImGui::BeginChild(15, ImVec2(0, 0), false, ImGuiWindowFlags_NoMove);

	const auto		settings			= aDisplay->GetSettings();
	const SVector2f	viewportSize		= { static_cast<f32>(ImGui::GetContentRegionAvail().x), static_cast<f32>(ImGui::GetContentRegionAvail().y) };
	const uintptr_t	textureIdInteger	= renderTarget->GetTexture();

	const SVector2f renderSize = GetDisplaySize(*aDisplay, viewportSize);

	if (settings.FitToWindow)
		aDisplay->SetViewportSize(renderSize);

	const ImTextureID textureId = reinterpret_cast<ImTextureID>(textureIdInteger);
	const ImVec2 size(renderSize.X, renderSize.Y);
	static constexpr ImVec2 uv0(0.f, 1.f);
	static constexpr ImVec2 uv1(1.f, 0.f);

	const ImVec2 imguiWindowPosition = (ImGui::GetWindowSize() - size) / 2.f;
	
	const ImVec2 screenPosition = imguiWindowPosition + ImGui::GetWindowPos();
	const SVector2i windowPos = Get<Window>().GetPosition();
	auto& cameraManager = Get<CameraManager>();

	const auto& window = Get<Window>();

	const bool windowHasFocus = window.HasFocus() || ImGui::IsWindowFocused();
	const auto& imguiIo = ImGui::GetIO();

	ImGui::SetCursorPos(imguiWindowPosition);
	cameraManager.SetInputRegion(SVector4f(screenPosition.x - windowPos.X, screenPosition.y - windowPos.Y, renderSize.X, renderSize.Y));
	cameraManager.SetInputDisplayMouse(ImGui::IsWindowHovered() ? aDisplay : std::weak_ptr<Display>());
	cameraManager.SetInputDisplayKeyboard((windowHasFocus && !imguiIo.WantCaptureKeyboard && !imguiIo.WantTextInput) ? aDisplay : std::weak_ptr<Display>());

	ImGui::Image(textureId, size, uv0, uv1);

	ImGui::EndChild();
}

SVector2f Renderer::GetDisplaySize(Display& aDisplay, const SVector2f aWindowSize)
{
	const auto settings = aDisplay.GetSettings();

	if (!settings.FixedAspect)
		return aWindowSize;
	
	const float aspectX = aWindowSize.X / settings.Resolution.X;
	const float aspectY = aWindowSize.Y / settings.Resolution.Y;

	if (aspectX < aspectY)
		return { aWindowSize.X, aWindowSize.X * (settings.Resolution.Y / settings.Resolution.X) };
	
	return { aWindowSize.Y * (settings.Resolution.X / settings.Resolution.Y), aWindowSize.Y };
}
