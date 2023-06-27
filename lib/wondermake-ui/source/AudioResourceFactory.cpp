#include "AudioResourceFactory.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(AudioResourceFactory);

Future<SharedReference<AudioResource>> AudioResourceFactory::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	return MakeCompletedFuture<SharedReference<AudioResource>>(aMakeOp(aPath));
}
