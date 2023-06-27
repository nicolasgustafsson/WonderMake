#pragma once

#include "wondermake-ui/AudioResource.h"

#include "wondermake-io/FileResourceFactory.h"

class AudioResourceFactory
	: public FileResourceFactory<
		AudioResource,
		Policy::Set<>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	Future<SharedReference<AudioResource>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;

};
