#include "pch.h"
#include "AudioManager.h"
#include <soloud.h>

AudioManager::AudioManager()
	: Debugged("Audio Manager")
{
	  // SoLoud engine
}

void AudioManager::Debug()
{
	ImGui::Begin("Audio Manager");

	
	ImGui::Text("Hello friend");

	ImGui::End(); 
}
