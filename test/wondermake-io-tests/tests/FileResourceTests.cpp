#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeIoTests/FileWatcherSystemMock.h"

#include "WonderMakeBaseTests/GuidGeneratorSystemMock.h"

#include "wondermake-io/FileResource.h"
#include "wondermake-io/FileResourceFactory.h"

class TestFileResource final
	: public FileResource<>
{};
class TestGameResourceFactoryMock
	: public FileResourceFactory<
		TestFileResource>
{
public:
	MOCK_METHOD(Future<SharedReference<TestFileResource>>, CreateResourceStrategy, (FileResourceId, FilePath, MakeResourceOp), (override));
	MOCK_METHOD(void, DestroyResource, (TestFileResource&), (override));

};
inline constexpr std::string_view locTestResourceTypeName = "TestFileResource";

inline constexpr auto locDefaultMake = [](auto, auto, auto aMake)
{
	return MakeCompletedFuture<SharedReference<TestFileResource>>(aMake());
};

class ResourceTest
	: public ::testing::Test
{
protected:
	void SetUp()
	{
		myFilePathData.Initialize(
			"c:/bin",
			"c:/data",
			"c:/user",
			"c:/userdata");

		myFileWatcherSystemMock = std::make_shared<NiceMock<FileWatcherSystemMock>>(myFilePathData);
		
		myFileWatcherSystemMock->Initialize();

		FileResourceSystem::InjectDependencies(std::make_tuple(GeFileWatcherSystem()));

		myResourceSystem = std::make_shared<FileResourceSystem>();
		myGuidSystemMock = std::make_shared<NiceMock<GuidGeneratorSystemMock>>();

		TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(GetGuidSystem(), GetResourceSystem()));

		myFactoryMock = std::make_shared<NiceMock<TestGameResourceFactoryMock>>();
	}

	SharedReference<FileWatcherSystemMock> GeFileWatcherSystem()
	{
		return SharedReference<FileWatcherSystemMock>::FromPointer(myFileWatcherSystemMock)
			.Unwrap();
	}
	SharedReference<FileResourceSystem> GetResourceSystem()
	{
		return SharedReference<FileResourceSystem>::FromPointer(myResourceSystem)
			.Unwrap();
	}
	SharedReference<NiceMock<GuidGeneratorSystemMock>> GetGuidSystem()
	{
		return SharedReference<NiceMock<GuidGeneratorSystemMock>>::FromPointer(myGuidSystemMock)
			.Unwrap();
	}
	SharedReference<NiceMock<TestGameResourceFactoryMock>> GetFactorySystem()
	{
		return SharedReference<NiceMock<TestGameResourceFactoryMock>>::FromPointer(myFactoryMock)
			.Unwrap();
	}

	std::shared_ptr<NiceMock<FileWatcherSystemMock>>		myFileWatcherSystemMock;
	std::shared_ptr<FileResourceSystem>						myResourceSystem;
	std::shared_ptr<NiceMock<GuidGeneratorSystemMock>>		myGuidSystemMock;
	std::shared_ptr<NiceMock<TestGameResourceFactoryMock>>	myFactoryMock;
	FilePathData											myFilePathData;

};

TEST(ResourceTests, system_unset_factory_returns_uncompleted_future)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto future = resourceSystem->GetResource<TestFileResource>(path);

	EXPECT_TRUE(future.IsValid());
	EXPECT_TRUE(!future.IsDone());
}

TEST(ResourceTests, setting_factory_completes_futures)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<NiceMock<TestGameResourceFactoryMock>>();

	auto future = resourceSystem->GetResource<TestFileResource>(path);

	EXPECT_TRUE(future.IsValid());
	EXPECT_TRUE(!future.IsDone());

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	factoryMock->Initialize();

	EXPECT_TRUE(future.IsCompleted());
}

TEST_F(ResourceTest, factory_returning_canceled_future_cancels_returned_future)
{
	Guid		expectedId		= *Guid::Parse("6b8395b7-b07a-4928-9a8e-806a7a3ab82b");
	FilePath	expectedPath	= FilePath(FilePath::EFolder::Bin, "path");

	EXPECT_CALL(*myGuidSystemMock, GenerateNew)
		.WillOnce(Return(expectedId));
	EXPECT_CALL(*myFactoryMock, CreateResourceStrategy(Eq(expectedId), expectedPath, _))
		.WillOnce(Return(MakeCanceledFuture<SharedReference<TestFileResource>>()));
	
	auto future = myFactoryMock->CreateResource(expectedPath);

	EXPECT_TRUE(future.IsCanceled());
}

TEST_F(ResourceTest, created_resource_has_correct_values_set)
{
	Guid		expectedId			= *Guid::Parse("6b8395b7-b07a-4928-9a8e-806a7a3ab82b");
	FilePath	expectedPath		= FilePath(FilePath::EFolder::Bin, "path");
	u32			expectedGeneration	= 0;

	EXPECT_CALL(*myGuidSystemMock, GenerateNew)
		.WillOnce(Return(expectedId));
	EXPECT_CALL(*myFactoryMock, CreateResourceStrategy(Eq(expectedId), expectedPath, _))
		.WillOnce(locDefaultMake);
	
	auto future = myFactoryMock->CreateResource(expectedPath);

	EXPECT_TRUE(future.IsCompleted());

	auto testResource = *future.GetResult();

	EXPECT_EQ(testResource->Id(),			expectedId);
	EXPECT_EQ(testResource->Path(),			expectedPath);
	EXPECT_EQ(testResource->TypeName(),		locTestResourceTypeName);
	EXPECT_EQ(testResource->Generation(),	expectedGeneration);
}

TEST_F(ResourceTest, created_resource_with_params_has_correct_values_set)
{
	Guid		expectedId = *Guid::Parse("6b8395b7-b07a-4928-9a8e-806a7a3ab82b");
	FilePath	expectedPath = FilePath(FilePath::EFolder::Bin, "path");
	u32			expectedGeneration = 1234;

	EXPECT_CALL(*myFactoryMock, CreateResourceStrategy(Eq(expectedId), expectedPath, _))
		.WillOnce(locDefaultMake);

	auto future = myFactoryMock->CreateResourceWithParams(expectedPath, expectedId, expectedGeneration);

	EXPECT_TRUE(future.IsCompleted());

	auto testResource = *future.GetResult();

	EXPECT_EQ(testResource->Id(),			expectedId);
	EXPECT_EQ(testResource->Path(),			expectedPath);
	EXPECT_EQ(testResource->TypeName(),		locTestResourceTypeName);
	EXPECT_EQ(testResource->Generation(),	expectedGeneration);
}

 // Put in global namespace so that the name can be tested
struct TestStructFileResource final
	: public FileResource<>
{};

TEST(ResourceTests, created_resource_has_correct_values_set_when_struct)
{
	class TestGameStructResourceFactoryMock
		: public FileResourceFactory<
		TestStructFileResource>
	{
	public:
		MOCK_METHOD(Future<SharedReference<TestStructFileResource>>, CreateResourceStrategy, (FileResourceId, FilePath, MakeResourceOp), (override));
		MOCK_METHOD(void, DestroyResource, (TestStructFileResource&), (override));

	};
	static constexpr std::string_view testResourceTypeName = "TestStructFileResource";

	FilePathData fpData;

	auto fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();
	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	TestGameStructResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<NiceMock<TestGameStructResourceFactoryMock>>();

	Guid		expectedId = *Guid::Parse("6b8395b7-b07a-4928-9a8e-806a7a3ab82b");
	FilePath	expectedPath = FilePath(FilePath::EFolder::Bin, "path");
	u32			expectedGeneration = 0;
	
	auto defaultMake = [](auto, auto, auto aMake)
	{
		return MakeCompletedFuture<SharedReference<TestStructFileResource>>(aMake());
	};

	EXPECT_CALL(*guidSystemMock, GenerateNew)
		.WillOnce(Return(expectedId));
	EXPECT_CALL(*factoryMock, CreateResourceStrategy(Eq(expectedId), expectedPath, _))
		.WillOnce(defaultMake);
	
	auto future = factoryMock->CreateResource(expectedPath);

	EXPECT_TRUE(future.IsCompleted());

	auto testResource = *future.GetResult();

	EXPECT_EQ(testResource->Id(),			expectedId);
	EXPECT_EQ(testResource->Path(),			expectedPath);
	EXPECT_EQ(testResource->TypeName(),		testResourceTypeName);
	EXPECT_EQ(testResource->Generation(),	expectedGeneration);
}

TEST_F(ResourceTest, releasing_a_resource_makes_a_destroy_call)
{
	Guid		expectedId		= *Guid::Parse("6b8395b7-b07a-4928-9a8e-806a7a3ab82b");
	FilePath	expectedPath	= FilePath(FilePath::EFolder::Bin, "path");

	EXPECT_CALL(*myGuidSystemMock, GenerateNew)
		.WillOnce(Return(expectedId));
	EXPECT_CALL(*myFactoryMock, CreateResourceStrategy(Eq(expectedId), expectedPath, _))
		.WillOnce(locDefaultMake);
	EXPECT_CALL(*myFactoryMock, DestroyResource)
		.Times(0);
	
	auto future = myFactoryMock->CreateResource(expectedPath);

	TestFileResource& resource = **future.GetResult();

	EXPECT_CALL(*myFactoryMock, DestroyResource(Ref(resource)))
		.Times(1);

	future.Reset();
}

TEST(ResourceTests, getting_a_resource_twice_before_finished_calls_create_only_once)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	factoryMock->Initialize();

	auto [_, future] = MakeAsync<SharedReference<TestFileResource>>();

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(Return(future));

	auto f1 = resourceSystem->GetResource<TestFileResource>(path);
	auto f2 = resourceSystem->GetResource<TestFileResource>(path);

	EXPECT_TRUE(f1.IsValid());
	EXPECT_TRUE(f2.IsValid());
	EXPECT_FALSE(f1.IsDone());
	EXPECT_FALSE(f2.IsDone());
}

TEST(ResourceTests, keep_loaded_calls_create_but_never_removed_when_factory_is_already_set)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	factoryMock->Initialize();

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	resourceSystem->KeepLoaded<TestFileResource>(path);
}

TEST(ResourceTests, keep_loaded_calls_create_but_never_removed_when_factory_is_set_afterwards)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	resourceSystem->KeepLoaded<TestFileResource>(path);

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	factoryMock->Initialize();
}

TEST(ResourceTests, unload_removes_the_resource)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	factoryMock->Initialize();

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	resourceSystem->KeepLoaded<TestFileResource>(path);

	EXPECT_CALL(*factoryMock, DestroyResource);

	resourceSystem->Unload<TestFileResource>(path);
}

TEST(ResourceTests, keep_loaded_with_type_name_calls_create_but_never_removed_when_factory_is_already_set)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	factoryMock->Initialize();

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	resourceSystem->KeepLoaded("TestFileResource", path);
}

TEST(ResourceTests, keep_loaded_with_type_name_calls_create_but_never_removed_when_factory_is_set_afterwards)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	resourceSystem->KeepLoaded("TestFileResource", path);

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	factoryMock->Initialize();
}

TEST(ResourceTests, unload_with_type_name_removes_the_resource)
{
	FilePathData fpData;

	FilePath	path = FilePath(FilePath::EFolder::Bin, "path");
	auto		fileWatcherSys = MakeSharedReference<NiceMock<FileWatcherSystemMock>>(fpData);

	fileWatcherSys->Initialize();

	FileResourceSystem::InjectDependencies(std::make_tuple(fileWatcherSys));

	auto resourceSystem = MakeSharedReference<FileResourceSystem>();

	auto guidSystemMock = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	guidSystemMock->DelegateToFake();

	TestGameResourceFactoryMock::InjectDependencies(std::make_tuple(guidSystemMock, resourceSystem));

	auto factoryMock = MakeSharedReference<StrictMock<TestGameResourceFactoryMock>>();

	factoryMock->Initialize();

	EXPECT_CALL(*factoryMock, CreateResourceStrategy)
		.WillOnce(locDefaultMake);

	resourceSystem->KeepLoaded<TestFileResource>(path);

	EXPECT_CALL(*factoryMock, DestroyResource);

	resourceSystem->Unload("TestFileResource", path);
}
