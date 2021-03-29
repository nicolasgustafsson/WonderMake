#include "pch.h"
#include "RenderObject.h"
#include "Graphics/RenderCommandProcessor.h"

BaseRenderObject::~BaseRenderObject()
{
	if (!myCurrentRenderHandle)
		return;

	myCurrentRenderHandle->Invalidate();

	myCurrentRenderHandle.reset();
}

void BaseRenderObject::Render()
{
	myCurrentRenderHandle.emplace(SystemPtr<RenderCommandProcessor>()->GetRenderLayer(myRenderLayer).AddToQueue(RenderCommand(*this, myRenderOrder)));
	
}

void BaseRenderObject::RenderImmediate()
{
	RenderInternal();
}

void RenderCommand::Execute()
{
	myRenderObject.get().RenderInternal();
}
