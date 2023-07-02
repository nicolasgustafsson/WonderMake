#pragma once

#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/Singleton.h"

#include <filesystem>
#include <shared_mutex>
#include <vector>

class FilePathData
	: public Singleton<FilePathData>
	, NonCopyable
	, NonMovable
{
public:
	inline static std::vector<std::filesystem::path> ExtractPaths(const std::filesystem::path& aPath)
	{
		const auto&							str			= aPath.native();
		char								delimiter	= '|';
		std::vector<std::filesystem::path>	paths;

		size_t last = 0;
		size_t next = 0;
		
		while ((next = str.find(delimiter, last)) != std::string::npos)
		{
			paths.emplace_back(str.substr(last, next - last));

			last = next + 1;
		}

		paths.emplace_back(str.substr(last));

		return paths;
	}

	inline FilePathData() noexcept = default;
	
	inline void Initialize(
		const std::filesystem::path& aPathBin,
		const std::filesystem::path& aPathData,
		const std::filesystem::path& aPathUser,
		const std::filesystem::path& aPathUserData) noexcept
	{
		std::unique_lock lock(myMutex);
		
		myPathListBin		= ExtractPaths(aPathBin);
		myPathListData		= ExtractPaths(aPathData);
		myPathListUser		= ExtractPaths(aPathUser);
		myPathListUserData	= ExtractPaths(aPathUserData);

		if (!myPathListBin.empty())
			myPathBin = myPathListBin.front();
		if (!myPathListData.empty())
			myPathData = myPathListData.front();
		if (!myPathListUser.empty())
			myPathUser = myPathListUser.front();
		if (!myPathListUserData.empty())
			myPathUserData = myPathListUserData.front();
	}
	
	inline void SetPathBin(const std::filesystem::path& aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathBin.clear();
		myPathListBin = ExtractPaths(aPath);

		if (!myPathListBin.empty())
			myPathBin = myPathListBin.front();
	}
	inline void SetPathData(const std::filesystem::path& aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathData.clear();
		myPathListData = ExtractPaths(aPath);

		if (!myPathListData.empty())
			myPathData = myPathListData.front();
	}
	inline void SetPathUser(const std::filesystem::path& aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathUser.clear();
		myPathListUser = ExtractPaths(aPath);

		if (!myPathListUser.empty())
			myPathUser = myPathListUser.front();
	}
	inline void SetPathUserData(const std::filesystem::path& aPath) noexcept
	{
		std::unique_lock lock(myMutex);

		myPathUserData.clear();
		myPathListUserData = ExtractPaths(aPath);

		if (!myPathListUserData.empty())
			myPathUserData = myPathListUserData.front();
	}

	inline [[nodiscard]] std::filesystem::path GetPathBin() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathBin;
	}
	inline [[nodiscard]] std::filesystem::path GetPathData() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathData;
	}
	inline [[nodiscard]] std::filesystem::path GetPathUser() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathUser;
	}
	inline [[nodiscard]] std::filesystem::path GetPathUserData() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathUserData;
	}

	inline [[nodiscard]] auto GetAllPathsBin() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathListBin;
	}
	inline [[nodiscard]] auto GetAllPathsData() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathListData;
	}
	inline [[nodiscard]] auto GetAllPathsUser() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathListUser;
	}
	inline [[nodiscard]] auto GetAllPathsUserData() const noexcept
	{
		std::shared_lock lock(myMutex);

		return myPathListUserData;
	}

private:
	mutable std::shared_mutex			myMutex;

	std::filesystem::path				myPathBin;
	std::filesystem::path				myPathData;
	std::filesystem::path				myPathUser;
	std::filesystem::path				myPathUserData;

	std::vector<std::filesystem::path>	myPathListBin;
	std::vector<std::filesystem::path>	myPathListData;
	std::vector<std::filesystem::path>	myPathListUser;
	std::vector<std::filesystem::path>	myPathListUserData;

};
