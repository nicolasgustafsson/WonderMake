#include "AudioNodeGraphResourceFactory.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(AudioMixingNodeGraphResourceFactory);
WM_REGISTER_SYSTEM(SoundEffectNodeGraphResourceFactory);

Future<SharedReference<AudioMixingNodeGraph>> AudioMixingNodeGraphResourceFactory::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	return MakeCompletedFuture<SharedReference<AudioMixingNodeGraph>>(aMakeOp(aPath));
}

Future<SharedReference<SoundEffectNodeGraph>> SoundEffectNodeGraphResourceFactory::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	return MakeCompletedFuture<SharedReference<SoundEffectNodeGraph>>(aMakeOp(aPath));
}
