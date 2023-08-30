#include "wondermake-ui/ShaderParsedData.h"

ShaderParsedData::SShaderCodeLine ShaderParsedData::GetShaderLineInfo(size_t aLine) const noexcept
{
	if (aLine >= myCodeLines.size())
		return SShaderCodeLine
		{
			.Path = FilePath(),
			.Line = 0
		};

	const auto& [pathIndex, line] = myCodeLines[aLine];

	return SShaderCodeLine
	{
		.Path = myRelatedFiles[pathIndex],
		.Line = line
	};
}
