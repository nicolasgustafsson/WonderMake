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

	inline [[nodiscard]] bool operator==(const FilePath&) const noexcept = default;

	inline [[nodiscard]] operator std::filesystem::path() const
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
	inline [[nodiscard]] operator std::string() const
	{
		return FilePath::operator std::filesystem::path().string();
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
