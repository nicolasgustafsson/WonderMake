#pragma once

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/Guid.h"
#include "wondermake-utility/RestrictTypes.h"

#include <string_view>
#include <typeindex>
#include <unordered_set>

using FileResourceId = Restricted<Guid, class FileResourceBase>;

class FileResourceBase
{
public:
	inline static void InjectResourceData(
		FileResourceId					aId,
		std::string_view				aTypeName,
		u32								aGeneration,
		FilePath						aPath,
		std::unordered_set<FilePath>	aRelatedFiles)
	{
		myNextData = SResourceData
		{
			.Id				= aId,
			.TypeName		= aTypeName,
			.Generation		= aGeneration,
			.Path			= std::move(aPath),
			.RelatedFiles	= std::move(aRelatedFiles)
		};
	}

	inline FileResourceBase() noexcept
		: myId(myNextData.Id)
		, myTypeName(myNextData.TypeName)
		, myGeneration(myNextData.Generation)
		, myFilePath(std::move(myNextData.Path))
		, myRelatedFiles(std::move(myNextData.RelatedFiles))
	{
		myNextData = {};
	}
	virtual ~FileResourceBase() noexcept = default;

	[[nodiscard]] inline FileResourceId Id() const noexcept
	{
		return myId;
	}
	[[nodiscard]] inline std::string_view TypeName() const noexcept
	{
		return myTypeName;
	}
	[[nodiscard]] inline const FilePath& Path() const noexcept
	{
		return myFilePath;
	}
	[[nodiscard]] inline u32 Generation() const noexcept
	{
		return myGeneration;
	}
	[[nodiscard]] inline const auto& RelatedFiles() const noexcept
	{
		return myRelatedFiles;
	}

private:
	struct SResourceData
	{
		FileResourceId					Id			= Guid::Zero();
		std::string_view				TypeName	= "[Error: Name unset]";
		u32								Generation	= 0;
		FilePath						Path;
		std::unordered_set<FilePath>	RelatedFiles;
	};

	static thread_local SResourceData myNextData;

	FileResourceId					myId;
	std::string_view				myTypeName;
	u32								myGeneration;
	FilePath						myFilePath;
	std::unordered_set<FilePath>	myRelatedFiles;

};

inline constexpr bool Reloadable = true;
inline constexpr bool NotReloadable = true;

template<bool Reloadable = Reloadable>
class FileResource
	: public FileResourceBase
{
public:
	static constexpr bool IsReloadable = Reloadable;

};

template<typename T>
concept CFileResource = std::is_base_of_v<FileResourceBase, T>;

template<CFileResource TResource>
[[nodiscard]] inline std::string_view GetFileResourceTypeName() noexcept
{
	static constexpr std::string_view prefixClass	= "class ";
	static constexpr std::string_view prefixStruct	= "struct ";

	std::string_view typeName = typeid(TResource).name();

	if (typeName.starts_with(prefixClass))
		return typeName.substr(prefixClass.size());

	if (typeName.starts_with(prefixStruct))
		return typeName.substr(prefixStruct.size());

	return typeName;
}