#pragma once

#include "wondermake-ui/AudioResource.h"

#include "wondermake-io/FileResourceProxy.h"

struct SPlayingAudioFile
{
	SoLoud::handle Handle;
	FileResourceRef<AudioResource> AudioResource;
};

struct SQueuedAudioFile
{
	FileResourceRef<AudioResource> AudioResource;
};