#pragma once

#include "Audio/AudioMixingNodeGraph.h"
#include "Audio/SoundEffectNodeGraph.h"

#include "wondermake-io/FileResourceFactory.h"

class AudioMixingNodeGraphResourceFactory
	: public FileResourceFactory<
		AudioMixingNodeGraph,
		Policy::Set<>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	Future<SharedReference<AudioMixingNodeGraph>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;

};

class SoundEffectNodeGraphResourceFactory
	: public FileResourceFactory<
		SoundEffectNodeGraph,
		Policy::Set<>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	Future<SharedReference<SoundEffectNodeGraph>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;

};
