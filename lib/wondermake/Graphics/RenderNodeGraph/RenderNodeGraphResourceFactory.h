#pragma once

#include "Graphics/RenderNodeGraph/RenderNodeGraph.h"

#include "wondermake-io/FileResourceFactory.h"

class RenderNodeGraphResourceFactory
	: public FileResourceFactory<
		RenderNodeGraph,
		Policy::Set<>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	Future<SharedReference<RenderNodeGraph>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;

};
