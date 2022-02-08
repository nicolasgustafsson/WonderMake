#include "wondermake-base/Process.h"

#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeBaseTests/WinEventSystemMock.h"
#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WinProcessSystem.h"

class WinProcessSystemTest
	: public ::testing::Test
{
protected:
	void SetUp()
	{
		myWinPlatformSystem.DelegateToFake();
	}

	void CreateWinProcessSystem()
	{
		WinProcessSystem::InjectDependencies(std::tie(myWinEventSystem, myWinPlatformSystem));

		myWinProcessSystem = std::make_shared<WinProcessSystem>();
	}

	NiceMock<WinEventSystemMock>		myWinEventSystem;
	NiceMock<WinPlatformSystemMock>		myWinPlatformSystem;
	std::shared_ptr<WinProcessSystem>	myWinProcessSystem;

};

const std::filesystem::path locDummyApplicationFilePath = L"./DummyApplication.exe";
const std::wstring locDummyCommandLine = L"-Dummy Command";

TEST_F(WinProcessSystemTest, can_be_constructed_and_destroyed)
{
	CreateWinProcessSystem();
}

TEST_F(WinProcessSystemTest, parameters_are_passed_to_createprocess)
{
	const std::wstring expectedCommandLine = L"\"./DummyApplication.exe\" -Dummy Command";

	CreateWinProcessSystem();

	EXPECT_CALL(myWinPlatformSystem, CreateProcessW(Eq(locDummyApplicationFilePath), Eq(expectedCommandLine), _, _, _, _, _, _, _, _));

	(void)myWinProcessSystem->StartProcess(locDummyApplicationFilePath, locDummyCommandLine);
}

TEST_F(WinProcessSystemTest, returns_file_not_found_when_createprocess_fails_with_file_not_found)
{
	CreateWinProcessSystem();

	EXPECT_CALL(myWinPlatformSystem, CreateProcessW)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_FILE_NOT_FOUND));

	const auto result = myWinProcessSystem->StartProcess(locDummyApplicationFilePath, locDummyCommandLine);

	ASSERT_FALSE(result);

	EXPECT_EQ(result, ProcessSystem::EStartError::FileNotFound);
	EXPECT_EQ(result.Meta(), ERROR_FILE_NOT_FOUND);
}

TEST_F(WinProcessSystemTest, returns_internal_error_when_createprocess_fails_with_unknown_error)
{
	CreateWinProcessSystem();
	
	EXPECT_CALL(myWinPlatformSystem, CreateProcessW)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_INVALID_FUNCTION));

	const auto result = myWinProcessSystem->StartProcess(locDummyApplicationFilePath, locDummyCommandLine);

	ASSERT_FALSE(result);

	EXPECT_EQ(result, ProcessSystem::EStartError::InternalError);
	EXPECT_EQ(result.Meta(), ERROR_INVALID_FUNCTION);
}

TEST_F(WinProcessSystemTest, returns_success_when_createprocess_returns_true)
{
	CreateWinProcessSystem();

	EXPECT_CALL(myWinPlatformSystem, CreateProcessW)
		.WillOnce(Return(TRUE));

	const auto result = myWinProcessSystem->StartProcess(locDummyApplicationFilePath, locDummyCommandLine);

	ASSERT_TRUE(result);
}

TEST_F(WinProcessSystemTest, returns_running_process)
{
	CreateWinProcessSystem();

	std::shared_ptr<Process> process = myWinProcessSystem->StartProcess(locDummyApplicationFilePath, locDummyCommandLine);

	ASSERT_NE(process, nullptr);

	EXPECT_EQ(process->GetState(), Process::EState::Running);
}
