#pragma once

class BaseRenderObject;
class RenderLayer;

class RenderHandle
{
public:
	void Invalidate();
	void Moved(BaseRenderObject& aNewAddress);
	u64 myId;

	RenderLayer& myRenderLayer;
};


