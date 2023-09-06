#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/SystemCounter.h"

MATCHER_P(SystemCounterInfoHasName, aTypeName, "Matches info type names")
{
	return ExplainMatchResult(
		AllOf(
			Field("TypeName",		&SystemCounter::SSystemInfo::TypeName,		aTypeName)),
		arg, result_listener);
}
MATCHER_P(SystemCounterInfoIsGeneration, aGeneration, "Matches info generations")
{
	return ExplainMatchResult(
		AllOf(
			Field("Generation", &SystemCounter::SSystemInfo::Generation, aGeneration)),
		arg, result_listener);
}
MATCHER_P(SystemCounterInfoHasDependencies, aList, "Matches info dependency ids")
{
	return ExplainMatchResult(
		AllOf(
			Field("Dependencies",	&SystemCounter::SSystemInfo::Dependencies,	UnorderedElementsAreArray(aList))),
		arg, result_listener);
}

TEST(SystemCounterTests, systems_return_empty_range_when_no_system_is_added)
{
	SystemCounter systemCounter;

	EXPECT_TRUE(systemCounter.Systems().empty());
}

TEST(SystemCounterTests, info_returned_by_systems_include_all_ids_and_type_names)
{
	static constexpr auto dummyTypeNameA = "System Type A";
	static constexpr auto dummyTypeNameB = "System Type B";
	static constexpr auto dummyTypeNameC = "System Type C";

	static constexpr SystemId idA = *Guid::Parse("1fec188d-f36c-4909-b8d5-328166a66125");
	static constexpr SystemId idB = *Guid::Parse("fa57e320-91fe-4a59-8550-263f20ee299e");
	static constexpr SystemId idC = *Guid::Parse("0ce4347b-358d-472e-bba5-12535aa33a85");

	SystemCounter systemCounter;

	systemCounter.AddSystem(idA, dummyTypeNameA);
	systemCounter.AddSystem(idB, dummyTypeNameB);
	systemCounter.AddSystem(idC, dummyTypeNameC);

	EXPECT_THAT(systemCounter.Systems(),
		UnorderedElementsAre(
			Pair(idA, SystemCounterInfoHasName(dummyTypeNameA)),
			Pair(idB, SystemCounterInfoHasName(dummyTypeNameB)),
			Pair(idC, SystemCounterInfoHasName(dummyTypeNameC))));
}

TEST(SystemCounterTests, removed_system_is_not_listed)
{
	static constexpr auto dummyTypeNameA = "System Type A";
	static constexpr auto dummyTypeNameB = "System Type B";
	static constexpr auto dummyTypeNameC = "System Type C";

	static constexpr SystemId idA = *Guid::Parse("1fec188d-f36c-4909-b8d5-328166a66125");
	static constexpr SystemId idB = *Guid::Parse("fa57e320-91fe-4a59-8550-263f20ee299e");
	static constexpr SystemId idC = *Guid::Parse("0ce4347b-358d-472e-bba5-12535aa33a85");

	SystemCounter systemCounter;

	systemCounter.AddSystem(idA, dummyTypeNameA);
	systemCounter.AddSystem(idB, dummyTypeNameB);
	systemCounter.AddSystem(idC, dummyTypeNameC);

	systemCounter.RemoveSystem(idB);

	EXPECT_THAT(systemCounter.Systems(),
		UnorderedElementsAre(
			Pair(idA, SystemCounterInfoHasName(dummyTypeNameA)),
			Pair(idC, SystemCounterInfoHasName(dummyTypeNameC))));
}

TEST(SystemCounterTests, info_returned_by_systems_include_all_added_dependencies)
{
	static constexpr auto dummyTypeName = "System Type";

	static constexpr SystemId idA = *Guid::Parse("1fec188d-f36c-4909-b8d5-328166a66125");
	static constexpr SystemId idB = *Guid::Parse("fa57e320-91fe-4a59-8550-263f20ee299e");
	static constexpr SystemId idC = *Guid::Parse("0ce4347b-358d-472e-bba5-12535aa33a85");

	SystemCounter systemCounter;

	systemCounter.AddSystem(idA, dummyTypeName);
	systemCounter.AddSystem(idB, dummyTypeName);

	systemCounter.AddSystemDependency(idA, idC);
	systemCounter.AddSystemDependency(idB, idA);
	systemCounter.AddSystemDependency(idB, idC);

	EXPECT_THAT(systemCounter.Systems(),
		UnorderedElementsAre(
			Pair(idA, SystemCounterInfoHasDependencies(std::vector{ idC })),
			Pair(idB, SystemCounterInfoHasDependencies(std::vector{ idA, idC }))));
}

TEST(SystemCounterTests, info_returned_by_systems_have_generation_set_to_zero_when_not_increased)
{
	static constexpr u32 expectedGeneration = 0;

	static constexpr auto dummyTypeName = "System Type";

	static constexpr SystemId idA = *Guid::Parse("1fec188d-f36c-4909-b8d5-328166a66125");
	static constexpr SystemId idB = *Guid::Parse("fa57e320-91fe-4a59-8550-263f20ee299e");
	static constexpr SystemId idC = *Guid::Parse("0ce4347b-358d-472e-bba5-12535aa33a85");

	SystemCounter systemCounter;

	systemCounter.AddSystem(idA, dummyTypeName);
	systemCounter.AddSystem(idB, dummyTypeName);
	systemCounter.AddSystem(idC, dummyTypeName);

	EXPECT_THAT(systemCounter.Systems(),
		UnorderedElementsAre(
			Pair(idA, SystemCounterInfoIsGeneration(expectedGeneration)),
			Pair(idB, SystemCounterInfoIsGeneration(expectedGeneration)),
			Pair(idC, SystemCounterInfoIsGeneration(expectedGeneration))));
}

TEST(SystemCounterTests, increase_generation_increases_the_generation_value_for_subsequent_added_systems)
{
	static constexpr u32 expectedGenerationA = 0;
	static constexpr u32 expectedGenerationB = 1;
	static constexpr u32 expectedGenerationC = 3;
	
	static constexpr auto dummyTypeName = "System Type";

	static constexpr SystemId idA = *Guid::Parse("1fec188d-f36c-4909-b8d5-328166a66125");
	static constexpr SystemId idB = *Guid::Parse("fa57e320-91fe-4a59-8550-263f20ee299e");
	static constexpr SystemId idC = *Guid::Parse("0ce4347b-358d-472e-bba5-12535aa33a85");

	SystemCounter systemCounter;

	systemCounter.AddSystem(idA, dummyTypeName);

	systemCounter.IncreaseGeneration();

	systemCounter.AddSystem(idB, dummyTypeName);

	systemCounter.IncreaseGeneration();
	systemCounter.IncreaseGeneration();

	systemCounter.AddSystem(idC, dummyTypeName);

	EXPECT_THAT(systemCounter.Systems(),
		UnorderedElementsAre(
			Pair(idA, SystemCounterInfoIsGeneration(expectedGenerationA)),
			Pair(idB, SystemCounterInfoIsGeneration(expectedGenerationB)),
			Pair(idC, SystemCounterInfoIsGeneration(expectedGenerationC))));
}
