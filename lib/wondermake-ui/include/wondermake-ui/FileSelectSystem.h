#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/FlagSet.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Guid.h"
#include "wondermake-utility/Result.h"

#include <filesystem>
#include <optional>

class GuidGeneratorSystem;
class ScheduleSystemSingleton;

enum class EFileSelectFlags
{
	AllowFiles			= 0b01,
	AllowDirectories	= 0b10
};

template<>
struct EnableEnumBitwiseOperators<EFileSelectFlags>
	: public std::true_type
{};

class FileSelectSystem
	: public System<
		Policy::Set<
			PAdd<GuidGeneratorSystem, PWrite>,
			PAdd<ScheduleSystemSingleton, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	void Initialize() override;

	Future<FilePath> OpenFileBrowser(std::string aWindowTitle, FilePath aStartPath, EFileSelectFlags aFlags = EFileSelectFlags::AllowFiles);

private:
	enum class ESelection
	{
		Parent,
		Current,
		Child
	};

	struct SOpenFileData
	{
		inline void OnCancel(CExecutor auto&& aExecutor, auto&& aOnCancel)
		{
			Promise.OnCancel(std::forward<decltype(aExecutor)>(aExecutor), std::forward<decltype(aOnCancel)>(aOnCancel));
		}

		Promise<FilePath>					Promise;
		std::string							WindowTitleWithId;
		FilePath							CurrentPath;
		Guid								Id;
		EFileSelectFlags					SelectFlags;
		FilePath							DisplayPath;
		std::unordered_map<FilePath, bool>	Paths;
		ESelection							Selection = ESelection::Current;
	};

	void Tick();

	std::optional<bool> TickWindow(const std::string& aWindowTitleWithId, FilePath& aCurrentPath, EFileSelectFlags aSelectFlags, FilePath& aDisplayPath, std::unordered_map<FilePath, bool>& aPaths, ESelection& aSelection);
	void TickWindowHeader(FilePath& aCurrentPath, FilePath& aDisplayPath, std::unordered_map<FilePath, bool>& aPaths, ESelection& aSelection);
	bool TickWindowContent(FilePath& aCurrentPath, EFileSelectFlags aSelectFlags, FilePath& aDisplayPath, std::unordered_map<FilePath, bool>& aPaths, ESelection& aSelection);
	bool TickWindowFooter(bool aIsSelectable);
	void GetFilesAndFolders(const FilePath& aPath, std::unordered_map<FilePath, bool>& aPaths);

	CancelableList<SOpenFileData> myOpenFileRequests = GetExecutor();

};
