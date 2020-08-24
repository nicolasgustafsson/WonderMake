#include "pch.h"
#include "RenderObject.h"
#include "Graphics/RenderCommandProcessor.h"

void BaseRenderObject::Render()
{
	SystemPtr<RenderCommandProcessor>()->GetRenderLayer(myRenderLayer).AddToQueue(RenderCommand(*this, myRenderOrder));
}

void BaseRenderObject::RenderImmediate()
{
	RenderInternal();
}

void RenderCommand::Execute()
{
	myRenderObject.get().RenderInternal();
}
