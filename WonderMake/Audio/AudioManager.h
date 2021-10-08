#pragma once
#include <soloud.h>
#include <soloud_wav.h>
#include "Audio/AudioStructs.h"
#include "Audio/AudioMixingNodeGraph.h"
#include "Audio/SoundEffectNodeGraph.h"

class DebugSettingsSystem;

class AudioManager
	: public Systemus<AudioManager, DebugSettingsSystem>
	, Debugged
{
public:
	AudioManager();
	~AudioManager();

	void Update() noexcept;

	void PlayAudio(const std::filesystem::path& aAudioPath);
	SoLoud::handle PlayAudio(ResourceProxy<AudioFile> aAudioFileToPlay);

	SoLoud::Bus& GetBus(const std::string& aBusName);
	SoLoud::Soloud& GetSoloudEngine();

protected:

	void TryPlayQueuedAudioFiles();
	void RemoveNonPlayingFiles();

	virtual void Debug() override;

	bool myHasInitializedAudio = false;

	SoLoud::Soloud mySoloudEngine;
	SoLoud::Bus myBus;
	
	std::optional<SoLoud::handle> myBusHandle;

	plf::colony<SPlayingAudioFile> myCurrentlyPlayingAudioFiles;
	plf::colony<SQueuedAudioFile> myQueuedAudioFiles;

	ResourceProxy<AudioMixingNodeGraph> myAudioMixingNodeGraph;

	plf::colony<ResourceProxy<SoundEffectNodeGraph>> mySoundEffects;

	std::unordered_map<std::string, SoLoud::Bus> myBusses;
};
