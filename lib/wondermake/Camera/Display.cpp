#include "Display.h"

#include "Camera/Camera.h"

Display::Display(
	OpenGLFacade&						aOpenGlFacade,
	GlfwFacade&							aGlfwFacade,
	std::string							aName,
	ResourceProxy<RenderNodeGraph>		aRenderGraph,
	const SSettings&					aSettings)
	: myGlfwFacade(aGlfwFacade)
	, myName(std::move(aName))
	, myRenderGraph(std::move(aRenderGraph))
	, myUniformBuffer(aOpenGlFacade)
	, mySettings(aSettings)
{
	SetViewportSize(aSettings.Resolution);
}

void Display::Render()
{
	if (!myCamera || !myRenderGraph.IsValid())
		return;

	const auto viewInverse
		= myCamera->GetViewMatrix()
		.GetFastInverse();

	const auto projectionMatrix		= myProjectionMatrix;
	const auto viewProjectionMatrix	= myProjectionMatrix * viewInverse;

	auto& buffer = myUniformBuffer.GetBuffer();

	buffer.ProjectionMatrix		= projectionMatrix;
	buffer.ViewProjectionMatrix	= viewProjectionMatrix;
	buffer.Resolution			= myViewportSize;
	buffer.InverseResolution	= { 1.f / myViewportSize.X, 1.f / myViewportSize.Y };

	myUniformBuffer.Update();
	myCamera->BindBuffer();

	myRenderGraph->Execute();
}

void Display::SetViewportSize(const SVector2f aViewportSize) noexcept
{
	myProjectionMatrix[0][0]		= 2.0f / aViewportSize.X;
	myProjectionMatrix[1][1]		= 2.0f / aViewportSize.Y;
	myProjectionMatrixInverse[0][0]	= aViewportSize.X / 2.0f;
	myProjectionMatrixInverse[1][1]	= aViewportSize.Y / 2.0f;
	myViewportSize					= aViewportSize;

	if (!myRenderGraph.IsValid())
		return;

	myRenderGraph->myGlobalData["ViewportSize"].emplace<SVector2f>(myViewportSize);
}

void Display::SetResolution(const SVector2f aResolution) noexcept
{
	mySettings.Resolution = aResolution;

	SetViewportSize(aResolution);
}

void Display::SetRenderGraph(ResourceProxy<RenderNodeGraph> aRenderGraph)
{
	myRenderGraph = std::move(aRenderGraph);

	if (!myRenderGraph.IsValid())
		return;

	myRenderGraph->myGlobalData["ViewportSize"].emplace<SVector2f>(myViewportSize);
}

[[nodiscard]] RenderTarget* Display::GetRenderTarget() const noexcept
{
	if (!myRenderGraph.IsValid())
		return nullptr;

	return myRenderGraph->GetFinalRenderTarget();
}

[[nodiscard]] SVector2f Display::ConvertToWorldPosition(const SVector2f aViewPosition, const SVector2f aDefaultPosition) const noexcept
{
	if (!myCamera)
	return aDefaultPosition;
	
	SVector2f pos = aViewPosition;
	const auto view = myCamera->GetInverseViewMatrix();
	
	pos -= SVector2f::One() / 2.f;

	pos *= 2.f;

	pos.Y = -pos.Y;

	return (pos.Promote(1.f) * myProjectionMatrixInverse * view).Demote();
}
