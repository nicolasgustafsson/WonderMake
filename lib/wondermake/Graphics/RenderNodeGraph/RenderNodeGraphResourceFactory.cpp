#include "RenderNodeGraphResourceFactory.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(RenderNodeGraphResourceFactory);

Future<SharedReference<RenderNodeGraph>> RenderNodeGraphResourceFactory::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	return MakeCompletedFuture<SharedReference<RenderNodeGraph>>(aMakeOp(aPath));
}
