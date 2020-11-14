#pragma once
#include <soloud.h>
#include <soloud_wav.h>
#include "Audio/AudioStructs.h"
#include "Audio/AudioMixingNodeGraph.h"
#include "Audio/SoundEffectNodeGraph.h"

class DebugSettingsSystem;

class AudioManager
	: public System<
		Policy::Set<
			PAdd<DebugSettingsSystem, PWrite>>
	>, Debugged
{
public:
	AudioManager(Dependencies&& aDependencies);
	~AudioManager();

	void Update() noexcept;

	void PlayAudio(const std::filesystem::path& aAudioPath);

	SoLoud::Bus& GetBus(const std::string& aBusName);
	SoLoud::Soloud& GetSoloudEngine();

protected:
	void PlayAudio(ResourceProxy<AudioFile> aAudioFileToPlay);

	void TryPlayQueuedAudioFiles();
	void RemoveNonPlayingFiles();

	virtual void Debug() override;

	bool myHasInitializedAudio = false;

	SoLoud::Soloud mySoloudEngine;
	SoLoud::Bus myBus;
	
	std::optional<SoLoud::handle> myBusHandle;

	plf::colony<SPlayingAudioFile> myCurrentlyPlayingAudioFiles;
	plf::colony<SQueuedAudioFile> myQueuedAudioFiles;

	AudioMixingNodeGraph myAudioMixingNodeGraph;

	plf::colony<SoundEffectNodeGraph> mySoundEffects;

	std::unordered_map<std::string, SoLoud::Bus> myBusses;
};
