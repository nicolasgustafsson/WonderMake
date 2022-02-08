#pragma once

class RenderLayer;

class RenderHandle
{
public:
	void Invalidate();
	u64 myId;

	RenderLayer& myRenderLayer;
};


