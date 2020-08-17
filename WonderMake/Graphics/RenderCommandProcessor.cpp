#include "pch.h"
#include "RenderCommandProcessor.h"

RenderCommandProcessor::RenderCommandProcessor()
{
	EnableTick();
}

void RenderCommandProcessor::AddToQueue(RenderCommand aCommand)
{
	myRenderCommands.push_back(std::move(aCommand));
}

void RenderCommandProcessor::ProcessQueue()
{
	std::sort(myRenderCommands.begin(), myRenderCommands.end(), std::less<RenderCommand>());

	for (auto&& renderCommand : myRenderCommands)
	{
		renderCommand.Execute();
	}
}

void RenderCommandProcessor::ClearQueue()
{
	myRenderCommands.clear();
}
