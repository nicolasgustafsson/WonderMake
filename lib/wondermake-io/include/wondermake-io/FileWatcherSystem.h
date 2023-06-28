#pragma once

#include "wondermake-io/ConfigurationIo.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Executor.h"
#include "wondermake-utility/FilePath.h"

#include <unordered_set>

class FileWatcherSystem
	: public SystemAbstracted
{
public:
	inline FileWatcherSystem(CExecutor auto&& aExecutor, const FilePathData& aFilePathData = FilePathData::Get())
		: myFilePathData(aFilePathData)
		, myOnFileChangeTriggers(std::forward<decltype(aExecutor)>(aExecutor))
	{}

	template<CExecutor TExecutor, typename TCallable>
	[[nodiscard]] inline EventSubscriber OnFileChange(const FilePath& aPath, TExecutor&& aExecutor, TCallable&& aCallable)
	{
		auto [trigger, subscriber] = MakeEventTrigger<FilePath>(std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallable));

		auto lexicalPath = aPath.LexicallyNormal();

		myOnFileChangeTriggers.Emplace(STriggerData(std::move(trigger), std::move(lexicalPath)));

		return std::move(subscriber);
	}

protected:
	void Initialize();
	void UpdateRegisteredDirs(
		const std::filesystem::path& aPathBin,
		const std::filesystem::path& aPathData,
		const std::filesystem::path& aPathUser,
		const std::filesystem::path& aPathUserData);
	void TriggerOnFileChange(const std::filesystem::path& aPath);

	virtual void RegisterDirWatch(std::filesystem::path aDirPath) = 0;
	virtual void UnregisterDirWatch(std::filesystem::path aDirPath) = 0;

private:
	struct STriggerData
	{
		inline STriggerData(EventTrigger<FilePath>&& aTrigger, FilePath aPath) noexcept
			: Trigger(std::move(aTrigger))
			, Path(std::move(aPath))
		{}
		inline STriggerData(STriggerData&&) noexcept = default;
		inline STriggerData& operator=(STriggerData&&) noexcept = default;

		inline void OnCancel(CExecutor auto&& aExecutor, auto&& aOnCancel)
		{
			Trigger
				.OnCancel()
				.ThenRun(std::forward<decltype(aExecutor)>(aExecutor), FutureRunResult(std::forward<decltype(aOnCancel)>(aOnCancel)))
				.Detach();
		}

		EventTrigger<FilePath>				Trigger;
		FilePath							Path;
	};

	const FilePathData&				myFilePathData;

	std::unordered_set<std::string>	myRegisteredDirs;
	CancelableList<STriggerData>	myOnFileChangeTriggers;

};
