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

BaseRenderObject::BaseRenderObject(BaseRenderObject&& aOther) noexcept
{
	*this = std::move(aOther);
}

BaseRenderObject& BaseRenderObject::operator=(BaseRenderObject&& aOther) noexcept
{
	myRenderLayer = aOther.myRenderLayer;
	myRenderOrder = aOther.myRenderOrder;

	if (aOther.myCurrentRenderHandle)
	{
		aOther.myCurrentRenderHandle->Invalidate();
		aOther.myCurrentRenderHandle.reset();
		Render();
	}
	return *this;
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
