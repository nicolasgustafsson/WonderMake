#pragma once

#include "wondermake-utility/FilePathData.h"

#include <ostream>

class FilePath
{
public:
	enum class EFolder
	{
		Unset,
		Bin,
		Data,
		User,
		UserData
	};

	inline FilePath(
		const FilePathData&		aData = FilePathData::Get()) noexcept
		: myData	(&aData)
	{}
	inline FilePath(
		std::filesystem::path	aPath,
		const FilePathData&		aData = FilePathData::Get()) noexcept
		: Path		(std::move(aPath))
		, myData	(&aData)
	{}
	inline FilePath(
		const EFolder			aLocation,
		std::filesystem::path	aPath,
		const FilePathData&		aData = FilePathData::Get()) noexcept
		: Location	(aLocation)
		, Path		(std::move(aPath))
		, myData	(&aData)
	{}

	[[nodiscard]] inline bool operator==(const FilePath&) const noexcept = default;

	[[nodiscard]] inline operator std::filesystem::path() const
	{
		if (Path.is_absolute())
			return Path;

		switch (Location)
		{
		case EFolder::Unset:	return Path;
		case EFolder::Bin:		return myData->GetPathBin()			/ Path;
		case EFolder::Data:		return myData->GetPathData()		/ Path;
		case EFolder::User:		return myData->GetPathUser()		/ Path;
		case EFolder::UserData:	return myData->GetPathUserData()	/ Path;
		}

		return Path;
	}
	[[nodiscard]] inline operator std::string() const
	{
		return FilePath::operator std::filesystem::path().string();
	}

	[[nodiscard]] inline static FilePath Resolve(const std::filesystem::path& aPath, const FilePathData& aData = FilePathData::Get())
	{
		auto path = aPath.lexically_normal();

		const auto& nativePath = path.native();

		for (const auto& root : aData.GetAllPathsBin())
			if (auto rootLexical = root.lexically_normal(); nativePath.starts_with(rootLexical.native()))
				return FilePath(EFolder::Bin, std::filesystem::relative(path, rootLexical), aData);

		for (const auto& root : aData.GetAllPathsData())
			if (auto rootLexical = root.lexically_normal(); nativePath.starts_with(rootLexical.native()))
				return FilePath(EFolder::Data, std::filesystem::relative(path, rootLexical), aData);

		for (const auto& root : aData.GetAllPathsUser())
			if (auto rootLexical = root.lexically_normal(); nativePath.starts_with(rootLexical.native()))
				return FilePath(EFolder::User, std::filesystem::relative(path, rootLexical), aData);

		for (const auto& root : aData.GetAllPathsUserData())
			if (auto rootLexical = root.lexically_normal(); nativePath.starts_with(rootLexical.native()))
				return FilePath(EFolder::UserData, std::filesystem::relative(path, rootLexical), aData);

		if (aPath.is_relative())
			return FilePath(EFolder::Bin, path, aData);

		return FilePath(EFolder::Unset, path, aData);
	}
	
	EFolder					Location = EFolder::Unset;
	std::filesystem::path	Path;

private:
	const FilePathData* myData;

};

namespace std
{
	template<>
	struct hash<FilePath>
	{
		inline [[nodiscard]] size_t operator()(const FilePath& aPath) const noexcept
		{
			return std::filesystem::hash_value(aPath.Path) ^ static_cast<size_t>(aPath.Location);
		}
	};
}

inline static void WmLogStream(std::ostream& aStream, const FilePath& aPath)
{
	static constexpr auto folderToString = [](const FilePath::EFolder aLocation) -> std::optional<std::string_view>
	{
		switch (aLocation)
		{
		case FilePath::EFolder::Unset:		return "Unset";
		case FilePath::EFolder::Bin:		return "Bin";
		case FilePath::EFolder::Data:		return "Data";
		case FilePath::EFolder::User:		return "User";
		case FilePath::EFolder::UserData:	return "UserData";
		}

		return std::nullopt;
	};

	const auto locationText = folderToString(aPath.Location);

	if (locationText)
		aStream << '[' << *locationText << ']' << aPath.Path;
	else
		aStream << "[Unknown(" << static_cast<std::underlying_type_t<FilePath::EFolder>>(aPath.Location) << ")]" << aPath.Path;
}
