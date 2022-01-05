#include "gtest/gtest.h"

#include "WonderMakeBase/DependencyInjector.h"

#include <functional>

struct TestMockWithNoDependencies {};
struct TestMockWithOneDependency
{
	TestMockWithOneDependency() = default;

	TestMockWithNoDependencies* myTestMock = nullptr;
};
struct TestMockWithTwoDependencies
{
	TestMockWithTwoDependencies() = default;

	TestMockWithNoDependencies* myTestMock = nullptr;
	TestMockWithOneDependency* mySecondTestMock = nullptr;
};

class DependencyInjectorTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		CreateMockA = [&counter = CountConstructA, &mock = MockA]() -> auto&
		{
			++counter;

			return mock;
		};
		CreateMockB = [&counter = CountConstructB, &mock = MockB](auto& aMockA) -> auto&
		{
			++counter;

			mock.myTestMock = &aMockA;

			return mock;
		};
		CreateMockC = [&counter = CountConstructC, &mock = MockC](auto& aMockA, auto& aMockB) -> auto&
		{
			++counter;

			mock.myTestMock = &aMockA;
			mock.mySecondTestMock = &aMockB;

			return mock;
		};
	}

	void AddA()
	{
		di.Add<TestMockWithNoDependencies>(CreateMockA);
	}
	void AddB()
	{
		di.Add<TestMockWithOneDependency, decltype(CreateMockB), TestMockWithNoDependencies>(CreateMockB);
	}
	void AddC()
	{
		di.Add<TestMockWithTwoDependencies, decltype(CreateMockC), TestMockWithNoDependencies, TestMockWithOneDependency>(CreateMockC);
	}

	DependencyInjector di;

	TestMockWithNoDependencies MockA;
	TestMockWithOneDependency MockB;
	TestMockWithTwoDependencies MockC;

	uint32_t CountConstructA = 0;
	uint32_t CountConstructB = 0;
	uint32_t CountConstructC = 0;

	std::function<TestMockWithNoDependencies&()> CreateMockA;
	std::function<TestMockWithOneDependency&(TestMockWithNoDependencies&)> CreateMockB;
	std::function<TestMockWithTwoDependencies&(TestMockWithNoDependencies&, TestMockWithOneDependency&)> CreateMockC;
};

TEST_F(DependencyInjectorTest, can_create_single_dependency_using_createall)
{
	AddA();

	EXPECT_EQ(CountConstructA, 0);

	di.CreateAll();

	EXPECT_EQ(CountConstructA, 1);
}

TEST_F(DependencyInjectorTest, can_create_single_dependency_using_get)
{
	AddA();

	EXPECT_EQ(CountConstructA, 0);

	(void)di.Get<TestMockWithNoDependencies>();

	EXPECT_EQ(CountConstructA, 1);
}

TEST_F(DependencyInjectorTest, can_create_several_dependencies_using_createall)
{
	AddA();
	AddB();
	AddC();

	EXPECT_EQ(CountConstructA, 0);
	EXPECT_EQ(CountConstructB, 0);
	EXPECT_EQ(CountConstructC, 0);

	di.CreateAll();

	EXPECT_EQ(CountConstructA, 1);
	EXPECT_EQ(CountConstructB, 1);
	EXPECT_EQ(CountConstructC, 1);
}

TEST_F(DependencyInjectorTest, can_create_several_dependencies_using_get)
{
	AddA();
	AddB();
	AddC();

	EXPECT_EQ(CountConstructA, 0);
	EXPECT_EQ(CountConstructB, 0);
	EXPECT_EQ(CountConstructC, 0);

	(void)di.Get<TestMockWithOneDependency>();

	EXPECT_EQ(CountConstructA, 1);
	EXPECT_EQ(CountConstructB, 1);
	EXPECT_EQ(CountConstructC, 0);
}

TEST_F(DependencyInjectorTest, can_fetch_object_with_no_dependencies)
{
	AddA();

	di.CreateAll();

	auto&& retDependency = di.Get<TestMockWithNoDependencies>();

	EXPECT_EQ(&MockA, &retDependency);
}

TEST_F(DependencyInjectorTest, can_fetch_object_with_dependencies)
{
	AddA();
	AddB();
	AddC();

	di.CreateAll();

	auto&& retDependency = di.Get<TestMockWithTwoDependencies>();

	EXPECT_EQ(&MockC, &retDependency);
	EXPECT_EQ(MockC.myTestMock, &MockA);
	EXPECT_EQ(MockC.mySecondTestMock, &MockB);
}

TEST_F(DependencyInjectorTest, throws_when_type_is_not_added)
{
	EXPECT_THROW(di.Get<TestMockWithNoDependencies>(), DependencyInjector::MissingDependencyException);
}

TEST_F(DependencyInjectorTest, throws_when_dependency_is_not_added)
{
	AddA();
	AddC();

	EXPECT_THROW(di.Get<TestMockWithTwoDependencies>(), DependencyInjector::MissingDependencyException);
}

TEST_F(DependencyInjectorTest, throws_when_dependency_is_not_added_during_createall)
{
	AddA();
	AddC();

	EXPECT_THROW(di.CreateAll(), DependencyInjector::MissingDependencyException);
}
