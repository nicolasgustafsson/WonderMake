#include "pch.h"
#include "AudioManager.h"
#include <soloud.h>
#include <Windows.h>
#include <commdlg.h>
#include <filesystem>
#include "Imgui/FileSelector.h"
#include <soloud_wav.h>
#include "Debugging/DebugSettingsSystem.h"
#include "Audio/AudioFile.h"
#include "Resources/ResourceSystem.h"
#include <System/SystemPtr.h>
#include "Audio/AudioStructs.h"

AudioManager::AudioManager()
	: Debugged("Audio Manager")
{
	mySoloudEngine.init();
}

void AudioManager::Update() noexcept
{
	TryPlayQueuedAudioFiles();
}

void AudioManager::PlayAudio(const std::filesystem::path& aAudioPath)
{
	ResourceProxy<AudioFile> audioFile = SystemPtr<ResourceSystem<AudioFile>>()->GetResource(aAudioPath);

	if (audioFile.IsValid())
	{
		PlayAudio(audioFile);
		mySoloudEngine.play(audioFile->GetSource());
	}
	else
	{
		myQueuedAudioFiles.insert({ audioFile });
	}
}

void AudioManager::PlayAudio(ResourceProxy<AudioFile> aAudioFileToPlay)
{
	auto handle = mySoloudEngine.play(aAudioFileToPlay->GetSource());
	myCurrentlyPlayingAudioFiles.insert({ handle, aAudioFileToPlay });
}

void AudioManager::TryPlayQueuedAudioFiles()
{
	auto it = myQueuedAudioFiles.begin();
	while (it != myQueuedAudioFiles.end())
	{
		SQueuedAudioFile& queuedFile = *it;
		if (queuedFile.AudioResource.IsValid())
		{
			PlayAudio(queuedFile.AudioResource);
			it = myQueuedAudioFiles.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void AudioManager::RemoveNonPlayingFiles()
{
	auto it = myCurrentlyPlayingAudioFiles.begin();
	while (it != myCurrentlyPlayingAudioFiles.end())
	{
		SPlayingAudioFile& playingAudio = *it;
		if (!mySoloudEngine.isValidVoiceHandle(playingAudio.Handle))
		{
			it = myCurrentlyPlayingAudioFiles.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void AudioManager::Debug()
{
	ImGui::Begin("Audio Manager");

	SystemPtr<DebugSettingsSystem> debugSettings;

	f32 volume = debugSettings->GetOrCreateDebugValue("Audio/GlobalVolume", 0.1f);

	if (volume < 0.f)
		volume = 0.f;
	if (volume > 1.f)
		volume = 1.f;

	mySoloudEngine.mGlobalVolume = volume;

	static std::filesystem::path path;

	if (ImGui::FileSelector::SelectFile(path))
	{
		PlayAudio(path);
	}

	ImGui::End(); 
}
