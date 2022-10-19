#pragma once

#include "wondermake-io/FileTypes.h"

#include "wondermake-base/SystemContainer.h"

#include <optional>
#include <filesystem>

class PlatformFilePaths
{
public:
	static [[nodiscard]] std::optional<std::filesystem::path> GetFolderLocation(const SystemContainer& aContainer, const FolderLocation aLocation);

};
