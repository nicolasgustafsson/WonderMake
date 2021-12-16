#include "pch.h"
#include "FileWatcher.h"

#ifdef _MSC_VER
#include <Windows.h>
#endif

REGISTER_SYSTEM(FileWatcher);

FileWatcher::FileWatcher()
{
	myShouldStop.store(false);

	myThread = std::thread([&]() { Watch(); });
}

void FileWatcher::UpdateFileChanges()
{
	std::lock_guard<std::mutex> lock(myMutex);
	while (!myQueuedChanges.empty())
	{
		std::wstring changedFile = std::move(myQueuedChanges.front());
		myQueuedChanges.pop();

		SFileChangedMessage fileChangedMessage;

		fileChangedMessage.FilePath = changedFile;

		if (std::filesystem::exists(fileChangedMessage.FilePath))
			WmDispatchMessage(fileChangedMessage);
	}
}

void FileWatcher::Watch()
{
#ifdef _MSC_VER
	auto path = std::filesystem::current_path();
	HANDLE directory = CreateFile(path.string().c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

	if (directory == INVALID_HANDLE_VALUE)
		return;
	
	FILE_NOTIFY_INFORMATION notifications[255];
	Stopwatch watch;

	while (myShouldStop.load() == false)
	{
		FILE_NOTIFY_INFORMATION* current = notifications;
		DWORD bytesReturned;

		if (ReadDirectoryChangesW(directory, notifications, sizeof(notifications), true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_ACTION_MODIFIED, &bytesReturned, nullptr, nullptr))
		{
			for (;;)
			{
				std::vector<wchar_t> data;
				data.resize(current->FileNameLength + 1);
				memcpy((void*)&data[0], (void*)current->FileName, current->FileNameLength);
				data.back() = L'\0';

				std::wstring fileName = std::wstring(data.begin(), data.end());

				std::filesystem::path filePath = fileName;

				static std::array<std::filesystem::path, 2> ignoredExtensions {"ini, txt"};

				if (std::find(ignoredExtensions.begin(), ignoredExtensions.end(), filePath.extension()) != ignoredExtensions.end())
				{
					if (current->NextEntryOffset == 0)
						break;

					current = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char*>(current) + current->NextEntryOffset);
					continue;
				}

				auto currentTime = watch.GetElapsedTime();
				auto&& lastTimeIt = lastTime.find(fileName);

				if (lastTime.find(fileName) == lastTime.end() ||
					currentTime > lastTimeIt->second + myThresholdForUpdate)
				{
					std::lock_guard<std::mutex> lock(myMutex);

					myQueuedChanges.push(fileName);

					lastTime[fileName] = currentTime;
				}

				if (current->NextEntryOffset == 0)
					break;

				current = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char*>(current) + current->NextEntryOffset);
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}
#endif
}

