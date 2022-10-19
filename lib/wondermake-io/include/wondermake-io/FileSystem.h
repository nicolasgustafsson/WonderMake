#pragma once

#include "wondermake-io/FileTypes.h"

#include "wondermake-base/System.h"

#include <optional>
#include <filesystem>

class FileSystem
	: public SystemAbstracted
{
public:
	virtual void SetFolderSuffix(const FolderLocation aLocation, std::filesystem::path aSuffix) = 0;

	virtual [[nodiscard]] std::optional<std::filesystem::path> GetFolderLocation(const FolderLocation aLocation) = 0;

protected:
	FileSystem() noexcept = default;

};