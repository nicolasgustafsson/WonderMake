#pragma once
#include <soloud_wav.h>

class AudioFile : public Resource, public NonCopyable
{
public:
	AudioFile(const std::filesystem::path& aPath);

	SoLoud::Wav& GetSource();

private:
	SoLoud::Wav mySource;
};

