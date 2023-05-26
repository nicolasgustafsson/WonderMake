#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WinGuidGeneratorSystem.h"

constexpr Guid GuidData = std::array<u8, 16> { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

class WinGuidGeneratorSystemTest
	: public ::testing::Test
{
public:
	WinGuidGeneratorSystemTest()
		: myWinPlatformSystem(MakeSharedReference<NiceMock<WinPlatformSystemMock>>())
	{}

	void SetUp() override
	{
		myWinPlatformSystem->DelegateToFake();
	}

	void CreateWinGuidGeneratorSystem()
	{
		myWinGuidGeneratorSystem = MakeSystem<WinGuidGeneratorSystem>(std::make_tuple(myWinPlatformSystem));
	}

	SharedReference<NiceMock<WinPlatformSystemMock>>	myWinPlatformSystem;
	std::shared_ptr<WinGuidGeneratorSystem>				myWinGuidGeneratorSystem;
};

TEST_F(WinGuidGeneratorSystemTest, generatenew_returns_a_valid_guid)
{
	CreateWinGuidGeneratorSystem();

	EXPECT_CALL(*myWinPlatformSystem, CoCreateGuid)
		.WillOnce([](GUID* pguid) -> HRESULT
			{
				memcpy(pguid, GuidData.data(), GuidData.size());

				return S_OK;
			});

	const auto guid = myWinGuidGeneratorSystem->GenerateNew();

	ASSERT_TRUE(guid);
	EXPECT_EQ(*guid, GuidData);
}

TEST_F(WinGuidGeneratorSystemTest, generatenew_returns_a_nullopt_on_error)
{
	CreateWinGuidGeneratorSystem();

	const auto guid = myWinGuidGeneratorSystem->GenerateNew();

	ASSERT_FALSE(guid);
}