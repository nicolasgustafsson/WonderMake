#pragma once
//[Nicos]: Shamefully stolen and repurposed with permission from: https://github.com/carlivandev/sic
//[Kevin]: Shamelessly refurbished to better fit my needs without permission from: https://github.com/carlivandev/sic - Get got scrub

#include "wondermake-ui/ShaderParsedData.h"

#include "wondermake-utility/FilePath.h"

namespace ShaderParser
{
	[[nodiscard]] ShaderParsedData ParseShader(const FilePath& aShaderPath, const FilePath& aSearchPath);
};
