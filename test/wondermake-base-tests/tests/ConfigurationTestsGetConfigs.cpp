#include "wondermake-tests-common/GTestInclude.h"

#include "ConfigurationTests.h"

#include "wondermake-base/Configuration.h"

constexpr EConfigGroup locDummyGroup = EConfigGroup::Application;

TEST(ConfigurationTests, get_configs_returns_empty_list_when_no_values_are_set)
{
	Configuration configuration;

	const auto configs = configuration.GetConfigs();

	EXPECT_EQ(configs.size(), 0);
}

TEST(ConfigurationTests, get_configs_returns_list_with_set_value)
{
	Configuration configuration;

	const std::string dummyId		= "dummy_id";
	const std::string dummyValue	= "dummy_value";

	configuration.Set<std::string>(dummyId, dummyValue, locDummyGroup);

	auto configs = configuration.GetConfigs();

	EXPECT_EQ(configs.size(), 1);

	ASSERT_TRUE(std::holds_alternative<Configuration::ConfigData<std::string>>(configs[dummyId]));

	auto&& config = std::get<Configuration::ConfigData<std::string>>(configs[dummyId]);

	EXPECT_EQ(config.Value, dummyValue);
	EXPECT_FALSE(config.Override);
}

TEST(ConfigurationTests, get_configs_returns_list_with_set_override)
{
	Configuration configuration;

	const std::string dummyId		= "dummy_id";
	const std::string dummyValue	= "dummy_value";
	const std::string dummyOverride	= "dummy_override";

	configuration.Set<std::string>(dummyId, dummyValue, locDummyGroup);

	configuration.SetOverride<std::string>(dummyId, dummyOverride);

	auto configs = configuration.GetConfigs();

	auto&& config = std::get<Configuration::ConfigData<std::string>>(configs[dummyId]);

	ASSERT_TRUE(config.Override);

	EXPECT_EQ(config.Override, dummyOverride);
}

TEST(ConfigurationTests, get_configs_returns_list_with_all_set_values)
{
	Configuration configuration;

	const std::string dummyIdA		= "dummy_id_a";
	const std::string dummyValueA	= "dummy_value_a";
	const std::string dummyIdB		= "dummy_id_b";
	const std::string dummyValueB	= "dummy_value_b";

	configuration.Set<std::string>(dummyIdA, dummyValueA, locDummyGroup);
	configuration.Set<std::string>(dummyIdB, dummyValueB, locDummyGroup);

	auto configs = configuration.GetConfigs();

	EXPECT_EQ(configs.size(), 2);

	ASSERT_TRUE(std::holds_alternative<Configuration::ConfigData<std::string>>(configs[dummyIdA]));
	ASSERT_TRUE(std::holds_alternative<Configuration::ConfigData<std::string>>(configs[dummyIdB]));

	auto&& configA = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdA]);
	auto&& configB = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdB]);

	EXPECT_EQ(configA.Value, dummyValueA);
	EXPECT_FALSE(configA.Override);
	EXPECT_EQ(configB.Value, dummyValueB);
	EXPECT_FALSE(configB.Override);
}

TEST(ConfigurationTests, get_configs_returns_list_with_all_config_groups)
{
	Configuration configuration;

	const std::string dummyIdA			= "dummy_id_a";
	const std::string dummyIdB			= "dummy_id_b";
	const std::string dummyIdC			= "dummy_id_c";
	const std::string dummyValue		= "dummy_value";
	constexpr EConfigGroup dummyGroupA	= EConfigGroup::Application;
	constexpr EConfigGroup dummyGroupB	= EConfigGroup::Device;
	constexpr EConfigGroup dummyGroupC	= EConfigGroup::User;

	configuration.Set<std::string>(dummyIdA, dummyValue, dummyGroupA);
	configuration.Set<std::string>(dummyIdB, dummyValue, dummyGroupB);
	configuration.Set<std::string>(dummyIdC, dummyValue, dummyGroupC);

	auto configs = configuration.GetConfigs();

	auto&& configA = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdA]);
	auto&& configB = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdB]);
	auto&& configC = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdC]);

	ASSERT_EQ(configA.ConfigGroup, dummyGroupA);
	ASSERT_EQ(configB.ConfigGroup, dummyGroupB);
	ASSERT_EQ(configC.ConfigGroup, dummyGroupC);
}

TEST(ConfigurationTests, get_configs_returns_list_with_all_set_overrides)
{
	Configuration configuration;

	const std::string dummyIdA			= "dummy_id_a";
	const std::string dummyValueA		= "dummy_value_a";
	const std::string dummyOverrideA	= "dummy_override_a";
	const std::string dummyIdB			= "dummy_id_b";
	const std::string dummyValueB		= "dummy_value_b";
	const std::string dummyOverrideB	= "dummy_override_b";

	configuration.Set<std::string>(dummyIdA, dummyValueA, locDummyGroup);
	configuration.Set<std::string>(dummyIdB, dummyValueB, locDummyGroup);

	configuration.SetOverride<std::string>(dummyIdA, dummyOverrideA);
	configuration.SetOverride<std::string>(dummyIdB, dummyOverrideB);

	auto configs = configuration.GetConfigs();

	auto&& configA = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdA]);
	auto&& configB = std::get<Configuration::ConfigData<std::string>>(configs[dummyIdB]);

	ASSERT_TRUE(configA.Override);
	ASSERT_TRUE(configB.Override);

	EXPECT_EQ(configA.Override, dummyOverrideA);
	EXPECT_EQ(configB.Override, dummyOverrideB);
}

TEST(ConfigurationTests, empty_allowed_list_is_returned_when_calling_getconfig_when_none_is_set)
{
	Configuration configuration;

	configuration.Set<bool>(			"dummy_id_bool",			false,				locDummyGroup);
	configuration.Set<u8>(				"dummy_id_u8",				0,					locDummyGroup);
	configuration.Set<u16>(				"dummy_id_u16",				0,					locDummyGroup);
	configuration.Set<u32>(				"dummy_id_u32",				0,					locDummyGroup);
	configuration.Set<u64>(				"dummy_id_u64",				0,					locDummyGroup);
	configuration.Set<i8>(				"dummy_id_i8",				0,					locDummyGroup);
	configuration.Set<i16>(				"dummy_id_i16",				0,					locDummyGroup);
	configuration.Set<i32>(				"dummy_id_i32",				0,					locDummyGroup);
	configuration.Set<i64>(				"dummy_id_i64",				0,					locDummyGroup);
	configuration.Set<std::string>(		"dummy_id_string",			"dummy_value",		locDummyGroup);
	configuration.Set<ETestEnumI32>(	"dummy_id_enum_i32",		ETestEnumI32::Zero,	locDummyGroup);
	configuration.Set<ETestEnumU32>(	"dummy_id_enum_u32",		ETestEnumU32::Zero,	locDummyGroup);

	auto configs = configuration.GetConfigs();

	EXPECT_TRUE(std::get<Configuration::ConfigData<bool>>			(configs["dummy_id_bool"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<u8>>				(configs["dummy_id_u8"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<u16>>			(configs["dummy_id_u16"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<u32>>			(configs["dummy_id_u32"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<u64>>			(configs["dummy_id_u64"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<i8>>				(configs["dummy_id_i8"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<i16>>			(configs["dummy_id_i16"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<i32>>			(configs["dummy_id_i32"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<i64>>			(configs["dummy_id_i64"])				.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<std::string>>	(configs["dummy_id_string"])			.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<i32>>			(configs["dummy_id_enum_i32"])			.AllowedValues.empty());
	EXPECT_TRUE(std::get<Configuration::ConfigData<u32>>			(configs["dummy_id_enum_u32"])			.AllowedValues.empty());
}

TEST(ConfigurationTests, allowed_list_is_returned_when_calling_getconfig_when_list_is_set)
{
	Configuration configuration;

	configuration.Set<bool>(			"dummy_id_bool",			true,					locDummyGroup,	Configuration::AllowedValues<bool>(			{ { "dummy_name", true } }));
	configuration.Set<u8>(				"dummy_id_u8",				0,						locDummyGroup,	Configuration::AllowedValues<u8>(			{ { "dummy_name", 1 } }));
	configuration.Set<u16>(				"dummy_id_u16",				0,						locDummyGroup,	Configuration::AllowedValues<u16>(			{ { "dummy_name", 1 } }));
	configuration.Set<u32>(				"dummy_id_u32",				0,						locDummyGroup,	Configuration::AllowedValues<u32>(			{ { "dummy_name", 1 } }));
	configuration.Set<u64>(				"dummy_id_u64",				0,						locDummyGroup,	Configuration::AllowedValues<u64>(			{ { "dummy_name", 1 } }));
	configuration.Set<i8>(				"dummy_id_i8",				0,						locDummyGroup,	Configuration::AllowedValues<i8>(			{ { "dummy_name", 1 } }));
	configuration.Set<i16>(				"dummy_id_i16",				0,						locDummyGroup,	Configuration::AllowedValues<i16>(			{ { "dummy_name", 1 } }));
	configuration.Set<i32>(				"dummy_id_i32",				0,						locDummyGroup,	Configuration::AllowedValues<i32>(			{ { "dummy_name", 1 } }));
	configuration.Set<i64>(				"dummy_id_i64",				0,						locDummyGroup,	Configuration::AllowedValues<i64>(			{ { "dummy_name", 1 } }));
	configuration.Set<std::string>(		"dummy_id_string",			"dummy_value",			locDummyGroup,	Configuration::AllowedValues<std::string>(	{ { "dummy_name", std::string("dummy_value") } }));
	configuration.Set<ETestEnumI32>(	"dummy_id_enum_i32",		ETestEnumI32::One,		locDummyGroup,	Configuration::AllowedValues<ETestEnumI32>(	{ { "dummy_name", ETestEnumI32::One } }));
	configuration.Set<ETestEnumU32>(	"dummy_id_enum_u32",		ETestEnumU32::One,		locDummyGroup,	Configuration::AllowedValues<ETestEnumU32>(	{ { "dummy_name", ETestEnumU32::One } }));

	auto configs = configuration.GetConfigs();

	EXPECT_EQ(std::get<Configuration::ConfigData<bool>>(		configs["dummy_id_bool"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u8>>(			configs["dummy_id_u8"])				.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u16>>(			configs["dummy_id_u16"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u32>>(			configs["dummy_id_u32"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u64>>(			configs["dummy_id_u64"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i8>>(			configs["dummy_id_i8"])				.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i16>>(			configs["dummy_id_i16"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i32>>(			configs["dummy_id_i32"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i64>>(			configs["dummy_id_i64"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<std::string>>(	configs["dummy_id_string"])			.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i32>>(			configs["dummy_id_enum_i32"])		.AllowedValues.size(), 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u32>>(			configs["dummy_id_enum_u32"])		.AllowedValues.size(), 1);

	EXPECT_EQ(std::get<Configuration::ConfigData<bool>>(		configs["dummy_id_bool"])			.AllowedValues["dummy_name"], true);
	EXPECT_EQ(std::get<Configuration::ConfigData<u8>>(			configs["dummy_id_u8"])				.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u16>>(			configs["dummy_id_u16"])			.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u32>>(			configs["dummy_id_u32"])			.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u64>>(			configs["dummy_id_u64"])			.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i8>>(			configs["dummy_id_i8"])				.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i16>>(			configs["dummy_id_i16"])			.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i32>>(			configs["dummy_id_i32"])			.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<i64>>(			configs["dummy_id_i64"])			.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<std::string>>(	configs["dummy_id_string"])			.AllowedValues["dummy_name"], std::string("dummy_value"));
	EXPECT_EQ(std::get<Configuration::ConfigData<i32>>(			configs["dummy_id_enum_i32"])		.AllowedValues["dummy_name"], static_cast<i32>(ETestEnumI32::One));
	EXPECT_EQ(std::get<Configuration::ConfigData<u32>>(			configs["dummy_id_enum_u32"])		.AllowedValues["dummy_name"], static_cast<u32>(ETestEnumU32::One));
	EXPECT_EQ(std::get<Configuration::ConfigData<i32>>(			configs["dummy_id_enum_i32"])		.AllowedValues["dummy_name"], 1);
	EXPECT_EQ(std::get<Configuration::ConfigData<u32>>(			configs["dummy_id_enum_u32"])		.AllowedValues["dummy_name"], 1);
}
