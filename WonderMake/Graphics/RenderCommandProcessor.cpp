#include "pch.h"
#include "RenderCommandProcessor.h"

void RenderLayer::AddToQueue(RenderCommand aCommand)
{
	myRenderCommands.push_back(std::move(aCommand));
}

void RenderLayer::ProcessQueue()
{
	std::sort(myRenderCommands.begin(), myRenderCommands.end(), std::less<RenderCommand>());

	for (auto&& renderCommand : myRenderCommands)
	{
		renderCommand.Execute();
	}
}

void RenderLayer::ClearQueue()
{
	myRenderCommands.clear();
}

RenderLayer& RenderCommandProcessor::GetRenderLayer(std::string& aRenderLayerName)
{
	//[Nicos]: Change this to find to avoid creating unnecessary layers?
	return myRenderLayers[aRenderLayerName];
}

void RenderCommandProcessor::Clear()
{
	for (auto& layer : myRenderLayers)
	{
		layer.second.ClearQueue();
	}
}
