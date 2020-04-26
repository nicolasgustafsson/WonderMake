#include "pch.h"
#include "AudioManager.h"
#include <soloud.h>
#include <Windows.h>
#include <commdlg.h>
#include <filesystem>
#include "Imgui/FileSelector.h"
#include <soloud_wav.h>
#include "Debugging/DebugSettingsSystem.h"

AudioManager::AudioManager()
	: Debugged("Audio Manager")
{
	mySoloudEngine.init();
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

	static std::filesystem::path path = debugSettings->GetOrCreateDebugValue<std::string>("Audio/defaultBgm", "Audio/Songs/Who Likes to Party.mp3");

	if (ImGui::FileSelector::SelectFile(path) || !myBgm)
	{
		myBgm.emplace();
		myBgm->load(path.string().c_str());
		mySoloudEngine.play(*myBgm);
	}

	ImGui::End(); 
}
