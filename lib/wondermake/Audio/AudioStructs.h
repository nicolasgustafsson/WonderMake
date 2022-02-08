#pragma once
#include "AudioFile.h"
#include "Resources/ResourceProxy.h"

struct SPlayingAudioFile
{
	SoLoud::handle Handle;
	ResourceProxy<AudioFile> AudioResource;
};

struct SQueuedAudioFile
{
	ResourceProxy<AudioFile> AudioResource;
};