#include "pch.h"
#include "RenderCommandProcessor.h"

RenderLayer::RenderLayer()
{
	myRenderCommands.reserve(10000);
}

RenderHandle RenderLayer::AddToQueue(RenderCommand aCommand)
{
	const u64 renderId = GetNextRenderId();
	aCommand.SetRenderId(renderId);
	myRenderCommands.push_back(std::move(aCommand));
	
	return { renderId, *this };
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

void RenderLayer::RemoveObjectFromQueue(const u64 aId)
{
	auto foundIt = std::find_if(myRenderCommands.begin(), myRenderCommands.end(), [aId](const RenderCommand& aRenderCommand) { return aRenderCommand.GetRenderId() == aId; });

	if (foundIt == myRenderCommands.end())
		return;

	myRenderCommands.erase(foundIt);
}

u64 RenderLayer::GetNextRenderId() noexcept
{
	//[Nicos]: This may overflow. That is 100% OK.
	return _myRenderId++;
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