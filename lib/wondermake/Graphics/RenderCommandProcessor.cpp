#include "pch.h"
#include "RenderCommandProcessor.h"

WM_REGISTER_SYSTEM(RenderCommandProcessor);

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

RenderLayer& RenderCommandProcessor::GetRenderLayer(std::string_view aRenderLayerName)
{
	//[Nicos]: Change this to find to avoid creating unnecessary layers?
	//conversion to string here as having a map full of string views is dangerous; Small buffer optimization should handle 99% of cases
	return myRenderLayers[std::string(aRenderLayerName)];
}

void RenderCommandProcessor::Clear()
{
	for (auto& layer : myRenderLayers)
	{
		layer.second.ClearQueue();
	}
}