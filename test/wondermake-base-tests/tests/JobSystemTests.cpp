#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/JobBase.h"
#include "wondermake-base/JobSystem.h"

#include "WonderMakeBaseTests/JobMock.h"

class SystemDepdenency
	: public System<>
{};

class JobTestBase
	: public jobs_refactor::JobAbstracted<>
{};

class JobTestSub
	: public JobMock<JobTestBase>
{
public:
	MOCK_METHOD(void, Run, (Promise<void>), (override));
};

class JobTestSubWithDependencies
	: public JobTestBase
	, public jobs_refactor::JobSub<
		Policy::Set<
			PAdd<SystemDepdenency, PRead>>>
{
public:
	MOCK_METHOD(void, Run, (Promise<void>), (override));
};

class JobTestBaseWithInput
	: public jobs_refactor::JobAbstracted<
		jobs_refactor::JobInputSet<
			jobs_refactor::JobInput<u32>,
			jobs_refactor::JobInput<f32>>>
{};

class JobTestSubWithInput
	: public JobMock<JobTestBaseWithInput>
{
public:
	void Run(Promise<void> aPromise, u32 aValue) override
	{
		MockedRunU32(std::move(aPromise), aValue);
	}
	void Run(Promise<void> aPromise, f32 aValue) override
	{
		MockedRunF32(std::move(aPromise), aValue);
	}

	MOCK_METHOD(void, MockedRunU32, (Promise<void>, u32));
	MOCK_METHOD(void, MockedRunF32, (Promise<void>, f32));
};

template<jobs_refactor::CJob TJob>
inline constexpr auto JobCreator()
{
	return [](auto&& aDependencies)
	{
		TJob::InjectDependencies(std::move(aDependencies));

		return MakeSharedReference<TJob>();
	};
}

TEST(JobSystemTests, starting_an_unregistered_job_returns_a_canceled_future)
{
	JobRegistry jobRegistry;
	SystemContainer systemContainer;
	AnyExecutor executor = InlineExecutor();
	
	jobs_refactor::JobSystem::InjectDependencies(std::tie());

	auto jobSystem = std::make_shared<jobs_refactor::JobSystem>(jobRegistry, systemContainer, executor);

	auto future = jobSystem->StartJob<JobTestBase>();

	EXPECT_TRUE(future.IsCanceled());
}

TEST(JobSystemTests, starting_a_job_with_missing_dependencies_returns_a_canceled_future)
{
	JobRegistry jobRegistry;
	SystemContainer systemContainer;
	AnyExecutor executor = InlineExecutor();
	
	jobRegistry.AddJob<StrictMock<JobTestSubWithDependencies>, JobTestBase>(JobCreator<StrictMock<JobTestSubWithDependencies>>());

	jobs_refactor::JobSystem::InjectDependencies(std::tie());

	auto jobSystem = std::make_shared<jobs_refactor::JobSystem>(jobRegistry, systemContainer, executor);

	auto future = jobSystem->StartJob<JobTestBase>();

	EXPECT_TRUE(future.IsCanceled());
}

TEST(JobSystemTests, starting_a_job_should_call_the_run_function)
{
	JobRegistry jobRegistry;
	SystemContainer systemContainer;
	AnyExecutor executor = InlineExecutor();
	auto jobMock = CreateJobMock<StrictMock<JobTestSub>>();

	jobMock->AddToRegistry(jobRegistry);

	jobs_refactor::JobSystem::InjectDependencies(std::tie());

	auto jobSystem = std::make_shared<jobs_refactor::JobSystem>(jobRegistry, systemContainer, executor);

	EXPECT_CALL(*jobMock, Run);

	(void)jobSystem->StartJob<JobTestBase>();
}

TEST(JobSystemTests, starting_a_job_with_dependencies_should_call_the_run_function)
{
	JobRegistry jobRegistry;
	SystemContainer systemContainer;
	AnyExecutor executor = InlineExecutor();

	SystemDepdenency::InjectDependencies(std::tie());

	systemContainer.Add(std::make_shared<SystemDepdenency>());
	jobRegistry.AddJob<StrictMock<JobTestSubWithDependencies>, JobTestBase>([](auto&& aDependencies)
		{
			StrictMock<JobTestSubWithDependencies>::InjectDependencies(std::move(aDependencies));

			auto mock = MakeSharedReference<StrictMock<JobTestSubWithDependencies>>();

			EXPECT_CALL(*mock, Run);

			return mock;
		});

	jobs_refactor::JobSystem::InjectDependencies(std::tie());

	auto jobSystem = std::make_shared<jobs_refactor::JobSystem>(jobRegistry, systemContainer, executor);

	(void)jobSystem->StartJob<JobTestBase>();
}

TEST(JobSystemTests, promise_passed_to_run_controls_the_returned_future)
{
	JobRegistry jobRegistry;
	SystemContainer systemContainer;
	AnyExecutor executor = InlineExecutor();
	auto jobMock = CreateJobMock<NiceMock<JobTestSub>>();

	jobMock->AddToRegistry(jobRegistry);

	jobs_refactor::JobSystem::InjectDependencies(std::tie());

	auto jobSystem = std::make_shared<jobs_refactor::JobSystem>(jobRegistry, systemContainer, executor);

	EXPECT_CALL(*jobMock, Run)
		.WillOnce([](auto&& aPromise)
			{
				aPromise.Complete();
			});

	auto future = jobSystem->StartJob<JobTestBase>();

	EXPECT_TRUE(future.IsCompleted());
}

TEST(JobSystemTests, args_passed_to_startjob_is_passed_to_the_run_function)
{
	constexpr u32 dummyArgU32 = 1234;
	constexpr f32 dummyArgF32 = 12.34f;

	JobRegistry jobRegistry;
	SystemContainer systemContainer;
	AnyExecutor executor = InlineExecutor();
	auto jobMock = CreateJobMock<StrictMock<JobTestSubWithInput>>();

	jobMock->AddToRegistry(jobRegistry);

	jobs_refactor::JobSystem::InjectDependencies(std::tie());

	auto jobSystem = std::make_shared<jobs_refactor::JobSystem>(jobRegistry, systemContainer, executor);

	EXPECT_CALL(*jobMock, MockedRunU32(_, dummyArgU32));

	(void)jobSystem->StartJob<JobTestBaseWithInput>(dummyArgU32);

	EXPECT_CALL(*jobMock, MockedRunF32(_, dummyArgF32));

	(void)jobSystem->StartJob<JobTestBaseWithInput>(dummyArgF32);
}
