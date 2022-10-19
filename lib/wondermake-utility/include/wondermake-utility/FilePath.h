#pragma once

#include "wondermake-utility/FilePathData.h"

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
