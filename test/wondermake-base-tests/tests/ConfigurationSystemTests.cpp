#include "wondermake-tests-common/GTestInclude.h"

#include "ConfigurationTests.h"

#include "wondermake-base/ConfigurationSystem.h"

inline constexpr ProcessId locDummyProcessId(1234);

TEST(ConfigurationSystemTests, getting_unset_ids_returns_default_value)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

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

	EXPECT_EQ(configuration.Get<f32>(dummyId, 0.f), 0.f);
	EXPECT_EQ(configuration.Get<f32>(dummyId, std::numeric_limits<f32>::min()), std::numeric_limits<f32>::min());
	EXPECT_EQ(configuration.Get<f32>(dummyId, std::numeric_limits<f32>::max()), std::numeric_limits<f32>::max());

	EXPECT_EQ(configuration.Get<f64>(dummyId, 0.), 0.);
	EXPECT_EQ(configuration.Get<f64>(dummyId, std::numeric_limits<f64>::min()), std::numeric_limits<f64>::min());
	EXPECT_EQ(configuration.Get<f64>(dummyId, std::numeric_limits<f64>::max()), std::numeric_limits<f64>::max());

	EXPECT_EQ(configuration.Get<std::string>(dummyId, ""), "");
	EXPECT_EQ(configuration.Get<std::string>(dummyId, "dummy_value"), "dummy_value");

	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, ETestEnumI32::Zero), ETestEnumI32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, ETestEnumI32::Min), ETestEnumI32::Min);
	EXPECT_EQ(configuration.Get<ETestEnumI32>(dummyId, ETestEnumI32::Max), ETestEnumI32::Max);

	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyId, ETestEnumU32::Zero), ETestEnumU32::Zero);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(dummyId, ETestEnumU32::Max), ETestEnumU32::Max);
}

TEST(ConfigurationSystemTests, values_can_be_set_and_get)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<bool>(					"dummy_id_bool",		true,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			1,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			1,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			1,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			1,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			1,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			1,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			1,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			1,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			1.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			1.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value_first",	dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::One,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::One,		dummyGroup);
	
	EXPECT_EQ(configuration.Get<bool>(			"dummy_id_bool",		false),					true);
	EXPECT_EQ(configuration.Get<u8>(			"dummy_id_u8",			0),						1);
	EXPECT_EQ(configuration.Get<u16>(			"dummy_id_u16",			0),						1);
	EXPECT_EQ(configuration.Get<u32>(			"dummy_id_u32",			0),						1);
	EXPECT_EQ(configuration.Get<u64>(			"dummy_id_u64",			0),						1);
	EXPECT_EQ(configuration.Get<i8>(			"dummy_id_i8",			0),						1);
	EXPECT_EQ(configuration.Get<i16>(			"dummy_id_i16",			0),						1);
	EXPECT_EQ(configuration.Get<i32>(			"dummy_id_i32",			0),						1);
	EXPECT_EQ(configuration.Get<i64>(			"dummy_id_i64",			0),						1);
	EXPECT_EQ(configuration.Get<f32>(			"dummy_id_f32",			0.f),					1.f);
	EXPECT_EQ(configuration.Get<f64>(			"dummy_id_f64",			0.),					1.);
	EXPECT_EQ(configuration.Get<std::string>(	"dummy_id_string",		"dummy_default"),		"dummy_value_first");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::Zero),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::Zero),	ETestEnumU32::One);
}

TEST(ConfigurationSystemTests, values_can_be_overridden)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<bool>(					"dummy_id_bool",		true,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			1,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			1,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			1,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			1,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			1,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			1,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			1,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			1,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			1.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			1.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value_first",	dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::One,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::One,		dummyGroup);
	
	configuration.SetOverride<bool>(			"dummy_id_bool",		false);
	configuration.SetOverride<u8>(				"dummy_id_u8",			2);
	configuration.SetOverride<u16>(				"dummy_id_u16",			2);
	configuration.SetOverride<u32>(				"dummy_id_u32",			2);
	configuration.SetOverride<u64>(				"dummy_id_u64",			2);
	configuration.SetOverride<i8>(				"dummy_id_i8",			2);
	configuration.SetOverride<i16>(				"dummy_id_i16",			2);
	configuration.SetOverride<i32>(				"dummy_id_i32",			2);
	configuration.SetOverride<i64>(				"dummy_id_i64",			2);
	configuration.SetOverride<f32>(				"dummy_id_f32",			2.f);
	configuration.SetOverride<f64>(				"dummy_id_f64",			2.);
	configuration.SetOverride<std::string>(		"dummy_id_string",		"dummy_value_second");
	configuration.SetOverride<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::Max);
	configuration.SetOverride<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::Max);
	
	EXPECT_EQ(configuration.Get<bool>(			"dummy_id_bool",		true),					false);
	EXPECT_EQ(configuration.Get<u8>(			"dummy_id_u8",			0),						2);
	EXPECT_EQ(configuration.Get<u16>(			"dummy_id_u16",			0),						2);
	EXPECT_EQ(configuration.Get<u32>(			"dummy_id_u32",			0),						2);
	EXPECT_EQ(configuration.Get<u64>(			"dummy_id_u64",			0),						2);
	EXPECT_EQ(configuration.Get<i8>(			"dummy_id_i8",			0),						2);
	EXPECT_EQ(configuration.Get<i16>(			"dummy_id_i16",			0),						2);
	EXPECT_EQ(configuration.Get<i32>(			"dummy_id_i32",			0),						2);
	EXPECT_EQ(configuration.Get<i64>(			"dummy_id_i64",			0),						2);
	EXPECT_EQ(configuration.Get<f32>(			"dummy_id_f32",			0.f),					2.f);
	EXPECT_EQ(configuration.Get<f64>(			"dummy_id_f64",			0.),					2.);
	EXPECT_EQ(configuration.Get<std::string>(	"dummy_id_string",		"dummy_default"),		"dummy_value_second");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::Zero),	ETestEnumI32::Max);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::Zero),	ETestEnumU32::Max);
}

TEST(ConfigurationSystemTests, overrides_can_be_reset)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<bool>(					"dummy_id_bool",		true,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			1,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			1,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			1,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			1,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			1,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			1,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			1,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			1,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			1.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			1.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value_first",	dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::One,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::One,		dummyGroup);
	
	configuration.SetOverride<bool>(			"dummy_id_bool",		false);
	configuration.SetOverride<u8>(				"dummy_id_u8",			2);
	configuration.SetOverride<u16>(				"dummy_id_u16",			2);
	configuration.SetOverride<u32>(				"dummy_id_u32",			2);
	configuration.SetOverride<u64>(				"dummy_id_u64",			2);
	configuration.SetOverride<i8>(				"dummy_id_i8",			2);
	configuration.SetOverride<i16>(				"dummy_id_i16",			2);
	configuration.SetOverride<i32>(				"dummy_id_i32",			2);
	configuration.SetOverride<i64>(				"dummy_id_i64",			2);
	configuration.SetOverride<f32>(				"dummy_id_f32",			2.f);
	configuration.SetOverride<f64>(				"dummy_id_f64",			2.);
	configuration.SetOverride<std::string>(		"dummy_id_string",		"dummy_value_second");
	configuration.SetOverride<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::Max);
	configuration.SetOverride<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::Max);
	
	configuration.ResetOverride(				"dummy_id_bool");
	configuration.ResetOverride(				"dummy_id_u8");
	configuration.ResetOverride(				"dummy_id_u16");
	configuration.ResetOverride(				"dummy_id_u32");
	configuration.ResetOverride(				"dummy_id_u64");
	configuration.ResetOverride(				"dummy_id_i8");
	configuration.ResetOverride(				"dummy_id_i16");
	configuration.ResetOverride(				"dummy_id_i32");
	configuration.ResetOverride(				"dummy_id_i64");
	configuration.ResetOverride(				"dummy_id_f32");
	configuration.ResetOverride(				"dummy_id_f64");
	configuration.ResetOverride(				"dummy_id_string");
	configuration.ResetOverride(				"dummy_id_enum_i32");
	configuration.ResetOverride(				"dummy_id_enum_u32");

	EXPECT_EQ(configuration.Get<bool>(			"dummy_id_bool",		false),					true);
	EXPECT_EQ(configuration.Get<u8>(			"dummy_id_u8",			0),						1);
	EXPECT_EQ(configuration.Get<u16>(			"dummy_id_u16",			0),						1);
	EXPECT_EQ(configuration.Get<u32>(			"dummy_id_u32",			0),						1);
	EXPECT_EQ(configuration.Get<u64>(			"dummy_id_u64",			0),						1);
	EXPECT_EQ(configuration.Get<i8>(			"dummy_id_i8",			0),						1);
	EXPECT_EQ(configuration.Get<i16>(			"dummy_id_i16",			0),						1);
	EXPECT_EQ(configuration.Get<i32>(			"dummy_id_i32",			0),						1);
	EXPECT_EQ(configuration.Get<i64>(			"dummy_id_i64",			0),						1);
	EXPECT_EQ(configuration.Get<f32>(			"dummy_id_f32",			0.f),					1.f);
	EXPECT_EQ(configuration.Get<f64>(			"dummy_id_f64",			0.),					1.);
	EXPECT_EQ(configuration.Get<std::string>(	"dummy_id_string",		"dummy_default"),		"dummy_value_first");
	EXPECT_EQ(configuration.Get<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::Zero),	ETestEnumI32::One);
	EXPECT_EQ(configuration.Get<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::Zero),	ETestEnumU32::One);
}

TEST(ConfigurationSystemTests, getconfigs_map_has_all_entries)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		true,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			1,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			1,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			1,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			1,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			1,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			1,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			1,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			1,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			1.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			1.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::One,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::One,		dummyGroup);
	
	auto configs = configuration.GetConfigs();

	EXPECT_NE(configs.find("dummy_id_bool"),		configs.cend());
	EXPECT_NE(configs.find("dummy_id_u8"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_u16"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_u32"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_u64"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_i8"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_i16"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_i32"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_i64"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_f32"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_f64"),			configs.cend());
	EXPECT_NE(configs.find("dummy_id_string"),		configs.cend());
	EXPECT_NE(configs.find("dummy_id_enum_i32"),	configs.cend());
	EXPECT_NE(configs.find("dummy_id_enum_u32"),	configs.cend());
}

TEST(ConfigurationSystemTests, getconfigs_entries_are_correct_raw_type)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		true,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			1,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			1,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			1,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			1,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			1,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			1,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			1,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			1,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			1.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			1.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::One,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::One,		dummyGroup);
	
	auto configs = configuration.GetConfigs();

	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<bool>>(		configs["dummy_id_bool"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<u8>>(			configs["dummy_id_u8"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<u16>>(			configs["dummy_id_u16"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<u32>>(			configs["dummy_id_u32"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<u64>>(			configs["dummy_id_u64"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<i8>>(			configs["dummy_id_i8"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<i16>>(			configs["dummy_id_i16"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<i32>>(			configs["dummy_id_i32"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<i64>>(			configs["dummy_id_i64"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<f32>>(			configs["dummy_id_f32"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<f64>>(			configs["dummy_id_f64"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<std::string>>(	configs["dummy_id_string"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<i32>>(			configs["dummy_id_enum_i32"]));
	EXPECT_TRUE(std::holds_alternative<Configuration::ConfigData<u32>>(			configs["dummy_id_enum_u32"]));
}

TEST(ConfigurationSystemTests, getconfigs_entries_have_correct_values)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		true,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			1,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			1,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			1,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			1,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			1,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			1,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			1,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			1,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			1.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			1.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::One,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::One,		dummyGroup);
	
	auto configs = configuration.GetConfigs();

	auto isSameValue = [&configs](const auto aKey, const auto aValue)
	{
		return std::visit([aValue](const auto& aElement)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(aElement.Value)>, std::decay_t<decltype(aValue)>>)
					return aElement.Value == aValue;
				
				return false;
			}, configs[aKey]);
	};

	EXPECT_TRUE(isSameValue("dummy_id_bool",		true));
	EXPECT_TRUE(isSameValue("dummy_id_u8",			u8(1)));
	EXPECT_TRUE(isSameValue("dummy_id_u16",			u16(1)));
	EXPECT_TRUE(isSameValue("dummy_id_u32",			u32(1)));
	EXPECT_TRUE(isSameValue("dummy_id_u64",			u64(1)));
	EXPECT_TRUE(isSameValue("dummy_id_i8",			i8(1)));
	EXPECT_TRUE(isSameValue("dummy_id_i16",			i16(1)));
	EXPECT_TRUE(isSameValue("dummy_id_i32",			i32(1)));
	EXPECT_TRUE(isSameValue("dummy_id_i64",			i64(1)));
	EXPECT_TRUE(isSameValue("dummy_id_f32",			f32(1.f)));
	EXPECT_TRUE(isSameValue("dummy_id_f64",			f64(1.)));
	EXPECT_TRUE(isSameValue("dummy_id_string",		std::string("dummy_value")));
	EXPECT_TRUE(isSameValue("dummy_id_enum_i32",	static_cast<i32>(ETestEnumI32::One)));
	EXPECT_TRUE(isSameValue("dummy_id_enum_u32",	static_cast<u32>(ETestEnumU32::One)));
}

TEST(ConfigurationSystemTests, getconfigs_entries_retain_value_when_overridden)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		false,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			0,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			0,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			0,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			0,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			0,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			0,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			0,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			0,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			0.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			0.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::Zero,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::Zero,		dummyGroup);
	
	configuration.SetOverride<bool>(			"dummy_id_bool",		true);
	configuration.SetOverride<u8>(				"dummy_id_u8",			1);
	configuration.SetOverride<u16>(				"dummy_id_u16",			1);
	configuration.SetOverride<u32>(				"dummy_id_u32",			1);
	configuration.SetOverride<u64>(				"dummy_id_u64",			1);
	configuration.SetOverride<i8>(				"dummy_id_i8",			1);
	configuration.SetOverride<i16>(				"dummy_id_i16",			1);
	configuration.SetOverride<i32>(				"dummy_id_i32",			1);
	configuration.SetOverride<i64>(				"dummy_id_i64",			1);
	configuration.SetOverride<f32>(				"dummy_id_f32",			1.f);
	configuration.SetOverride<f64>(				"dummy_id_f64",			1.);
	configuration.SetOverride<std::string>(		"dummy_id_string",		"dummy_override");
	configuration.SetOverride<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::One);
	configuration.SetOverride<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::One);
	
	auto configs = configuration.GetConfigs();
	
	auto isSameValue = [&configs](const auto aKey, const auto aValue)
	{
		return std::visit([&aValue](const auto& aElement)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(aElement.Value)>, std::decay_t<decltype(aValue)>>)
					return aElement.Value == aValue;
				
				return false;
			}, configs[aKey]);
	};
	
	EXPECT_TRUE(isSameValue("dummy_id_bool",		false));
	EXPECT_TRUE(isSameValue("dummy_id_u8",			u8(0)));
	EXPECT_TRUE(isSameValue("dummy_id_u16",			u16(0)));
	EXPECT_TRUE(isSameValue("dummy_id_u32",			u32(0)));
	EXPECT_TRUE(isSameValue("dummy_id_u64",			u64(0)));
	EXPECT_TRUE(isSameValue("dummy_id_i8",			i8(0)));
	EXPECT_TRUE(isSameValue("dummy_id_i16",			i16(0)));
	EXPECT_TRUE(isSameValue("dummy_id_i32",			i32(0)));
	EXPECT_TRUE(isSameValue("dummy_id_i64",			i64(0)));
	EXPECT_TRUE(isSameValue("dummy_id_f32",			f32(0.f)));
	EXPECT_TRUE(isSameValue("dummy_id_f64",			f64(0.)));
	EXPECT_TRUE(isSameValue("dummy_id_string",		std::string("dummy_value")));
	EXPECT_TRUE(isSameValue("dummy_id_enum_i32",	static_cast<i32>(ETestEnumI32::Zero)));
	EXPECT_TRUE(isSameValue("dummy_id_enum_u32",	static_cast<u32>(ETestEnumU32::Zero)));
}

TEST(ConfigurationSystemTests, getconfigs_entries_have_unset_overrides)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		false,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			0,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			0,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			0,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			0,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			0,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			0,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			0,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			0,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			0.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			0.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::Zero,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::Zero,		dummyGroup);
	
	auto configs = configuration.GetConfigs();

	auto isOverrideSet = [&configs](const auto aKey)
	{
		return std::visit([](const auto& aElement)
			{
				return static_cast<bool>(aElement.Override);
			}, configs[aKey]);
	};
	
	EXPECT_FALSE(isOverrideSet("dummy_id_bool"));
	EXPECT_FALSE(isOverrideSet("dummy_id_u8"));
	EXPECT_FALSE(isOverrideSet("dummy_id_u16"));
	EXPECT_FALSE(isOverrideSet("dummy_id_u32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_u64"));
	EXPECT_FALSE(isOverrideSet("dummy_id_i8"));
	EXPECT_FALSE(isOverrideSet("dummy_id_i16"));
	EXPECT_FALSE(isOverrideSet("dummy_id_i32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_i64"));
	EXPECT_FALSE(isOverrideSet("dummy_id_f32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_f64"));
	EXPECT_FALSE(isOverrideSet("dummy_id_string"));
	EXPECT_FALSE(isOverrideSet("dummy_id_enum_i32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_enum_u32"));
}

TEST(ConfigurationSystemTests, getconfigs_entries_have_set_overrides)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		false,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			0,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			0,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			0,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			0,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			0,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			0,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			0,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			0,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			0.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			0.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::Zero,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::Zero,		dummyGroup);
	
	configuration.SetOverride<bool>(			"dummy_id_bool",		true);
	configuration.SetOverride<u8>(				"dummy_id_u8",			1);
	configuration.SetOverride<u16>(				"dummy_id_u16",			1);
	configuration.SetOverride<u32>(				"dummy_id_u32",			1);
	configuration.SetOverride<u64>(				"dummy_id_u64",			1);
	configuration.SetOverride<i8>(				"dummy_id_i8",			1);
	configuration.SetOverride<i16>(				"dummy_id_i16",			1);
	configuration.SetOverride<i32>(				"dummy_id_i32",			1);
	configuration.SetOverride<i64>(				"dummy_id_i64",			1);
	configuration.SetOverride<f32>(				"dummy_id_f32",			1.f);
	configuration.SetOverride<f64>(				"dummy_id_f64",			1.);
	configuration.SetOverride<std::string>(		"dummy_id_string",		"dummy_override");
	configuration.SetOverride<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::One);
	configuration.SetOverride<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::One);
	
	auto configs = configuration.GetConfigs();

	auto isOverrideSet = [&configs](const auto aKey)
	{
		return std::visit([](const auto& aElement)
			{
				return static_cast<bool>(aElement.Override);
			}, configs[aKey]);
	};
	
	EXPECT_TRUE(isOverrideSet("dummy_id_bool"));
	EXPECT_TRUE(isOverrideSet("dummy_id_u8"));
	EXPECT_TRUE(isOverrideSet("dummy_id_u16"));
	EXPECT_TRUE(isOverrideSet("dummy_id_u32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_u64"));
	EXPECT_TRUE(isOverrideSet("dummy_id_i8"));
	EXPECT_TRUE(isOverrideSet("dummy_id_i16"));
	EXPECT_TRUE(isOverrideSet("dummy_id_i32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_i64"));
	EXPECT_TRUE(isOverrideSet("dummy_id_f32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_f64"));
	EXPECT_TRUE(isOverrideSet("dummy_id_string"));
	EXPECT_TRUE(isOverrideSet("dummy_id_enum_i32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_enum_u32"));
}

TEST(ConfigurationSystemTests, getconfigs_entries_have_correct_overrides)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_bool",		false,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_u8",			0,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_u16",			0,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_u32",			0,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_u64",			0,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_i8",			0,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_i16",			0,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_i32",			0,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_i64",			0,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_f32",			0.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_f64",			0.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_enum_i32",	ETestEnumI32::Zero,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_enum_u32",	ETestEnumU32::Zero,		dummyGroup);
	
	configuration.SetOverride<bool>(			"dummy_id_bool",		true);
	configuration.SetOverride<u8>(				"dummy_id_u8",			1);
	configuration.SetOverride<u16>(				"dummy_id_u16",			1);
	configuration.SetOverride<u32>(				"dummy_id_u32",			1);
	configuration.SetOverride<u64>(				"dummy_id_u64",			1);
	configuration.SetOverride<i8>(				"dummy_id_i8",			1);
	configuration.SetOverride<i16>(				"dummy_id_i16",			1);
	configuration.SetOverride<i32>(				"dummy_id_i32",			1);
	configuration.SetOverride<i64>(				"dummy_id_i64",			1);
	configuration.SetOverride<f32>(				"dummy_id_f32",			1.f);
	configuration.SetOverride<f64>(				"dummy_id_f64",			1.);
	configuration.SetOverride<std::string>(		"dummy_id_string",		"dummy_override");
	configuration.SetOverride<ETestEnumI32>(	"dummy_id_enum_i32",	ETestEnumI32::One);
	configuration.SetOverride<ETestEnumU32>(	"dummy_id_enum_u32",	ETestEnumU32::One);
	
	auto configs = configuration.GetConfigs();

	auto isSameOverride = [&configs](const auto aKey, const auto aOverride)
	{
		return std::visit([&aOverride](const auto& aElement)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(aElement.Value)>, std::decay_t<decltype(aOverride)>>)
					return *aElement.Override == aOverride;

				return false;
			}, configs[aKey]);
	};
	
	EXPECT_TRUE(isSameOverride("dummy_id_bool",		true));
	EXPECT_TRUE(isSameOverride("dummy_id_u8",		u8(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_u16",		u16(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_u32",		u32(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_u64",		u64(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_i8",		i8(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_i16",		i16(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_i32",		i32(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_i64",		i64(1)));
	EXPECT_TRUE(isSameOverride("dummy_id_f32",		f32(1.f)));
	EXPECT_TRUE(isSameOverride("dummy_id_f64",		f64(1.)));
	EXPECT_TRUE(isSameOverride("dummy_id_string",	std::string("dummy_override")));
	EXPECT_TRUE(isSameOverride("dummy_id_enum_i32",	static_cast<i32>(ETestEnumI32::One)));
	EXPECT_TRUE(isSameOverride("dummy_id_enum_u32",	static_cast<u32>(ETestEnumU32::One)));
}

TEST(ConfigurationSystemTests, getconfigs_set_override_is_only_set_for_one_entry)
{
	constexpr EConfigGroup dummyGroup = EConfigGroup::Application;

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_value_bool",			false,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_value_u8",			0,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_value_u16",			0,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_value_u32",			0,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_value_u64",			0,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_value_i8",			0,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_value_i16",			0,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_value_i32",			0,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_value_i64",			0,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_value_f32",			0.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_value_f64",			0.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_value_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_value_enum_i32",		ETestEnumI32::Zero,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_value_enum_u32",		ETestEnumU32::Zero,		dummyGroup);
	
	configuration.Set<bool>(					"dummy_id_override_bool",		false,					dummyGroup);
	configuration.Set<u8>(						"dummy_id_override_u8",			0,						dummyGroup);
	configuration.Set<u16>(						"dummy_id_override_u16",		0,						dummyGroup);
	configuration.Set<u32>(						"dummy_id_override_u32",		0,						dummyGroup);
	configuration.Set<u64>(						"dummy_id_override_u64",		0,						dummyGroup);
	configuration.Set<i8>(						"dummy_id_override_i8",			0,						dummyGroup);
	configuration.Set<i16>(						"dummy_id_override_i16",		0,						dummyGroup);
	configuration.Set<i32>(						"dummy_id_override_i32",		0,						dummyGroup);
	configuration.Set<i64>(						"dummy_id_override_i64",		0,						dummyGroup);
	configuration.Set<f32>(						"dummy_id_override_f32",		0.f,					dummyGroup);
	configuration.Set<f64>(						"dummy_id_override_f64",		0.,						dummyGroup);
	configuration.Set<std::string>(				"dummy_id_override_string",		"dummy_value",			dummyGroup);
	configuration.Set<ETestEnumI32>(			"dummy_id_override_enum_i32",	ETestEnumI32::Zero,		dummyGroup);
	configuration.Set<ETestEnumU32>(			"dummy_id_override_enum_u32",	ETestEnumU32::Zero,		dummyGroup);

	configuration.SetOverride<bool>(			"dummy_id_override_bool",		true);
	configuration.SetOverride<u8>(				"dummy_id_override_u8",			1);
	configuration.SetOverride<u16>(				"dummy_id_override_u16",		1);
	configuration.SetOverride<u32>(				"dummy_id_override_u32",		1);
	configuration.SetOverride<u64>(				"dummy_id_override_u64",		1);
	configuration.SetOverride<i8>(				"dummy_id_override_i8",			1);
	configuration.SetOverride<i16>(				"dummy_id_override_i16",		1);
	configuration.SetOverride<i32>(				"dummy_id_override_i32",		1);
	configuration.SetOverride<i64>(				"dummy_id_override_i64",		1);
	configuration.SetOverride<f32>(				"dummy_id_override_f32",		1.f);
	configuration.SetOverride<f64>(				"dummy_id_override_f64",		1.);
	configuration.SetOverride<std::string>(		"dummy_id_override_string",		"dummy_override");
	configuration.SetOverride<ETestEnumI32>(	"dummy_id_override_enum_i32",	ETestEnumI32::One);
	configuration.SetOverride<ETestEnumU32>(	"dummy_id_override_enum_u32",	ETestEnumU32::One);

	auto configs = configuration.GetConfigs();

	auto isOverrideSet = [&configs](const auto aKey)
	{
		return std::visit([](const auto& aElement)
			{
				return static_cast<bool>(aElement.Override);
			}, configs[aKey]);
	};
	
	EXPECT_FALSE(isOverrideSet("dummy_id_value_bool"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_u8"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_u16"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_u32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_u64"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_i8"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_i16"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_i32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_i64"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_f32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_f64"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_string"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_enum_i32"));
	EXPECT_FALSE(isOverrideSet("dummy_id_value_enum_u32"));

	EXPECT_TRUE(isOverrideSet("dummy_id_override_bool"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_u8"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_u16"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_u32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_u64"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_i8"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_i16"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_i32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_i64"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_f32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_f64"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_string"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_enum_i32"));
	EXPECT_TRUE(isOverrideSet("dummy_id_override_enum_u32"));
}

TEST(ConfigurationSystemTests, getconfigs_entries_are_correct_config_group)
{
	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;
	
	configuration.Set<bool>(					"dummy_id_application_bool",		false,					EConfigGroup::Application);
	configuration.Set<u8>(						"dummy_id_application_u8",			0,						EConfigGroup::Application);
	configuration.Set<u16>(						"dummy_id_application_u16",			0,						EConfigGroup::Application);
	configuration.Set<u32>(						"dummy_id_application_u32",			0,						EConfigGroup::Application);
	configuration.Set<u64>(						"dummy_id_application_u64",			0,						EConfigGroup::Application);
	configuration.Set<i8>(						"dummy_id_application_i8",			0,						EConfigGroup::Application);
	configuration.Set<i16>(						"dummy_id_application_i16",			0,						EConfigGroup::Application);
	configuration.Set<i32>(						"dummy_id_application_i32",			0,						EConfigGroup::Application);
	configuration.Set<i64>(						"dummy_id_application_i64",			0,						EConfigGroup::Application);
	configuration.Set<f32>(						"dummy_id_application_f32",			0.f,					EConfigGroup::Application);
	configuration.Set<f64>(						"dummy_id_application_f64",			0.,						EConfigGroup::Application);
	configuration.Set<std::string>(				"dummy_id_application_string",		"dummy_value",			EConfigGroup::Application);
	configuration.Set<ETestEnumI32>(			"dummy_id_application_enum_i32",	ETestEnumI32::Zero,		EConfigGroup::Application);
	configuration.Set<ETestEnumU32>(			"dummy_id_application_enum_u32",	ETestEnumU32::Zero,		EConfigGroup::Application);
	
	configuration.Set<bool>(					"dummy_id_user_bool",				false,					EConfigGroup::User);
	configuration.Set<u8>(						"dummy_id_user_u8",					0,						EConfigGroup::User);
	configuration.Set<u16>(						"dummy_id_user_u16",				0,						EConfigGroup::User);
	configuration.Set<u32>(						"dummy_id_user_u32",				0,						EConfigGroup::User);
	configuration.Set<u64>(						"dummy_id_user_u64",				0,						EConfigGroup::User);
	configuration.Set<i8>(						"dummy_id_user_i8",					0,						EConfigGroup::User);
	configuration.Set<i16>(						"dummy_id_user_i16",				0,						EConfigGroup::User);
	configuration.Set<i32>(						"dummy_id_user_i32",				0,						EConfigGroup::User);
	configuration.Set<i64>(						"dummy_id_user_i64",				0,						EConfigGroup::User);
	configuration.Set<f32>(						"dummy_id_user_f32",				0.f,					EConfigGroup::User);
	configuration.Set<f64>(						"dummy_id_user_f64",				0.,						EConfigGroup::User);
	configuration.Set<std::string>(				"dummy_id_user_string",				"dummy_value",			EConfigGroup::User);
	configuration.Set<ETestEnumI32>(			"dummy_id_user_enum_i32",			ETestEnumI32::Zero,		EConfigGroup::User);
	configuration.Set<ETestEnumU32>(			"dummy_id_user_enum_u32",			ETestEnumU32::Zero,		EConfigGroup::User);
	
	configuration.Set<bool>(					"dummy_id_device_bool",				false,					EConfigGroup::Device);
	configuration.Set<u8>(						"dummy_id_device_u8",				0,						EConfigGroup::Device);
	configuration.Set<u16>(						"dummy_id_device_u16",				0,						EConfigGroup::Device);
	configuration.Set<u32>(						"dummy_id_device_u32",				0,						EConfigGroup::Device);
	configuration.Set<u64>(						"dummy_id_device_u64",				0,						EConfigGroup::Device);
	configuration.Set<i8>(						"dummy_id_device_i8",				0,						EConfigGroup::Device);
	configuration.Set<i16>(						"dummy_id_device_i16",				0,						EConfigGroup::Device);
	configuration.Set<i32>(						"dummy_id_device_i32",				0,						EConfigGroup::Device);
	configuration.Set<i64>(						"dummy_id_device_i64",				0,						EConfigGroup::Device);
	configuration.Set<f32>(						"dummy_id_device_f32",				0.f,					EConfigGroup::Device);
	configuration.Set<f64>(						"dummy_id_device_f64",				0.,						EConfigGroup::Device);
	configuration.Set<std::string>(				"dummy_id_device_string",			"dummy_value",			EConfigGroup::Device);
	configuration.Set<ETestEnumI32>(			"dummy_id_device_enum_i32",			ETestEnumI32::Zero,		EConfigGroup::Device);
	configuration.Set<ETestEnumU32>(			"dummy_id_device_enum_u32",			ETestEnumU32::Zero,		EConfigGroup::Device);
	
	auto configs = configuration.GetConfigs();

	auto getConfigGroup = [&configs](const auto aKey)
	{
		return std::visit([](const auto& aElement)
			{
				return aElement.ConfigGroup;
			}, configs[aKey]);
	};
	
	EXPECT_EQ(getConfigGroup("dummy_id_application_bool"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_u8"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_u16"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_u32"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_u64"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_i8"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_i16"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_i32"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_i64"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_f32"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_f64"),		EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_string"),	EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_enum_i32"),	EConfigGroup::Application);
	EXPECT_EQ(getConfigGroup("dummy_id_application_enum_u32"),	EConfigGroup::Application);
	
	EXPECT_EQ(getConfigGroup("dummy_id_user_bool"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_u8"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_u16"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_u32"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_u64"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_i8"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_i16"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_i32"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_i64"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_f32"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_f64"),				EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_string"),			EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_enum_i32"),			EConfigGroup::User);
	EXPECT_EQ(getConfigGroup("dummy_id_user_enum_u32"),			EConfigGroup::User);
	
	EXPECT_EQ(getConfigGroup("dummy_id_device_bool"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_u8"),				EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_u16"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_u32"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_u64"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_i8"),				EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_i16"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_i32"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_i64"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_f32"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_f64"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_string"),			EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_enum_i32"),		EConfigGroup::Device);
	EXPECT_EQ(getConfigGroup("dummy_id_device_enum_u32"),		EConfigGroup::Device);
}
