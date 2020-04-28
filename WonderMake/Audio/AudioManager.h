#pragma once
#include <soloud.h>
#include <soloud_wav.h>
#include "Audio/AudioStructs.h"

class AudioManager : public System, Debugged
{
public:
	AudioManager();

	void Update() noexcept;

	void PlayAudio(const std::filesystem::path& aAudioPath);
protected:
	void PlayAudio(ResourceProxy<AudioFile> aAudioFileToPlay);

	void TryPlayQueuedAudioFiles();
	void RemoveNonPlayingFiles();

	virtual void Debug() override;

	SoLoud::Soloud mySoloudEngine;
	
	std::optional<SoLoud::handle> myBgmHandle;

	plf::colony<SPlayingAudioFile> myCurrentlyPlayingAudioFiles;
	plf::colony<SQueuedAudioFile> myQueuedAudioFiles;
};

