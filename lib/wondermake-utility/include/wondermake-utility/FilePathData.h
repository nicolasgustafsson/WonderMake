#pragma once

#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/Singleton.h"

#include <filesystem>
#include <shared_mutex>

class FilePathData
	: public Singleton<FilePathData>
	, NonCopyable
	, NonMovable
{
public:
	inline FilePathData() noexcept = default;
	
	inline void Initialize(
		std::filesystem::path aPathBin,
		std::filesystem::path aPathData,
		std::filesystem::path aPathUser,
		std::filesystem::path aPathUserData) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathBin		= std::move(aPathBin);
		myPathData		= std::move(aPathData);
		myPathUser		= std::move(aPathUser);
		myPathUserData	= std::move(aPathUserData);
	}
	
	inline void SetPathBin(std::filesystem::path aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathBin = std::move(aPath);
	}
	inline void SetPathData(std::filesystem::path aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathData = std::move(aPath);
	}
	inline void SetPathUser(std::filesystem::path aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathUser = std::move(aPath);
	}
	inline void SetPathUserData(std::filesystem::path aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathUserData = std::move(aPath);
	}

	inline [[nodiscard]] const std::filesystem::path& GetPathBin() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathBin;
	}
	inline [[nodiscard]] const std::filesystem::path& GetPathData() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathData;
	}
	inline [[nodiscard]] const std::filesystem::path& GetPathUser() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathUser;
	}
	inline [[nodiscard]] const std::filesystem::path& GetPathUserData() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathUserData;
	}

private:
	mutable std::shared_mutex	myMutex;

	std::filesystem::path		myPathBin;
	std::filesystem::path		myPathData;
	std::filesystem::path		myPathUser;
	std::filesystem::path		myPathUserData;

};
