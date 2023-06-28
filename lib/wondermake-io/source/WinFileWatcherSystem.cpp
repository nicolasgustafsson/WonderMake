#include "WinFileWatcherSystem.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"
#include "wondermake-base/WinEventSystem.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/StringUtility.h"

WM_REGISTER_SYSTEM_MASKED(WinFileWatcherSystem, FileWatcherSystem);

WinFileWatcherSystem::WinFileWatcherSystem()
	: FileWatcherSystem(GetExecutor())
{}

void WinFileWatcherSystem::Initialize()
{
	FileWatcherSystem::Initialize();

	auto& configSys = Get<ConfigurationSystem>();
	auto updateDirs = [this, &configSys](auto&&...)
	{
		UpdateRegisteredDirs(
			configSys.Get<std::string>(ConfigurationIo::ConfigDirectoryExtraBins,	""),
			configSys.Get<std::string>(ConfigurationIo::ConfigDirectoryData,		""),
			configSys.Get<std::string>(ConfigurationIo::ConfigDirectoryUser,		""),
			configSys.Get<std::string>(ConfigurationIo::ConfigDirectoryUserData,	""));
	};

	configSys.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryExtraBins,	GetExecutor(), updateDirs)
		.Detach();
	configSys.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryData,		GetExecutor(), updateDirs)
		.Detach();
	configSys.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryUser,		GetExecutor(), updateDirs)
		.Detach();
	configSys.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryUserData,	GetExecutor(), updateDirs)
		.Detach();
}

void WinFileWatcherSystem::RegisterDirWatch(std::filesystem::path aDirPath)
{
	auto& platformSys = Get<WinPlatformSystem>();

	auto dirName = aDirPath.wstring();

    HANDLE handleDir = platformSys.CreateFileW(
        dirName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (handleDir == INVALID_HANDLE_VALUE)
    {
		const DWORD err = platformSys.GetLastError();

		WmLogError(TagWonderMake << "Failed to get directory handle; error: " << err << ", dir path: " << aDirPath << '.');

		return;
    }

	static constexpr LPSECURITY_ATTRIBUTES	lpEventAttributes	= NULL;
	static constexpr BOOL					bManualReset		= TRUE;
	static constexpr BOOL					bInitialState		= FALSE;
	static constexpr LPCWSTR				lpName				= NULL;

	OVERLAPPED overlapped = {};

	overlapped.hEvent = platformSys.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (overlapped.hEvent == NULL)
	{
		const DWORD err = platformSys.GetLastError();

		WmLogError(TagWonderMake << "Failed to create event handle for directory watch; error: " << err << '.');

		return;
	}

	auto dirStr = aDirPath.string();

	auto dirDataPtr = std::make_unique<SDirectoryData>(
		SDirectoryData
		{
			.DirPath			= std::move(aDirPath),
			.HandleDir			= WinEventHandle(handleDir, platformSys),
			.HandleOverlapped	= WinEventHandle(overlapped.hEvent, platformSys),
			.Overlapped			= std::move(overlapped)
		});

	auto& [dirPath, dataPtr] = *myDirectoriesData.emplace(std::make_pair(std::move(dirStr), std::move(dirDataPtr))).first;

	SetCallback(*dataPtr);
}

void WinFileWatcherSystem::UnregisterDirWatch(std::filesystem::path aDirPath)
{
	myDirectoriesData.erase(aDirPath.string());
}

void WinFileWatcherSystem::SetCallback(SDirectoryData& aData)
{
	auto& eventSys		= Get<WinEventSystem>();
	auto& platformSys	= Get<WinPlatformSystem>();

	aData.Buffer = {};

	BOOL result = platformSys.ReadDirectoryChangesW(
		aData.HandleDir.Get(),
		aData.Buffer.data(),
		static_cast<DWORD>(aData.Buffer.size()),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
		NULL,
		&aData.Overlapped,
		NULL);

	if (result != TRUE)
	{
		const DWORD err = platformSys.GetLastError();

		WmLogError(TagWonderMake << "Failed to read directory changes; error: " << err << ", dir path: " << aData.DirPath << '.');

		return;
	}

	aData.Future = eventSys.RegisterEvent(aData.HandleOverlapped)
		.ThenRun(GetExecutor(), FutureRunResult([this, &aData]()
		{
			ProcessCallback(aData);
		}));
}

void WinFileWatcherSystem::ProcessCallback(SDirectoryData& aData)
{
	auto& platformSys = Get<WinPlatformSystem>();

	DWORD bytes = 0;

	const BOOL result = platformSys.GetOverlappedResult(aData.HandleOverlapped.Get(), &aData.Overlapped, &bytes, FALSE);

	const DWORD err = platformSys.GetLastError();

	platformSys.ResetEvent(aData.HandleOverlapped.Get());

	DWORD offset = 0;
    FILE_NOTIFY_INFORMATION* fni = NULL;

    do
    {
        fni = (FILE_NOTIFY_INFORMATION*)(&aData.Buffer[offset]);

		auto pathString = Utility::Utf8Encode(std::wstring(fni->FileName, fni->FileNameLength / sizeof(WCHAR)));

		FilePath path = aData.DirPath;

		path.Path.append(pathString);
		
		TriggerOnFileChange(path);

        offset += fni->NextEntryOffset;

    } while (fni->NextEntryOffset != 0);

	SetCallback(aData);
}
