#pragma once

#include "wondermake-utility/Typedefs.h"

class RenderLayer;

class RenderHandle
{
public:
	void Invalidate();
	u64 myId;

	RenderLayer& myRenderLayer;
};


