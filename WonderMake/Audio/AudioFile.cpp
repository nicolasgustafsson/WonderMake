#include "pch.h"
#include "AudioFile.h"

AudioFile::AudioFile(const std::filesystem::path& aPath)
{
	mySource.load(aPath.string().c_str());
}

SoLoud::Wav& AudioFile::GetSource()
{
	return mySource;
}