#pragma once

#include <soloud_wav.h>

// soloud_wav.h includes the windows header, which causes Windows macros to bleed into other includes.
// Including WinPlatform fixes that issue.
#include "wondermake-utility/WinPlatform.h"

#include "Resources/Resource.h"

class AudioFile : public Resource, public NonCopyable
{
public:
	AudioFile(const std::filesystem::path& aPath);

	SoLoud::Wav& GetSource();

private:
	SoLoud::Wav mySource;
};
