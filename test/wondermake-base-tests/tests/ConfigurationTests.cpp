#include "wondermake-tests-common/GTestInclude.h"

#include "ConfigurationTests.h"

#include "wondermake-base/Configuration.h"

constexpr EConfigGroup locDummyGroup = EConfigGroup::Application;

using namespace MemoryUnitLiterals;

using UMemoryUnit = MemoryUnit<EMemoryRatio::B, u32>;
using IMemoryUnit = MemoryUnit<EMemoryRatio::B, i32>;

TEST(ConfigurationTests, getting_unset_ids_returns_default_value)
{
	Configuration configuration;

	const std::string dummyId = "dummy_id";

	EXPECT_EQ(configuration.Get<bool>(dummyId, false), false);
	EXPECT_EQ(configuration.Get<bool>(dummyId, true), true);

	EXPECT_EQ(configuration.Get<u8>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<u8>(dummyId, std::numeric_limits<u8>::max()), std::numeric_limits<u8>::max());

	EXPECT_EQ(configuration.Get<u16>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<u16>(dummyId, std::numeric_limits<u16>::max()), std::numeric_limits<u16>::max());

	EXPECT_EQ(configuration.Get<u32>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<u32>(dummyId, std::numeric_limits<u32>::max()), std::numeric_limits<u32>::max());

	EXPECT_EQ(configuration.Get<u64>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<u64>(dummyId, std::numeric_limits<u64>::max()), std::numeric_limits<u64>::max());

	EXPECT_EQ(configuration.Get<i8>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<i8>(dummyId, std::numeric_limits<i8>::min()), std::numeric_limits<i8>::min());
	EXPECT_EQ(configuration.Get<i8>(dummyId, std::numeric_limits<i8>::max()), std::numeric_limits<i8>::max());

	EXPECT_EQ(configuration.Get<i16>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<i16>(dummyId, std::numeric_limits<i16>::min()), std::numeric_limits<i16>::min());
	EXPECT_EQ(configuration.Get<i16>(dummyId, std::numeric_limits<i16>::max()), std::numeric_limits<i16>::max());

	EXPECT_EQ(configuration.Get<i32>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<i32>(dummyId, std::numeric_limits<i32>::min()), std::numeric_limits<i32>::min());
	EXPECT_EQ(configuration.Get<i32>(dummyId, std::numeric_limits<i32>::max()), std::numeric_limits<i32>::max());

	EXPECT_EQ(configuration.Get<i64>(dummyId, 0), 0);
	EXPECT_EQ(configuration.Get<i64>(dummyId, std::numeric_limits<i64>::min()), std::numeric_limits<i64>::min());
	EXPECT_EQ(configuration.Get<i64>(dummyId, std::numeric_limits<i64>::max()), std::numeric_limits<i64>::max());

	EXPECT_EQ(configuration.Get<std::string>(dummyId, ""), "");
	EXPECT_EQ(configuration.Get<std::string>(dummyId, "dummy_value"), "dummy_value");

	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, ETestEnumI32::Zero), ETestEnumI32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, ETestEnumI32::Min), ETestEnumI32::Min);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, ETestEnumI32::Max), ETestEnumI32::Max);

	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyId, ETestEnumU32::Zero), ETestEnumU32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyId, ETestEnumU32::Max), ETestEnumU32::Max);

	EXPECT_EQ(configuration.Get<UMemoryUnit>(dummyId, 0_B), 0_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(dummyId, UMemoryUnit(std::numeric_limits<u32>::max())), UMemoryUnit(std::numeric_limits<u32>::max()));
	
	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyId, 0_B), 0_B);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyId, IMemoryUnit(std::numeric_limits<i32>::min())), IMemoryUnit(std::numeric_limits<i32>::min()));
	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyId, IMemoryUnit(std::numeric_limits<i32>::max())), IMemoryUnit(std::numeric_limits<i32>::max()));
}

TEST(ConfigurationTests, type_bool_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdFalse	= "dummy_id_false";
	const std::string dummyIdTrue	= "dummy_id_true";

	configuration.Set<bool>(dummyIdFalse, false, locDummyGroup);
	configuration.Set<bool>(dummyIdTrue, true, locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdFalse,	true),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdFalse,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdFalse,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdFalse,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdFalse,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdFalse,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdFalse,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdTrue,	false),				true);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdTrue,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdTrue,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdTrue,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdTrue,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdTrue,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdTrue,	1_B),				1_B);
}

TEST(ConfigurationTests, type_u8_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<u8>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<u8>(dummyIdMax, std::numeric_limits<u8>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					std::numeric_limits<u8>::max());
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_u16_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<u16>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<u16>(dummyIdMax, std::numeric_limits<u16>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					std::numeric_limits<u16>::max());
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_u32_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<u32>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<u32>(dummyIdMax, std::numeric_limits<u32>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					std::numeric_limits<u32>::max());
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_u64_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<u64>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<u64>(dummyIdMax, std::numeric_limits<u64>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					std::numeric_limits<u64>::max());
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_i8_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<i8>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<i8>(dummyIdMin, std::numeric_limits<i8>::min(), locDummyGroup);
	configuration.Set<i8>(dummyIdMax, std::numeric_limits<i8>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMin,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMin,		1),					std::numeric_limits<i8>::min());
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMin,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMin,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMin,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					std::numeric_limits<i8>::max());
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_i16_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<i16>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<i16>(dummyIdMin, std::numeric_limits<i16>::min(), locDummyGroup);
	configuration.Set<i16>(dummyIdMax, std::numeric_limits<i16>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMin,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMin,		1),					std::numeric_limits<i16>::min());
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMin,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMin,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMin,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					std::numeric_limits<i16>::max());
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_i32_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<i32>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<i32>(dummyIdMin, std::numeric_limits<i32>::min(), locDummyGroup);
	configuration.Set<i32>(dummyIdMax, std::numeric_limits<i32>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMin,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMin,		1),					std::numeric_limits<i32>::min());
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMin,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMin,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMin,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					std::numeric_limits<i32>::max());
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_i64_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<i64>(dummyIdZero, 0, locDummyGroup);
	configuration.Set<i64>(dummyIdMin, std::numeric_limits<i64>::min(), locDummyGroup);
	configuration.Set<i64>(dummyIdMax, std::numeric_limits<i64>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					0);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMin,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMin,		1),					std::numeric_limits<i64>::min());
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMin,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMin,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMin,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					std::numeric_limits<i64>::max());
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_string_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdEmpty = "dummy_id_empty";
	const std::string dummyIdNonEmpty = "dummy_id_non_empty";

	const std::string dummyValue = "dummy_value";

	configuration.Set<std::string>(dummyIdEmpty, "", locDummyGroup);
	configuration.Set<std::string>(dummyIdNonEmpty, dummyValue, locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdEmpty,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdEmpty,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdEmpty,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdEmpty,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdEmpty,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdEmpty,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdEmpty,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdNonEmpty,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdNonEmpty,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdNonEmpty,	""),				dummyValue);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdNonEmpty,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdNonEmpty,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdNonEmpty,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdNonEmpty,	1_B),				1_B);
}

TEST(ConfigurationTests, type_enum_i32_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<ETestEnumI32>(dummyIdZero, ETestEnumI32::Zero, locDummyGroup);
	configuration.Set<ETestEnumI32>(dummyIdMin, ETestEnumI32::Min, locDummyGroup);
	configuration.Set<ETestEnumI32>(dummyIdMax, ETestEnumI32::Max, locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMin,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMin,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMin,		ETestEnumI32::One),	ETestEnumI32::Min);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMin,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::Max);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_enum_u32_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<ETestEnumU32>(dummyIdZero, ETestEnumU32::Zero, locDummyGroup);
	configuration.Set<ETestEnumU32>(dummyIdMax, ETestEnumU32::Max, locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::Zero);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::Max);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_memoryunit_i32_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<IMemoryUnit>(dummyIdZero, 0_B, locDummyGroup);
	configuration.Set<IMemoryUnit>(dummyIdMin, std::numeric_limits<i32>::min(), locDummyGroup);
	configuration.Set<IMemoryUnit>(dummyIdMax, std::numeric_limits<i32>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				0_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMin,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMin,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMin,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMin,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMin,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMin,		1_B),				IMemoryUnit(std::numeric_limits<i32>::min()));
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMin,		1_B),				1_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				IMemoryUnit(std::numeric_limits<i32>::max()));
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				1_B);
}

TEST(ConfigurationTests, type_memoryunit_u32_can_be_set_and_get)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";

	configuration.Set<UMemoryUnit>(dummyIdZero, 0_B, locDummyGroup);
	configuration.Set<UMemoryUnit>(dummyIdMax, std::numeric_limits<u32>::max(), locDummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdZero,	false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdZero,	1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdZero,	""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdZero,	ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdZero,	ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdZero,	1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdZero,	1_B),				0_B);
	
	EXPECT_EQ(configuration.Get<bool>(			dummyIdMax,		false),				false);
	EXPECT_EQ(configuration.Get<u8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<u64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i8>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i16>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i32>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<i64>(			dummyIdMax,		1),					1);
	EXPECT_EQ(configuration.Get<std::string>(	dummyIdMax,		""),				"");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	dummyIdMax,		ETestEnumI32::One),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	dummyIdMax,		ETestEnumU32::One),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	dummyIdMax,		1_B),				1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	dummyIdMax,		1_B),				UMemoryUnit(std::numeric_limits<u32>::max()));
}

TEST(ConfigurationTests, values_can_only_be_set_once)
{
	Configuration configuration;

	configuration.Set<bool>(			"dummy_id_bool",			true,					locDummyGroup);
	configuration.Set<u8>(				"dummy_id_u8",				1,						locDummyGroup);
	configuration.Set<u16>(				"dummy_id_u16",				1,						locDummyGroup);
	configuration.Set<u32>(				"dummy_id_u32",				1,						locDummyGroup);
	configuration.Set<u64>(				"dummy_id_u64",				1,						locDummyGroup);
	configuration.Set<i8>(				"dummy_id_i8",				1,						locDummyGroup);
	configuration.Set<i16>(				"dummy_id_i16",				1,						locDummyGroup);
	configuration.Set<i32>(				"dummy_id_i32",				1,						locDummyGroup);
	configuration.Set<i64>(				"dummy_id_i64",				1,						locDummyGroup);
	configuration.Set<std::string>(		"dummy_id_string",			"dummy_value_first",	locDummyGroup);
	configuration.Set<ETestEnumI32>(	"dummy_id_enum_i32",		ETestEnumI32::One,		locDummyGroup);
	configuration.Set<ETestEnumU32>(	"dummy_id_enum_u32",		ETestEnumU32::One,		locDummyGroup);
	configuration.Set<IMemoryUnit>(		"dummy_id_memoryunit_i32",	1_B,					locDummyGroup);
	configuration.Set<UMemoryUnit>(		"dummy_id_memoryunit_u32",	1_B,					locDummyGroup);
	
	configuration.Set<bool>(			"dummy_id_bool",			false,					locDummyGroup);
	configuration.Set<u8>(				"dummy_id_u8",				2,						locDummyGroup);
	configuration.Set<u16>(				"dummy_id_u16",				2,						locDummyGroup);
	configuration.Set<u32>(				"dummy_id_u32",				2,						locDummyGroup);
	configuration.Set<u64>(				"dummy_id_u64",				2,						locDummyGroup);
	configuration.Set<i8>(				"dummy_id_i8",				2,						locDummyGroup);
	configuration.Set<i16>(				"dummy_id_i16",				2,						locDummyGroup);
	configuration.Set<i32>(				"dummy_id_i32",				2,						locDummyGroup);
	configuration.Set<i64>(				"dummy_id_i64",				2,						locDummyGroup);
	configuration.Set<std::string>(		"dummy_id_string",			"dummy_value_second",	locDummyGroup);
	configuration.Set<ETestEnumI32>(	"dummy_id_enum_i32",		ETestEnumI32::Zero,		locDummyGroup);
	configuration.Set<ETestEnumU32>(	"dummy_id_enum_u32",		ETestEnumU32::Zero,		locDummyGroup);
	configuration.Set<IMemoryUnit>(		"dummy_id_memoryunit_i32",	0_B,					locDummyGroup);
	configuration.Set<UMemoryUnit>(		"dummy_id_memoryunit_u32",	0_B,					locDummyGroup);

	EXPECT_EQ(configuration.Get<bool>(			"dummy_id_bool",			false),					true);
	EXPECT_EQ(configuration.Get<u8>(			"dummy_id_u8",				0),						1);
	EXPECT_EQ(configuration.Get<u16>(			"dummy_id_u16",				0),						1);
	EXPECT_EQ(configuration.Get<u32>(			"dummy_id_u32",				0),						1);
	EXPECT_EQ(configuration.Get<u64>(			"dummy_id_u64",				0),						1);
	EXPECT_EQ(configuration.Get<i8>(			"dummy_id_i8",				0),						1);
	EXPECT_EQ(configuration.Get<i16>(			"dummy_id_i16",				0),						1);
	EXPECT_EQ(configuration.Get<i32>(			"dummy_id_i32",				0),						1);
	EXPECT_EQ(configuration.Get<i64>(			"dummy_id_i64",				0),						1);
	EXPECT_EQ(configuration.Get<std::string>(	"dummy_id_string",			"dummy_default"),		"dummy_value_first");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	"dummy_id_enum_i32",		ETestEnumI32::Zero),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	"dummy_id_enum_u32",		ETestEnumU32::Zero),	ETestEnumU32::One);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(	"dummy_id_memoryunit_i32",	0_B),					1_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(	"dummy_id_memoryunit_u32",	0_B),					1_B);
}

TEST(ConfigurationTests, type_bool_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyId		= "dummy_id";
	constexpr bool dummyValue		= false;
	constexpr bool dummyDefault		= false;
	constexpr bool dummyOverride	= true;

	configuration.Set<bool>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<bool>(dummyId, dummyOverride);

	EXPECT_EQ(configuration.Get<bool>(dummyId, dummyDefault), dummyOverride);
}

TEST(ConfigurationTests, type_u8_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr u8 dummyValue			= 0;
	constexpr u8 dummyDefault		= 1;

	configuration.Set<u8>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<u8>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<u8>(dummyIdZero, 0);
	configuration.SetOverride<u8>(dummyIdMax, std::numeric_limits<u8>::max());

	EXPECT_EQ(configuration.Get<u8>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<u8>(dummyIdMax, dummyDefault), std::numeric_limits<u8>::max());
}

TEST(ConfigurationTests, type_u16_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr u16 dummyValue		= 0;
	constexpr u16 dummyDefault		= 1;

	configuration.Set<u16>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<u16>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<u16>(dummyIdZero, 0);
	configuration.SetOverride<u16>(dummyIdMax, std::numeric_limits<u16>::max());

	EXPECT_EQ(configuration.Get<u16>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<u16>(dummyIdMax, dummyDefault), std::numeric_limits<u16>::max());
}

TEST(ConfigurationTests, type_u32_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr u32 dummyValue		= 0;
	constexpr u32 dummyDefault		= 1;

	configuration.Set<u32>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<u32>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<u32>(dummyIdZero, 0);
	configuration.SetOverride<u32>(dummyIdMax, std::numeric_limits<u32>::max());

	EXPECT_EQ(configuration.Get<u32>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<u32>(dummyIdMax, dummyDefault), std::numeric_limits<u32>::max());
}

TEST(ConfigurationTests, type_u64_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr u64 dummyValue		= 0;
	constexpr u64 dummyDefault		= 1;

	configuration.Set<u64>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<u64>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<u64>(dummyIdZero, 0);
	configuration.SetOverride<u64>(dummyIdMax, std::numeric_limits<u64>::max());

	EXPECT_EQ(configuration.Get<u64>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<u64>(dummyIdMax, dummyDefault), std::numeric_limits<u64>::max());
}

TEST(ConfigurationTests, type_i8_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr i8 dummyValue			= 0;
	constexpr i8 dummyDefault		= 1;

	configuration.Set<i8>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<i8>(dummyIdMin, dummyValue, locDummyGroup);
	configuration.Set<i8>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<i8>(dummyIdZero, 0);
	configuration.SetOverride<i8>(dummyIdMin, std::numeric_limits<i8>::min());
	configuration.SetOverride<i8>(dummyIdMax, std::numeric_limits<i8>::max());

	EXPECT_EQ(configuration.Get<i8>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<i8>(dummyIdMin, dummyDefault), std::numeric_limits<i8>::min());
	EXPECT_EQ(configuration.Get<i8>(dummyIdMax, dummyDefault), std::numeric_limits<i8>::max());
}

TEST(ConfigurationTests, type_i16_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr i16 dummyValue		= 0;
	constexpr i16 dummyDefault		= 1;

	configuration.Set<i16>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<i16>(dummyIdMin, dummyValue, locDummyGroup);
	configuration.Set<i16>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<i16>(dummyIdZero, 0);
	configuration.SetOverride<i16>(dummyIdMin, std::numeric_limits<i16>::min());
	configuration.SetOverride<i16>(dummyIdMax, std::numeric_limits<i16>::max());

	EXPECT_EQ(configuration.Get<i16>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<i16>(dummyIdMin, dummyDefault), std::numeric_limits<i16>::min());
	EXPECT_EQ(configuration.Get<i16>(dummyIdMax, dummyDefault), std::numeric_limits<i16>::max());
}

TEST(ConfigurationTests, type_i32_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr i32 dummyValue		= 0;
	constexpr i32 dummyDefault		= 1;

	configuration.Set<i32>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<i32>(dummyIdMin, dummyValue, locDummyGroup);
	configuration.Set<i32>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<i32>(dummyIdZero, 0);
	configuration.SetOverride<i32>(dummyIdMin, std::numeric_limits<i32>::min());
	configuration.SetOverride<i32>(dummyIdMax, std::numeric_limits<i32>::max());

	EXPECT_EQ(configuration.Get<i32>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<i32>(dummyIdMin, dummyDefault), std::numeric_limits<i32>::min());
	EXPECT_EQ(configuration.Get<i32>(dummyIdMax, dummyDefault), std::numeric_limits<i32>::max());
}

TEST(ConfigurationTests, type_i64_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero	= "dummy_id_zero";
	const std::string dummyIdMin	= "dummy_id_min";
	const std::string dummyIdMax	= "dummy_id_max";
	constexpr i64 dummyValue		= 0;
	constexpr i64 dummyDefault		= 1;

	configuration.Set<i64>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<i64>(dummyIdMin, dummyValue, locDummyGroup);
	configuration.Set<i64>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<i64>(dummyIdZero, 0);
	configuration.SetOverride<i64>(dummyIdMin, std::numeric_limits<i64>::min());
	configuration.SetOverride<i64>(dummyIdMax, std::numeric_limits<i64>::max());

	EXPECT_EQ(configuration.Get<i64>(dummyIdZero, dummyDefault), 0);
	EXPECT_EQ(configuration.Get<i64>(dummyIdMin, dummyDefault), std::numeric_limits<i64>::min());
	EXPECT_EQ(configuration.Get<i64>(dummyIdMax, dummyDefault), std::numeric_limits<i64>::max());
}

TEST(ConfigurationTests, type_string_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdEmpty		= "dummy_id_empty";
	const std::string dummyIdNonEmpty	= "dummy_id_non_empty";
	const std::string dummyValue		= "dummy_value";
	const std::string dummyDefault		= "dummy_default";
	const std::string dummyOverride		= "dummy_override";

	configuration.Set<std::string>(dummyIdEmpty, dummyValue, locDummyGroup);
	configuration.Set<std::string>(dummyIdNonEmpty, dummyValue, locDummyGroup);

	configuration.SetOverride<std::string>(dummyIdEmpty, "");
	configuration.SetOverride<std::string>(dummyIdNonEmpty, dummyOverride);

	EXPECT_EQ(configuration.Get<std::string>(dummyIdEmpty, dummyDefault), "");
	EXPECT_EQ(configuration.Get<std::string>(dummyIdNonEmpty, dummyDefault), dummyOverride);
}

TEST(ConfigurationTests, type_enum_i32_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero		= "dummy_id_zero";
	const std::string dummyIdMin		= "dummy_id_min";
	const std::string dummyIdMax		= "dummy_id_max";
	constexpr ETestEnumI32 dummyValue	= ETestEnumI32::One;
	constexpr ETestEnumI32 dummyDefault	= ETestEnumI32::One;

	configuration.Set<ETestEnumI32>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<ETestEnumI32>(dummyIdMin, dummyValue, locDummyGroup);
	configuration.Set<ETestEnumI32>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<ETestEnumI32>(dummyIdZero, ETestEnumI32::Zero);
	configuration.SetOverride<ETestEnumI32>(dummyIdMin, ETestEnumI32::Min);
	configuration.SetOverride<ETestEnumI32>(dummyIdMax, ETestEnumI32::Max);

	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyIdZero, dummyDefault), ETestEnumI32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyIdMin, dummyDefault), ETestEnumI32::Min);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyIdMax, dummyDefault), ETestEnumI32::Max);
}

TEST(ConfigurationTests, type_enum_u32_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero		= "dummy_id_zero";
	const std::string dummyIdMax		= "dummy_id_max";
	constexpr ETestEnumU32 dummyValue	= ETestEnumU32::One;
	constexpr ETestEnumU32 dummyDefault	= ETestEnumU32::One;

	configuration.Set<ETestEnumU32>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<ETestEnumU32>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<ETestEnumU32>(dummyIdZero, ETestEnumU32::Zero);
	configuration.SetOverride<ETestEnumU32>(dummyIdMax, ETestEnumU32::Max);

	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyIdZero, dummyDefault), ETestEnumU32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyIdMax, dummyDefault), ETestEnumU32::Max);
}

TEST(ConfigurationTests, type_memoryunit_i32_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero		= "dummy_id_zero";
	const std::string dummyIdMin		= "dummy_id_min";
	const std::string dummyIdMax		= "dummy_id_max";
	constexpr IMemoryUnit dummyValue	= 1_B;
	constexpr IMemoryUnit dummyDefault	= 1_B;

	configuration.Set<IMemoryUnit>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<IMemoryUnit>(dummyIdMin, dummyValue, locDummyGroup);
	configuration.Set<IMemoryUnit>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<IMemoryUnit>(dummyIdZero, 0_B);
	configuration.SetOverride<IMemoryUnit>(dummyIdMin, IMemoryUnit(std::numeric_limits<i32>::min()));
	configuration.SetOverride<IMemoryUnit>(dummyIdMax, IMemoryUnit(std::numeric_limits<i32>::max()));

	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyIdZero, dummyDefault), 0_B);
	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyIdMin, dummyDefault), IMemoryUnit(std::numeric_limits<i32>::min()));
	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyIdMax, dummyDefault), IMemoryUnit(std::numeric_limits<i32>::max()));
}

TEST(ConfigurationTests, type_memoryunit_u32_get_returns_overridden_value)
{
	Configuration configuration;

	const std::string dummyIdZero		= "dummy_id_zero";
	const std::string dummyIdMax		= "dummy_id_max";
	constexpr UMemoryUnit dummyValue	= 1_B;
	constexpr UMemoryUnit dummyDefault	= 1_B;

	configuration.Set<UMemoryUnit>(dummyIdZero, dummyValue, locDummyGroup);
	configuration.Set<UMemoryUnit>(dummyIdMax, dummyValue, locDummyGroup);

	configuration.SetOverride<UMemoryUnit>(dummyIdZero, 0_B);
	configuration.SetOverride<UMemoryUnit>(dummyIdMax, UMemoryUnit(std::numeric_limits<u32>::max()));

	EXPECT_EQ(configuration.Get<UMemoryUnit>(dummyIdZero, dummyDefault), 0_B);
	EXPECT_EQ(configuration.Get<UMemoryUnit>(dummyIdMax, dummyDefault), UMemoryUnit(std::numeric_limits<u32>::max()));
}

TEST(ConfigurationTests, type_bool_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId		= "dummy_id";
	constexpr bool dummyValue		= true;
	constexpr bool dummyDefault		= false;
	constexpr bool dummyOverride	= false;

	configuration.Set<bool>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<bool>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<bool>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_u8_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr u8 dummyValue		= 1;
	constexpr u8 dummyDefault	= 2;
	constexpr u8 dummyOverride	= 3;

	configuration.Set<u8>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<u8>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<u8>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_u16_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr u16 dummyValue	= 1;
	constexpr u16 dummyDefault	= 2;
	constexpr u16 dummyOverride	= 3;

	configuration.Set<u16>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<u16>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<u16>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_u32_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr u32 dummyValue	= 1;
	constexpr u32 dummyDefault	= 2;
	constexpr u32 dummyOverride	= 3;

	configuration.Set<u32>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<u32>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<u32>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_u64_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr u64 dummyValue	= 1;
	constexpr u64 dummyDefault	= 2;
	constexpr u64 dummyOverride	= 3;

	configuration.Set<u64>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<u64>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<u64>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_i8_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr i8 dummyValue		= 1;
	constexpr i8 dummyDefault	= 2;
	constexpr i8 dummyOverride	= 3;

	configuration.Set<i8>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<i8>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<i8>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_i16_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr i16 dummyValue	= 1;
	constexpr i16 dummyDefault	= 2;
	constexpr i16 dummyOverride	= 3;

	configuration.Set<i16>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<i16>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<i16>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_i32_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr i32 dummyValue	= 1;
	constexpr i32 dummyDefault	= 2;
	constexpr i32 dummyOverride	= 3;

	configuration.Set<i32>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<i32>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<i32>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_i64_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId	= "dummy_id";
	constexpr i64 dummyValue	= 1;
	constexpr i64 dummyDefault	= 2;
	constexpr i64 dummyOverride	= 3;

	configuration.Set<i64>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<i64>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<i64>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_string_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId		= "dummy_id";
	const std::string dummyValue	= "dummy_value";
	const std::string dummyDefault	= "dummy_default";
	const std::string dummyOverride	= "dummy_override";

	configuration.Set<std::string>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<std::string>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<std::string>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_enum_i32_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId = "dummy_id";
	constexpr ETestEnumI32 dummyValue = ETestEnumI32::One;
	constexpr ETestEnumI32 dummyDefault = ETestEnumI32::Zero;
	constexpr ETestEnumI32 dummyOverride = ETestEnumI32::Zero;

	configuration.Set<ETestEnumI32>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<ETestEnumI32>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_enum_u32_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId = "dummy_id";
	constexpr ETestEnumU32 dummyValue = ETestEnumU32::One;
	constexpr ETestEnumU32 dummyDefault = ETestEnumU32::Zero;
	constexpr ETestEnumU32 dummyOverride = ETestEnumU32::Zero;

	configuration.Set<ETestEnumU32>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<ETestEnumU32>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_memoryunit_i32_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId = "dummy_id";
	static constexpr IMemoryUnit dummyValue		= 1_B;
	static constexpr IMemoryUnit dummyDefault	= 2_B;
	static constexpr IMemoryUnit dummyOverride	= 3_B;
	
	configuration.Set<IMemoryUnit>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<IMemoryUnit>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<IMemoryUnit>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, type_memoryunit_u32_reset_overridden_value)
{
	Configuration configuration;

	const std::string dummyId = "dummy_id";
	static constexpr UMemoryUnit dummyValue		= 1_B;
	static constexpr UMemoryUnit dummyDefault	= 2_B;
	static constexpr UMemoryUnit dummyOverride	= 3_B;

	configuration.Set<UMemoryUnit>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<UMemoryUnit>(dummyId, dummyOverride);

	configuration.ResetOverride(dummyId);

	EXPECT_EQ(configuration.Get<UMemoryUnit>(dummyId, dummyDefault), dummyValue);
}

TEST(ConfigurationTests, set_memory_ratio_sets_ratio)
{
	static constexpr auto dummyId = "dummy_id";
	static constexpr auto getRatio = [](const auto& aConfig)
	{
		return aConfig.MemoryRatio;
	};

	Configuration configuration;

	configuration.Set<u32>(dummyId, 0, locDummyGroup);
	
	{
		configuration.SetMemoryRatio(dummyId, EMemoryRatio::B);

		auto configs = configuration.GetConfigs();

		auto ratio = std::visit(getRatio, configs[dummyId]);

		ASSERT_TRUE(ratio);

		EXPECT_EQ(*ratio, EMemoryRatio::B);
	}

	{
		configuration.SetMemoryRatio(std::string_view(dummyId), EMemoryRatio::KB);

		auto configs = configuration.GetConfigs();

		auto ratio = std::visit(getRatio, configs[dummyId]);

		ASSERT_TRUE(ratio);

		EXPECT_EQ(*ratio, EMemoryRatio::KB);
	}

	{
		configuration.SetMemoryRatio(std::string(dummyId), EMemoryRatio::KiB);

		auto configs = configuration.GetConfigs();

		auto ratio = std::visit(getRatio, configs[dummyId]);

		ASSERT_TRUE(ratio);

		EXPECT_EQ(*ratio, EMemoryRatio::KiB);
	}
}

TEST(ConfigurationTests, configuration_are_equal_if_they_contain_same_values_and_overrides)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id", 123, EConfigGroup::Application);
	configurationL.Set<u32>("test_id_restricted", 234, EConfigGroup::Application, Configuration::AllowedValues<u32>{ { {"FIRST", 1}, {"SECOND", 2}, {"THIRD", 3} } });
	configurationL.Set<std::string>("test_id_text", "Hello", EConfigGroup::Device);
	configurationL.Set<UMemoryUnit>("test_id_memoryunit", 1234_KiB, EConfigGroup::User);

	configurationL.SetOverride<std::string>("test_id_text", "World");

	configurationR.Set<u32>("test_id", 123, EConfigGroup::Application);
	configurationR.Set<u32>("test_id_restricted", 234, EConfigGroup::Application, Configuration::AllowedValues<u32>{ { {"FIRST", 1}, { "SECOND", 2 }, { "THIRD", 3 } } });
	configurationR.Set<std::string>("test_id_text", "Hello", EConfigGroup::Device);
	configurationR.Set<UMemoryUnit>("test_id_memoryunit", 1234_KiB, EConfigGroup::User);

	configurationR.SetOverride<std::string>("test_id_text", "World");

	EXPECT_EQ(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_a_value_is_different)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application);
	configurationR.Set<u32>("test_id_mismatch", 100000000, EConfigGroup::Application);

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_a_override_is_different)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application);
	configurationR.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application);

	configurationL.SetOverride<u32>("test_id_mismatch", 456);
	configurationR.SetOverride<u32>("test_id_mismatch", 1000000);

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_a_config_group_is_different)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application);
	configurationR.Set<u32>("test_id_mismatch", 123, EConfigGroup::Device);

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_a_config_memory_ratio_is_different)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<MemoryUnit<EMemoryRatio::KB, u32>>("test_id_mismatch", 1000_KB, EConfigGroup::Application);
	configurationR.Set<MemoryUnit<EMemoryRatio::KiB, u32>>("test_id_mismatch", 1000_KiB, EConfigGroup::Application);

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_ids_are_not_the_same)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id_mismatch_1", 123, EConfigGroup::Application);
	configurationR.Set<u32>("test_id_mismatch_2", 123, EConfigGroup::Application);

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_one_contains_more_configs)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id", 123, EConfigGroup::Application);
	configurationL.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application);
	configurationR.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application);

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_restricted_lists_have_different_values)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application, Configuration::AllowedValues<u32>{ { {"FIRST", 1}, { "SECOND", 2 }, { "THIRD", 4 } } });
	configurationR.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application, Configuration::AllowedValues<u32>{ { {"FIRST", 1}, { "SECOND", 2 }, { "THIRD", 3 } } });

	EXPECT_NE(configurationL, configurationR);
}

TEST(ConfigurationTests, configuration_are_not_equal_if_restricted_lists_contains_more_values)
{
	Configuration configurationL;
	Configuration configurationR;

	configurationL.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application, Configuration::AllowedValues<u32>{ { {"FIRST", 1}, { "SECOND", 2 }, { "THIRD", 3 } } });
	configurationR.Set<u32>("test_id_mismatch", 123, EConfigGroup::Application, Configuration::AllowedValues<u32>{ { {"FIRST", 1}, { "SECOND", 2 }, { "THIRD", 3 }, { "FOURTH", 4 } } });

	EXPECT_NE(configurationL, configurationR);
}
