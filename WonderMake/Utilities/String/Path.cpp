#include "pch.h"
#include "Path.h"

Path::Path(const std::filesystem::path& aPath)
{
	myPath = aPath;

	myPath = std::filesystem::relative(myPath, Platform::GetApplicationRoot());
}
