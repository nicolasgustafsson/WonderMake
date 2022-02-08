#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/DependencyInjector.h"

struct TestMockWithNoDependencies
{
	MOCK_METHOD(void, Construct, ());

	auto CreateConstruct()
	{
		return [this]() -> auto&
		{
			Construct();

			return *this;
		};
	}
};
struct TestMockWithOneDependency
{
	TestMockWithOneDependency() = default;

	MOCK_METHOD(void, Construct, (TestMockWithNoDependencies&));

	auto CreateConstruct()
	{
		return[this](auto& aDependency) -> auto&
		{
			Construct(aDependency);

			return *this;
		};
	}
};
struct TestMockWithTwoDependencies
{
	TestMockWithTwoDependencies() = default;

	MOCK_METHOD(void, Construct, (TestMockWithNoDependencies&, TestMockWithOneDependency&));

	auto CreateConstruct()
	{
		return[this](auto& aDependencyA, auto& aDependencyB) -> auto&
		{
			Construct(aDependencyA, aDependencyB);

			return *this;
		};
	}
};

class DependencyInjectorTest
	: public ::testing::Test
{
protected:
	void AddA()
	{
		auto create = myMockA.CreateConstruct();

		myDependencyInjector.Add<TestMockWithNoDependencies>(create);
	}
	void AddB()
	{
		auto create = myMockB.CreateConstruct();

		myDependencyInjector.Add<TestMockWithOneDependency, decltype(create), TestMockWithNoDependencies>(create);
	}
	void AddC()
	{
		auto create = myMockC.CreateConstruct();

		myDependencyInjector.Add<TestMockWithTwoDependencies, decltype(create), TestMockWithNoDependencies, TestMockWithOneDependency>(create);
	}

	DependencyInjector myDependencyInjector;

	NiceMock<TestMockWithNoDependencies>	myMockA;
	NiceMock<TestMockWithOneDependency>		myMockB;
	NiceMock<TestMockWithTwoDependencies>	myMockC;
};

template<typename TType>
struct Comparator
{
public:
	Comparator(const TType& aRef)
		: Ref(aRef)
	{}

	const TType& Ref;

};

template<typename TRhsType, typename TLhsType>
bool operator==(const TRhsType& aLhs, Comparator<TLhsType> aRhs)
{
	return &aLhs == &aRhs.Ref;
}

TEST_F(DependencyInjectorTest, can_create_single_dependency_using_createall)
{
	EXPECT_CALL(myMockA, Construct).Times(0);

	AddA();

	EXPECT_CALL(myMockA, Construct).Times(1);

	ASSERT_TRUE(myDependencyInjector.CreateAll());
}

TEST_F(DependencyInjectorTest, can_create_single_dependency_using_get)
{
	AddA();

	EXPECT_CALL(myMockA, Construct);

	ASSERT_TRUE(myDependencyInjector.Get<TestMockWithNoDependencies>());
}

TEST_F(DependencyInjectorTest, can_create_several_dependencies_using_createall)
{
	EXPECT_CALL(myMockA, Construct).Times(0);
	EXPECT_CALL(myMockB, Construct).Times(0);
	EXPECT_CALL(myMockC, Construct).Times(0);

	AddA();
	AddB();
	AddC();

	EXPECT_CALL(myMockA, Construct);
	EXPECT_CALL(myMockB, Construct);
	EXPECT_CALL(myMockC, Construct);

	ASSERT_TRUE(myDependencyInjector.CreateAll());
}

TEST_F(DependencyInjectorTest, can_create_several_dependencies_using_get)
{
	AddA();
	AddB();
	AddC();

	EXPECT_CALL(myMockA, Construct).Times(1);
	EXPECT_CALL(myMockB, Construct).Times(1);
	EXPECT_CALL(myMockC, Construct).Times(0);

	ASSERT_TRUE(myDependencyInjector.Get<TestMockWithOneDependency>());
}

TEST_F(DependencyInjectorTest, can_fetch_object_with_no_dependencies)
{
	AddA();

	myDependencyInjector.CreateAll();

	auto result = myDependencyInjector.Get<TestMockWithNoDependencies>();

	ASSERT_TRUE(result);

	TestMockWithNoDependencies& testMock = static_cast<std::reference_wrapper<TestMockWithNoDependencies>>(result);

	EXPECT_EQ(&myMockA, &testMock);
}

TEST_F(DependencyInjectorTest, can_fetch_object_with_dependencies)
{
	AddA();
	AddB();
	AddC();

	EXPECT_CALL(myMockC, Construct(Eq(Comparator(myMockA)), Eq(Comparator(myMockB))));

	auto result = myDependencyInjector.Get<TestMockWithTwoDependencies>();

	ASSERT_TRUE(result);

	TestMockWithTwoDependencies& testMock = static_cast<std::reference_wrapper<TestMockWithTwoDependencies>>(result);

	EXPECT_EQ(&myMockC, &testMock);
}

TEST_F(DependencyInjectorTest, returns_error_when_type_is_not_added)
{
	const auto result = myDependencyInjector.Get<TestMockWithNoDependencies>();

	ASSERT_FALSE(result);

	EXPECT_EQ(result, DependencyInjector::ECreateError::MissingDependency);
}

TEST_F(DependencyInjectorTest, returns_error_when_dependency_is_not_added)
{
	AddA();
	AddC();

	const auto result = myDependencyInjector.Get<TestMockWithTwoDependencies>();

	ASSERT_FALSE(result);

	EXPECT_EQ(result, DependencyInjector::ECreateError::MissingDependency);
}

TEST_F(DependencyInjectorTest, returns_error_when_dependency_is_not_added_during_createall)
{
	AddA();
	AddC();

	const auto result = myDependencyInjector.CreateAll();

	ASSERT_FALSE(result);

	EXPECT_EQ(result, DependencyInjector::ECreateError::MissingDependency);
}
