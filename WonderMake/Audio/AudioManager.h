#pragma once
#include <soloud.h>
#include <soloud_wav.h>
#include "Audio/AudioStructs.h"

class AudioManager : public System, Debugged
{
public:
	AudioManager();
	~AudioManager();

	void Update() noexcept;

	void PlayAudio(const std::filesystem::path& aAudioPath);

	SoLoud::Bus& GetBus(std::string aBusName);
	SoLoud::Soloud& GetSoloudEngine();

protected:
	void PlayAudio(ResourceProxy<AudioFile> aAudioFileToPlay);

	void TryPlayQueuedAudioFiles();
	void RemoveNonPlayingFiles();

	virtual void Debug() override;

	SoLoud::Soloud mySoloudEngine;
	SoLoud::Bus myBus;
	
	std::optional<SoLoud::handle> myBusHandle;

	plf::colony<SPlayingAudioFile> myCurrentlyPlayingAudioFiles;
	plf::colony<SQueuedAudioFile> myQueuedAudioFiles;

	std::unordered_map<std::string, SoLoud::Bus> myBusses;
};
