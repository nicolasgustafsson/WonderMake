#include "RenderHandle.h"
#include "Graphics/RenderCommandProcessor.h"

void RenderHandle::Invalidate()
{
	myRenderLayer.RemoveObjectFromQueue(myId);
}


