#include "pch.h"
#include "AudioManager.h"
#include <soloud.h>
#include <filesystem>
#include "Imgui/FileSelector.h"
#include <soloud_wav.h>
#include "Debugging/DebugSettingsSystem.h"
#include "Audio/AudioFile.h"
#include "Resources/ResourceSystem.h"
#include "Audio/AudioStructs.h"
#include <soloud_speech.h>
#include "Imgui/NodeGraphGui.h"
#include "Audio/AudioMixingNodeGraph.h"

REGISTER_SYSTEM(AudioManager);

AudioManager::AudioManager()
	: Debugged("Audio Manager")
{
	myAudioMixingNodeGraph = SystemPtr<ResourceSystem<AudioMixingNodeGraph>>()->GetResource(std::filesystem::path("NodeGraphs") / "Audio" / "AudioNodeGraph.json");
	mySoloudEngine.init(mySoloudEngine.FLAGS::CLIP_ROUNDOFF, mySoloudEngine.BACKENDS::WASAPI, SoLoud::Soloud::AUTO, 2048, 2);
	myBusHandle = mySoloudEngine.play(myBus);

	mySoloudEngine.setMaxActiveVoiceCount(255);
	mySoloudEngine.mGlobalVolume = 0.1f;
}

AudioManager::~AudioManager()
{
	mySoloudEngine.deinit();
}

void AudioManager::Update() noexcept
{
	if (!myHasInitializedAudio)
	{
		myAudioMixingNodeGraph->ExecuteExternal();
		myHasInitializedAudio = true;
	}

	TryPlayQueuedAudioFiles();
}

void AudioManager::PlayAudio(const std::filesystem::path& aAudioPath)
{
	auto it = std::find_if(mySoundEffects.begin(), mySoundEffects.end(), [aAudioPath](const ResourceProxy<SoundEffectNodeGraph>& aSoundEffect) 
		{ 
			return std::filesystem::equivalent(aAudioPath, aSoundEffect->GetPath());
		});

	if (it == mySoundEffects.end())
		it = mySoundEffects.insert(SystemPtr<ResourceSystem<SoundEffectNodeGraph>>()->GetResource(aAudioPath));

	(*it)->Execute();
}

SoLoud::handle AudioManager::PlayAudio(ResourceProxy<AudioFile> aAudioFileToPlay)
{
	auto handle = myBusses["Gameplay"].play(aAudioFileToPlay->GetSource());

	myCurrentlyPlayingAudioFiles.insert({ handle, aAudioFileToPlay });

	return handle;
}

SoLoud::Bus& AudioManager::GetBus(const std::string& aBusName)
{
	return myBusses[aBusName];
}

SoLoud::Soloud& AudioManager::GetSoloudEngine()
{
	return mySoloudEngine;
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

	f32 volume = Get<DebugSettingsSystem>().GetOrCreateDebugValue("Audio/GlobalVolume", 0.1f);

	if (volume < 0.f)
		volume = 0.f;
	if (volume > 1.f)
		volume = 1.f;

	mySoloudEngine.mGlobalVolume = volume;

	static std::filesystem::path path;

	ImGui::Text("Audio backend: %s", mySoloudEngine.getBackendString());
	ImGui::Text("Channel count: %u", mySoloudEngine.getBackendChannels());
	ImGui::Text("Sample rate: %uhz", mySoloudEngine.getBackendSamplerate());
	ImGui::Text("Buffer size: %u samples", mySoloudEngine.getBackendBufferSize() / mySoloudEngine.getBackendChannels());
	ImGui::Text("Estimated latency: %.1fms", 1000.f * (static_cast<f32>(mySoloudEngine.getBackendBufferSize() / mySoloudEngine.getBackendChannels()) / static_cast<f32>(mySoloudEngine.getBackendSamplerate())));
	ImGui::Text("Active voices: %.2u/%.2u", mySoloudEngine.getActiveVoiceCount(), mySoloudEngine.getMaxActiveVoiceCount());
	ImGui::Text("Virtual voices: %.2u", mySoloudEngine.getVoiceCount());

	ImGui::Separator();

	ImGui::Text("Audio File tester");
	if (ImGui::FileSelector::SelectFile(path))
	{
		PlayAudio(path);
	}

	ImGui::Separator();
	ImGui::Text("Text To Speech");
	static char buffer[2048] = "What the fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and \n\
I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills. I am trained in gorilla warfare and I'm the top sniper in the entire US armed forces. \n\
You are nothing to me but just another target. I will wipe you the fuck out with precision the likes of which has never been seen before on this Earth, mark my fucking words. \n\
You think you can get away with saying that shit to me over the Internet? Think again, fucker. \n\
As we speak I am contacting my secret network of spies across the USA and your IP is being traced right now so you better prepare for the storm, maggot. \n\
The storm that wipes out the pathetic little thing you call your life. You're fucking dead, kid. \n\
I can be anywhere, anytime, and I can kill you in over seven hundred ways, and that's just with my bare hands. \n\
Not only am I extensively trained in unarmed combat, but I have access to the entire arsenal of the United States Marine Corps and I will use it to its full extent to wipe your miserable ass off the face of the continent, you little shit.\n\
If only you could have known what unholy retribution your little 'clever' comment was about to bring down upon you, maybe you would have held your fucking tongue. \n\
But you couldn't, you didn't, and now you're paying the price, you goddamn idiot. \n\
I will shit fury all over you and you will drown in it. You're fucking dead, kiddo.";

	ImGui::InputTextMultiline("Text", buffer, 2048);

	if (ImGui::Button("Play"))
	{
		static SoLoud::handle TtsHandle;
		static SoLoud::Speech speech;
		speech.setText(buffer);
		TtsHandle = mySoloudEngine.play(speech);
	}

	if (ImGui::Button("Edit audio mixing node graph"))
		myAudioMixingNodeGraph->ShouldBeVisible = true;

	if (myAudioMixingNodeGraph->ShouldBeVisible)
		WmGui::NodeGraphEditor::NodeGraphEdit(*myAudioMixingNodeGraph);

	ImGui::Separator();

	ImGui::Text("Loaded sound effects");

	for (auto& soundEffectNodeGraph : mySoundEffects)
	{
		if (ImGui::Button(soundEffectNodeGraph->GetName().c_str()))
			soundEffectNodeGraph->ShouldBeVisible = true;

		ImGui::PushID("Hejsan");
		WmGui::NodeGraphEditor::NodeGraphEdit(*soundEffectNodeGraph);
		ImGui::PopID();
	}

	ImGui::End(); 
}
