#include "pch.h"
#include "AudioFile.h"
#include "Resources/AssetDatabase.h"
#include "Assets/AssetManager.h"


AudioFile::AudioFile(const std::filesystem::path& aPath)
{
	mySource.load(aPath.string().c_str());
}

SoLoud::Wav& AudioFile::GetSource()
{
	return mySource;
}

REGISTER_RESOURCE(AudioFile);
