#pragma once

#include <soloud_wav.h>

#include "wondermake-io/FileResource.h"

#include "wondermake-utility/FilePath.h"
// soloud_wav.h includes the windows header, which causes Windows macros to bleed into other includes.
// Including WinPlatform fixes that issue.
#include "wondermake-utility/WinPlatform.h"

class AudioResource
	: public FileResource<>
{
public:
	inline AudioResource(const FilePath& aPath)
	{
		std::string pathString = aPath;

		mySource.load(pathString.c_str());
	}

	[[nodiscard]] inline SoLoud::Wav& Source() noexcept
	{
		return mySource;
	}
	[[nodiscard]] inline const SoLoud::Wav& Source() const noexcept
	{
		return mySource;
	}

private:
	SoLoud::Wav mySource;
};
