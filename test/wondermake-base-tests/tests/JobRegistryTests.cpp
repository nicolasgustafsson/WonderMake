#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/JobRegistry.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Typedefs.h"

#include "WonderMakeBaseTests/JobMock.h"

class TestJobAMock
	: public Job<>
{
public:
	MOCK_METHOD(void, Run, (Promise<void>), (override));
};

class TestJobB
	: public JobAbstracted<
		JobInputSet<
			JobInput<void>,
			JobInput<u32>>>
{};

class TestJobBMock
	: public JobMock<TestJobB>
{
public:
	MOCK_METHOD(void, Run, (Promise<void>), (override));
	MOCK_METHOD(void, Run, (Promise<void>, u32), (override));
};

class TestSystemDepedency
	: public System<>
{};

class TestJobCMock
	: public Job<
		NoJobInput,
		NoJobOutput,
		Policy::Set<
			PAdd<TestSystemDepedency, PRead>>>
{
public:
	MOCK_METHOD(void, Run, (Promise<void>), (override));
};

class TestJobDMock
	: public TestJobB
	, public JobSub<
		Policy::Set<
			PAdd<TestSystemDepedency, PWrite>>>
{
public:
	void Run(Promise<void>) override {}
	void Run(Promise<void>, u32) override {};
};

TEST(JobRegistryTests, getjobpolicies_returns_error_when_job_is_not_registered)
{
	JobRegistry registry;

	auto result = registry.GetJobPolicies<TestJobAMock>();

	ASSERT_FALSE(result);

	EXPECT_EQ(result, JobRegistry::EGetPoliciesError::JobNotRegistered);
}

TEST(JobRegistryTests, getjobpolicies_returns_empty_list_when_job_has_no_dependencies)
{
	JobRegistry registry;

	registry.AddJob<TestJobAMock>([](auto&& aDependencies)
		{
			TestJobAMock::InjectDependencies(std::move(aDependencies));

			return MakeSharedReference<TestJobAMock>();
		});

	auto result = registry.GetJobPolicies<TestJobAMock>();

	ASSERT_TRUE(result);

	EXPECT_EQ(result, Policy::Set<>::GetPolicies());
}

TEST(JobRegistryTests, getjobpolicies_returns_list_when_job_has_dependencies)
{
	JobRegistry registry;
	const auto expectedPolicies = Policy::Set<PAdd<TestSystemDepedency, PRead>>::GetPolicies();

	registry.AddJob<TestJobCMock>([](auto&& aDependencies)
		{
			TestJobCMock::InjectDependencies(std::move(aDependencies));

			return MakeSharedReference<TestJobCMock>();
		});

	auto result = registry.GetJobPolicies<TestJobCMock>();

	ASSERT_TRUE(result);

	EXPECT_EQ(result, expectedPolicies);
}

TEST(JobRegistryTests, getjobpolicies_returns_list_when_masked_job_has_dependencies)
{
	JobRegistry registry;
	const auto expectedPolicies = Policy::Set<PAdd<TestSystemDepedency, PWrite>>::GetPolicies();

	registry.AddJob<TestJobDMock, TestJobB>([](auto&& aDependencies)
		{
			TestJobDMock::InjectDependencies(std::move(aDependencies));

			return MakeSharedReference<TestJobDMock>();
		});

	auto result = registry.GetJobPolicies<TestJobB>();

	ASSERT_TRUE(result);

	EXPECT_EQ(result, expectedPolicies);
}

TEST(JobRegistryTests, create_returns_error_when_job_is_not_added)
{
	JobRegistry registry;
	SystemContainer dummySystemContainer;

	auto result = registry.Create<TestJobAMock>(dummySystemContainer);

	ASSERT_FALSE(result);

	EXPECT_EQ(result, JobRegistry::ECreateError::JobNotRegistered);
}

TEST(JobRegistryTests, create_returns_error_when_a_dependency_is_missing)
{
	JobRegistry registry;
	SystemContainer dummySystemContainer;

	registry.AddJob<TestJobCMock>([](auto&& aDependencies)
		{
			TestJobCMock::InjectDependencies(std::move(aDependencies));

			return MakeSharedReference<TestJobCMock>();
		});

	auto result = registry.Create<TestJobCMock>(dummySystemContainer);

	ASSERT_FALSE(result);

	EXPECT_EQ(result, JobRegistry::ECreateError::MissingDependency);
}

TEST(JobRegistryTests, added_jobs_can_be_created)
{
	StrictMock<TestJobBMock>::InjectDependencies(std::tie());

	auto mock = MakeSharedReference<StrictMock<TestJobBMock>>();
	JobRegistry registry;
	SystemContainer dummySystemContainer;

	registry.AddJob<StrictMock<TestJobBMock>>([mock](auto&&)
		{
			return mock;
		});

	auto result = registry.Create<StrictMock<TestJobBMock>>(dummySystemContainer);

	ASSERT_TRUE(result);

	EXPECT_EQ(result, mock);
}

TEST(JobRegistryTests, added_jobs_can_be_run)
{
	StrictMock<TestJobBMock>::InjectDependencies(std::tie());

	auto mock = MakeSharedReference<StrictMock<TestJobBMock>>();
	JobRegistry registry;
	SystemContainer dummySystemContainer;

	registry.AddJob<StrictMock<TestJobBMock>>([mock](auto&&)
		{
			return mock;
		});

	SharedReference<StrictMock<TestJobBMock>> result = registry.Create<StrictMock<TestJobBMock>>(dummySystemContainer);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(*mock, Run(_));
	EXPECT_CALL(*mock, Run(_, _));

	result->Run(std::move(promise));
	result->Run(std::move(promise), 1);
}

TEST(JobRegistryTests, added_masked_jobs_can_be_created)
{
	auto mock = CreateJobMock<StrictMock<TestJobBMock>>();
	JobRegistry registry;
	SystemContainer dummySystemContainer;

	mock->AddToRegistry(registry);

	auto result = registry.Create<TestJobB>(dummySystemContainer);

	ASSERT_TRUE(result);

	EXPECT_EQ(result, mock);
}

TEST(JobRegistryTests, added_masked_jobs_can_be_run)
{
	auto mock = CreateJobMock<StrictMock<TestJobBMock>>();
	JobRegistry registry;
	SystemContainer dummySystemContainer;

	mock->AddToRegistry(registry);

	SharedReference<TestJobB> result = registry.Create<TestJobB>(dummySystemContainer);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(*mock, Run(_));
	EXPECT_CALL(*mock, Run(_, _));

	result->Run(std::move(promise));
	result->Run(std::move(promise), 1);
}
