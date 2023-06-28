#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeIoTests/FileWatcherSystemMock.h"

#include "wondermake-io/FileWatcherSystem.h"

class FileWatcherCallbackMock
{
public:
	MOCK_METHOD(void, OnChange, (const FilePath&));

	auto GetCallable()
	{
		return [this](const FilePath& aPath)
		{
			OnChange(aPath);
		};
	}
};

inline constexpr auto locNoOp = [](auto&&...) {};

class FileWatcherSystemTest
	: public ::testing::Test
{
protected:
	void SetUp()
	{
		myFilePathData.Initialize(
			"c:/bin|d:/bin",
			"c:/data|d:/data",
			"c:/user|d:/user",
			"c:/userdata|d:/userdata");

		myFileWatcherSystemMock = std::make_shared<NiceMock<FileWatcherSystemMock>>(myFilePathData);

		myFileWatcherSystemMock->Initialize();
	}

	SharedReference<NiceMock<FileWatcherSystemMock>> GetFileWatcherSystem()
	{
		return SharedReference<NiceMock<FileWatcherSystemMock>>::FromPointer(myFileWatcherSystemMock)
			.Unwrap();
	}

	std::shared_ptr<NiceMock<FileWatcherSystemMock>>	myFileWatcherSystemMock;
	FilePathData										myFilePathData;

};

TEST(FileWatcherSystemTests, initialize_registers_all_paths)
{
	FilePathData fpData;

	fpData.Initialize(
		"c:/bin|d:/bin",
		"c:/data|d:/data",
		"c:/user|d:/user",
		"c:/userdata|d:/userdata");

	auto fileWatcherSystemMock = std::make_shared<StrictMock<FileWatcherSystemMock>>(fpData);

	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("c:/bin")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("c:/data")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("c:/user")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("c:/userdata")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("d:/bin")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("d:/data")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("d:/user")));
	EXPECT_CALL(*fileWatcherSystemMock, RegisterDirWatch(Eq("d:/userdata")));

	fileWatcherSystemMock->Initialize();
}

TEST_F(FileWatcherSystemTest, onfilechange_returns_valid_subscriber)
{
	auto subscriber = GetFileWatcherSystem()->OnFileChange(FilePath(), InlineExecutor(), locNoOp);

	EXPECT_TRUE(subscriber.IsValid());
}

TEST_F(FileWatcherSystemTest, trigger_calls_registered_callback_with_path)
{
	FilePath				expectedPath(FilePath::EFolder::Data, "test/path", myFilePathData);
	std::filesystem::path	triggerPathA = "c:/data/test/path";
	std::filesystem::path	triggerPathB = "d:/data/test/path";

	StrictMock<FileWatcherCallbackMock> callback;

	GetFileWatcherSystem()->OnFileChange(expectedPath, InlineExecutor(), callback.GetCallable())
		.Detach();

	EXPECT_CALL(callback, OnChange(expectedPath))
		.Times(1);

	GetFileWatcherSystem()->ExternalTrigger(triggerPathA);

	EXPECT_CALL(callback, OnChange(expectedPath))
		.Times(1);

	GetFileWatcherSystem()->ExternalTrigger(triggerPathB);
}
