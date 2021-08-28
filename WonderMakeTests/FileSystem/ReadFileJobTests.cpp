#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "FileSystem/ReadFileJob.h"
#include "FileSystem/WinFileSystem.h"

constexpr auto InlineExecutor = [](auto aCallable)
{
	aCallable();
};

struct JobDependencies
{
	JobDependencies()
		: myScheduleSystem(InlineExecutor, InlineExecutor)
		, myJobSystem([this]() { JobSystem::InjectDependencies(std::tie(myScheduleSystem)); return JobSystem(mySystemContainer); }())
	{
		mySystemContainer.AddSystem<ScheduleSystem>([this]() -> auto& { return myScheduleSystem; });
		mySystemContainer.AddSystem<JobSystem>([this]() -> auto& { return myJobSystem; });
	};

	SystemContainer mySystemContainer;
	ScheduleSystem myScheduleSystem;

	JobSystem myJobSystem;
};

class FileSystemMock
	: public FileSystem
	, public SystemSub<>
{
public:

	void SetFolderSuffix(const FolderLocation /*aLocation*/, std::filesystem::path /*aSuffix*/) override
	{
		++myCallCountSet;
	};
	
	std::optional<std::filesystem::path> GetFolderLocation(const FolderLocation aLocation) override
	{
		++myCallCountGet;
		
		CHECK(aLocation == myLocation);

		return myPath;
	}

	void SetExpectedArgs(const FolderLocation aLocation)
	{
		myLocation = aLocation;
	}
	void SetReturnValue(std::filesystem::path aPath)
	{
		myPath = std::move(aPath);
	}

	u32 CallCountSet() const
	{
		return myCallCountSet;
	}
	u32 CallCountGet() const
	{
		return myCallCountGet;
	}

private:
	u32 myCallCountSet = 0;
	u32 myCallCountGet = 0;

	FolderLocation myLocation;
	std::optional<std::filesystem::path> myPath;
};

const auto MockDir = std::filesystem::current_path() / "FileSystem/MockFiles";

TEST_CASE("Completes with error if the location is unable to be found.", "[ReadFileJob]")
{
	JobDependencies dependencies;
	FileSystemMock fsMock;
	ReadFileJob::Promise promise;

	fsMock.SetExpectedArgs(FolderLocation::Bin);

	JobFuture<ReadFileJob::Output, ReadFileJob::OutputError> future(promise, dependencies.myScheduleSystem);
	u32 callCountCompleted = 0;
	u32 callCountError = 0;

	future.Then([&callCountCompleted](auto&&)
		{
			++callCountCompleted;
		});
	future.Error([&callCountError](auto&& aError)
		{
			++callCountError;
			CHECK(aError == ReadFileError::InvalidArguments);
		});

	ReadFileJob::InjectDependencies(promise, std::tie(dependencies.myJobSystem, fsMock));

	ReadFileJob readFile(FolderLocation::Bin, "Dummy");

	CHECK(callCountCompleted == 0);
	CHECK(callCountError == 1);
}

TEST_CASE("Completes with error if file does not exist.", "[ReadFileJob]")
{
	JobDependencies dependencies;
	FileSystemMock fsMock;
	ReadFileJob::Promise promise;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	fsMock.SetReturnValue(MockDir);

	JobFuture<ReadFileJob::Output, ReadFileJob::OutputError> future(promise, dependencies.myScheduleSystem);
	u32 callCountCompleted = 0;
	u32 callCountError = 0;

	future.Then([&callCountCompleted](auto&&)
		{
			++callCountCompleted;
		});
	future.Error([&callCountError](auto&& aError)
		{
			++callCountError;
			CHECK(aError == ReadFileError::FileNotFound);
		});

	ReadFileJob::InjectDependencies(promise, std::tie(dependencies.myJobSystem, fsMock));

	ReadFileJob readFile(FolderLocation::Bin, "Non.Existent");

	CHECK(callCountCompleted == 0);
	CHECK(callCountError == 1);
}

TEST_CASE("Completes with error if path is folder.", "[ReadFileJob]")
{
	JobDependencies dependencies;
	FileSystemMock fsMock;
	ReadFileJob::Promise promise;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	fsMock.SetReturnValue(MockDir);

	JobFuture<ReadFileJob::Output, ReadFileJob::OutputError> future(promise, dependencies.myScheduleSystem);
	u32 callCountCompleted = 0;
	u32 callCountError = 0;

	future.Then([&callCountCompleted](auto&&)
		{
			++callCountCompleted;
		});
	future.Error([&callCountError](auto&& aError)
		{
			++callCountError;
			CHECK(aError == ReadFileError::NotAFile);
		});

	ReadFileJob::InjectDependencies(promise, std::tie(dependencies.myJobSystem, fsMock));

	ReadFileJob readFile(FolderLocation::Bin, "Folder");

	CHECK(callCountCompleted == 0);
	CHECK(callCountError == 1);
}

TEST_CASE("Successfully reads a file.", "[ReadFileJob]")
{
	JobDependencies dependencies;
	FileSystemMock fsMock;
	ReadFileJob::Promise promise;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	fsMock.SetReturnValue(MockDir);

	JobFuture<ReadFileJob::Output, ReadFileJob::OutputError> future(promise, dependencies.myScheduleSystem);
	u32 callCountCompleted = 0;
	u32 callCountError = 0;

	std::string buffer;

	future.Then([&callCountCompleted, &buffer](auto&& aBuffer)
		{
			buffer = std::string(aBuffer.begin(), aBuffer.end());

			++callCountCompleted;
		});
	future.Error([&callCountError](auto&&)
		{
			++callCountError;
		});

	ReadFileJob::InjectDependencies(promise, std::tie(dependencies.myJobSystem, fsMock));

	ReadFileJob readFile(FolderLocation::Bin, "TextFile.txt");

	CHECK(callCountCompleted == 1);
	CHECK(callCountError == 0);
	CHECK(buffer == "Hello World!");
}
