#include <catch2/catch.hpp>

#include "Job/JobMock.h"

#include "FileSystem/ReadFileJob.h"

#include "WinFileSystem.h"

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

TEST_CASE("Passes Location correctly to FileSystem.", "[ReadFileJob]")
{
	JobMock<ReadFileJob> jobMock;

	FileSystemMock::InjectDependencies(std::tie());

	FileSystemMock fsMock;

	jobMock.Inject(fsMock);
	jobMock.ExpectAny();

	SECTION("Bin")
	{
		fsMock.SetExpectedArgs(FolderLocation::Bin);

		jobMock.Run(FolderLocation::Bin, "Dummy");
	}
	SECTION("Data")
	{
		fsMock.SetExpectedArgs(FolderLocation::Data);

		jobMock.Run(FolderLocation::Data, "Dummy");
	}
	SECTION("User")
	{
		fsMock.SetExpectedArgs(FolderLocation::User);

		jobMock.Run(FolderLocation::User, "Dummy");
	}
	SECTION("UserData")
	{
		fsMock.SetExpectedArgs(FolderLocation::UserData);

		jobMock.Run(FolderLocation::UserData, "Dummy");
	}
}

TEST_CASE("Completes with error if the location is unable to be found.", "[ReadFileJob]")
{
	JobMock<ReadFileJob> jobMock;

	FileSystemMock::InjectDependencies(std::tie());

	FileSystemMock fsMock;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	jobMock.Inject(fsMock);
	jobMock.ExpectError(ReadFileError::InvalidArguments);

	jobMock.Run(FolderLocation::Bin, "Dummy");
}

TEST_CASE("Completes with error if file does not exist.", "[ReadFileJob]")
{
	JobMock<ReadFileJob> jobMock;

	FileSystemMock::InjectDependencies(std::tie());

	FileSystemMock fsMock;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	fsMock.SetReturnValue(MockDir);
	jobMock.Inject(fsMock);
	jobMock.ExpectError(ReadFileError::FileNotFound);

	jobMock.Run(FolderLocation::Bin, "Non.Existent");
}

TEST_CASE("Completes with error if path is folder.", "[ReadFileJob]")
{
	JobMock<ReadFileJob> jobMock;

	FileSystemMock::InjectDependencies(std::tie());

	FileSystemMock fsMock;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	fsMock.SetReturnValue(MockDir);
	jobMock.Inject(fsMock);
	jobMock.ExpectError(ReadFileError::NotAFile);

	jobMock.Run(FolderLocation::Bin, "Folder");
}

TEST_CASE("Successfully reads a file.", "[ReadFileJob]")
{
	constexpr std::string_view dataInFile = "Hello World!";

	JobMock<ReadFileJob> jobMock;

	FileSystemMock::InjectDependencies(std::tie());

	FileSystemMock fsMock;

	fsMock.SetExpectedArgs(FolderLocation::Bin);
	fsMock.SetReturnValue(MockDir);
	jobMock.Inject(fsMock);
	jobMock.ExpectOutput(Container<u8, Iterable, Indexable, ContiguousElements, EqualityComparable>(dataInFile.begin(), dataInFile.end()));

	jobMock.Run(FolderLocation::Bin, "TextFile.txt");
}
