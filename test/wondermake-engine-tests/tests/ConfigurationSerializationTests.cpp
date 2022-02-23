#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeBaseTests/ProcessSystemMock.h"

#include "wondermake-engine/DeserializeConfigurationJob.h"
#include "wondermake-engine/SerializeConfigurationJob.h"

#include "wondermake-base/ConfigurationSystem.h"

#include "json/json.hpp"

using namespace std::literals::string_view_literals;

inline constexpr auto locTestConfigGroup = EConfigGroup::Application;

TEST(ConfigurationSerializationTests, deserialize_completes_with_parsing_error_when_given_non_json_string)
{
	constexpr auto dummyJsonString = "Hello World!";
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promise, future] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promise), locTestConfigGroup, dummyJsonString);

	ASSERT_TRUE(future.IsCompleted());

	ASSERT_TRUE(future.GetResult());

	ASSERT_FALSE(*future.GetResult());

	EXPECT_EQ(*future.GetResult(), EDeserializeConfigurationError::ParsingError);
}

TEST(ConfigurationSerializationTests, deserialize_completes_with_parsing_error_when_given_non_configuration_json)
{
	constexpr auto dummyJsonString = "{\"Hello\":\"World!\"}";
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promise, future] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promise), locTestConfigGroup, dummyJsonString);

	ASSERT_TRUE(future.IsCompleted());

	ASSERT_TRUE(future.GetResult());

	ASSERT_FALSE(*future.GetResult());

	EXPECT_EQ(*future.GetResult(), EDeserializeConfigurationError::InvalidJsonStructure);
}

TEST(ConfigurationSerializationTests, deserialize_completes_with_success_when_given_configuration_json)
{
	constexpr auto dummyJsonString = "{\"overrides\": {}}";
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promise, future] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promise), locTestConfigGroup, dummyJsonString);

	ASSERT_TRUE(future.IsCompleted());

	ASSERT_TRUE(future.GetResult());

	EXPECT_TRUE(*future.GetResult());
}

TEST(ConfigurationSerializationTests, deserialize_sets_no_override_when_no_configuration_is_set)
{
	constexpr auto dummyJsonString = "{\"overrides\": { \"kalle\": \"hobbe\", \"greger\": 1234, \"sylt\": -1234 } }";
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promise, future] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promise), locTestConfigGroup, dummyJsonString);

	EXPECT_EQ(configSystem.Get<std::string>("kalle", ""), "");
	EXPECT_EQ(configSystem.Get<u64>("greger", 0), 0);
	EXPECT_EQ(configSystem.Get<i64>("sylt", 0), 0);
}

TEST(ConfigurationSerializationTests, deserialize_sets_configurations_when_configuration_is_set)
{
	constexpr auto dummyJsonString = "{\"overrides\": { \"kalle\": \"hobbe\", \"greger\": 1234, \"sylt\": -1234 } }";
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<std::string>("kalle", "ruben", locTestConfigGroup);
	configSystem.Set<u64>("greger", 5678, locTestConfigGroup);
	configSystem.Set<i64>("sylt", 5678, locTestConfigGroup);

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promise, future] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promise), locTestConfigGroup, dummyJsonString);

	EXPECT_EQ(configSystem.Get<std::string>("kalle", ""), "hobbe");
	EXPECT_EQ(configSystem.Get<u64>("greger", 0), 1234);
	EXPECT_EQ(configSystem.Get<i64>("sylt", 0), -1234);
}

TEST(ConfigurationSerializationTests, deserialize_do_not_set_config_when_wrong_config_group)
{
	constexpr EConfigGroup dummyGroupTarget = EConfigGroup::Application;
	constexpr EConfigGroup dummyGroupOther = EConfigGroup::Device;
	constexpr auto dummyJsonString = "{\"overrides\": { \"kalle\": \"hobbe\", \"greger\": 1234, \"sylt\": -1234 } }";
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<std::string>("kalle", "ruben", dummyGroupTarget);
	configSystem.Set<u64>("greger", 5678, dummyGroupTarget);
	configSystem.Set<i64>("sylt", 5678, dummyGroupOther);

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promise, future] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promise), locTestConfigGroup, dummyJsonString);

	EXPECT_EQ(configSystem.Get<std::string>("kalle", ""), "hobbe");
	EXPECT_EQ(configSystem.Get<u64>("greger", 0), 1234);
	EXPECT_EQ(configSystem.Get<i64>("sylt", 0), 5678);
}

TEST(ConfigurationSerializationTests, serialize_completes_when_run)
{
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promise, future] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promise), locTestConfigGroup);

	EXPECT_TRUE(future.IsCompleted());
}

TEST(ConfigurationSerializationTests, serialized_outputs_valid_json)
{
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promise, future] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promise), locTestConfigGroup);

	EXPECT_NO_THROW(json::parse(*future.GetResult()));
}

TEST(ConfigurationSerializationTests, serialized_outputs_valid_structure)
{
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<std::string>("kalle", "ruben", locTestConfigGroup);
	configSystem.Set<u64>("greger", 5678, locTestConfigGroup);
	configSystem.Set<i64>("sylt", 5678, locTestConfigGroup);

	configSystem.SetOverride<std::string>("kalle", std::string("hobbe"));
	configSystem.SetOverride<u64>("greger", 1234);
	configSystem.SetOverride<i64>("sylt", -1234);

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promise, future] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promise), locTestConfigGroup);

	json j = json::parse(*future.GetResult());

	auto&& overrides = j["overrides"];
	json expectedOverrides;

	expectedOverrides["kalle"] = "hobbe";
	expectedOverrides["greger"] = 1234;
	expectedOverrides["sylt"] = -1234;

	EXPECT_EQ(overrides, expectedOverrides);
}

TEST(ConfigurationSerializationTests, serialized_outputs_selected_config_group)
{
	constexpr EConfigGroup dummyGroupTarget	= EConfigGroup::Application;
	constexpr EConfigGroup dummyGroupOther	= EConfigGroup::Device;
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<std::string>("kalle", "ruben", dummyGroupTarget);
	configSystem.Set<u64>("greger", 5678, dummyGroupTarget);
	configSystem.Set<i64>("sylt", 5678, dummyGroupOther);

	configSystem.SetOverride<std::string>("kalle", std::string("hobbe"));
	configSystem.SetOverride<u64>("greger", 1234);
	configSystem.SetOverride<i64>("sylt", -1234);

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promise, future] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promise), dummyGroupTarget);

	json j = json::parse(*future.GetResult());

	auto&& overrides = j["overrides"];
	json expectedOverrides;

	expectedOverrides["kalle"] = "hobbe";
	expectedOverrides["greger"] = 1234;

	EXPECT_EQ(overrides, expectedOverrides);
}

TEST(ConfigurationSerializationTests, serialized_output_can_be_deserialized)
{
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<std::string>("kalle", "ruben", locTestConfigGroup);
	configSystem.Set<u64>("greger", 5678, locTestConfigGroup);
	configSystem.Set<i64>("sylt", 5678, locTestConfigGroup);

	configSystem.SetOverride<std::string>("kalle", std::string("hobbe"));
	configSystem.SetOverride<u64>("greger", 1234);
	configSystem.SetOverride<i64>("sylt", -1234);

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promiseSerialize, futureSerialize] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promiseSerialize), locTestConfigGroup);

	configSystem.ResetOverride("kalle");
	configSystem.ResetOverride("greger");
	configSystem.ResetOverride("sylt");

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promiseDeserialize, futureDeserialize] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promiseDeserialize), locTestConfigGroup, *futureSerialize.GetResult());

	EXPECT_TRUE(futureDeserialize.IsCompleted());

	EXPECT_EQ(configSystem.Get<std::string>("kalle", ""), "hobbe");
	EXPECT_EQ(configSystem.Get<u64>("greger", 0), 1234);
	EXPECT_EQ(configSystem.Get<i64>("sylt", 0), -1234);
}

TEST(ConfigurationSerializationTests, boolean_values_are_serialized_and_deserialized_correctly)
{
	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<bool>("boolean_false",	false,	locTestConfigGroup);
	configSystem.Set<bool>("boolean_true",	true,	locTestConfigGroup);

	configSystem.SetOverride<bool>("boolean_false",	false);
	configSystem.SetOverride<bool>("boolean_true",	true);

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promiseSerialize, futureSerialize] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promiseSerialize), locTestConfigGroup);

	configSystem.ResetOverride("boolean_false");
	configSystem.ResetOverride("boolean_true");

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promiseDeserialize, futureDeserialize] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promiseDeserialize), locTestConfigGroup, *futureSerialize.GetResult());

	EXPECT_EQ(configSystem.Get<bool>("boolean_false",	true),	false);
	EXPECT_EQ(configSystem.Get<bool>("boolean_true",	false),	true);
}

TEST(ConfigurationSerializationTests, unsigned_values_are_serialized_and_deserialized_correctly)
{
	constexpr auto dummyDefaultValue = 1;

	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<u8>(			"u8_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u8>(			"u8_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u16>(			"u16_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u16>(			"u16_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u32>(			"u32_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u32>(			"u32_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u64>(			"u64_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<u64>(			"u64_max",		dummyDefaultValue,	locTestConfigGroup);

	configSystem.SetOverride<u8>(	"u8_zero",		0);
	configSystem.SetOverride<u8>(	"u8_max",		std::numeric_limits<u8>::max());
	configSystem.SetOverride<u16>(	"u16_zero",		0);
	configSystem.SetOverride<u16>(	"u16_max",		std::numeric_limits<u16>::max());
	configSystem.SetOverride<u32>(	"u32_zero",		0);
	configSystem.SetOverride<u32>(	"u32_max",		std::numeric_limits<u32>::max());
	configSystem.SetOverride<u64>(	"u64_zero",		0);
	configSystem.SetOverride<u64>(	"u64_max",		std::numeric_limits<u64>::max());

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promiseSerialize, futureSerialize] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promiseSerialize), locTestConfigGroup);

	configSystem.ResetOverride("u8_zero");
	configSystem.ResetOverride("u8_max");
	configSystem.ResetOverride("u16_zero");
	configSystem.ResetOverride("u16_max");
	configSystem.ResetOverride("u32_zero");
	configSystem.ResetOverride("u32_max");
	configSystem.ResetOverride("u64_zero");
	configSystem.ResetOverride("u64_max");

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promiseDeserialize, futureDeserialize] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promiseDeserialize), locTestConfigGroup, *futureSerialize.GetResult());

	EXPECT_EQ(configSystem.Get<u8>(		"u8_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<u8>(		"u8_max",	dummyDefaultValue),	std::numeric_limits<u8>::max());
	EXPECT_EQ(configSystem.Get<u16>(	"u16_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<u16>(	"u16_max",	dummyDefaultValue),	std::numeric_limits<u16>::max());
	EXPECT_EQ(configSystem.Get<u32>(	"u32_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<u32>(	"u32_max",	dummyDefaultValue),	std::numeric_limits<u32>::max());
	EXPECT_EQ(configSystem.Get<u64>(	"u64_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<u64>(	"u64_max",	dummyDefaultValue),	std::numeric_limits<u64>::max());
}

TEST(ConfigurationSerializationTests, signed_values_are_serialized_and_deserialized_correctly)
{
	constexpr auto dummyDefaultValue = 1;

	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<i8>(			"i8_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i8>(			"i8_min",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i8>(			"i8_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i16>(			"i16_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i16>(			"i16_min",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i16>(			"i16_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i32>(			"i32_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i32>(			"i32_min",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i32>(			"i32_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i64>(			"i64_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i64>(			"i64_min",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<i64>(			"i64_max",		dummyDefaultValue,	locTestConfigGroup);

	configSystem.SetOverride<i8>(	"i8_zero",		0);
	configSystem.SetOverride<i8>(	"i8_min",		std::numeric_limits<i8>::min());
	configSystem.SetOverride<i8>(	"i8_max",		std::numeric_limits<i8>::max());
	configSystem.SetOverride<i16>(	"i16_zero",		0);
	configSystem.SetOverride<i16>(	"i16_min",		std::numeric_limits<i16>::min());
	configSystem.SetOverride<i16>(	"i16_max",		std::numeric_limits<i16>::max());
	configSystem.SetOverride<i32>(	"i32_zero",		0);
	configSystem.SetOverride<i32>(	"i32_min",		std::numeric_limits<i32>::min());
	configSystem.SetOverride<i32>(	"i32_max",		std::numeric_limits<i32>::max());
	configSystem.SetOverride<i64>(	"i64_zero",		0);
	configSystem.SetOverride<i64>(	"i64_min",		std::numeric_limits<i64>::min());
	configSystem.SetOverride<i64>(	"i64_max",		std::numeric_limits<i64>::max());

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promiseSerialize, futureSerialize] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promiseSerialize), locTestConfigGroup);

	configSystem.ResetOverride("i8_zero");
	configSystem.ResetOverride("i8_min");
	configSystem.ResetOverride("i8_max");
	configSystem.ResetOverride("i16_zero");
	configSystem.ResetOverride("i16_min");
	configSystem.ResetOverride("i16_max");
	configSystem.ResetOverride("i32_zero");
	configSystem.ResetOverride("i32_min");
	configSystem.ResetOverride("i32_max");
	configSystem.ResetOverride("i64_zero");
	configSystem.ResetOverride("i64_min");
	configSystem.ResetOverride("i64_max");

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promiseDeserialize, futureDeserialize] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promiseDeserialize), locTestConfigGroup, *futureSerialize.GetResult());

	EXPECT_EQ(configSystem.Get<i8>(		"i8_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<i8>(		"i8_min",	dummyDefaultValue),	std::numeric_limits<i8>::min());
	EXPECT_EQ(configSystem.Get<i8>(		"i8_max",	dummyDefaultValue),	std::numeric_limits<i8>::max());
	EXPECT_EQ(configSystem.Get<i16>(	"i16_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<i16>(	"i16_min",	dummyDefaultValue),	std::numeric_limits<i16>::min());
	EXPECT_EQ(configSystem.Get<i16>(	"i16_max",	dummyDefaultValue),	std::numeric_limits<i16>::max());
	EXPECT_EQ(configSystem.Get<i32>(	"i32_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<i32>(	"i32_min",	dummyDefaultValue),	std::numeric_limits<i32>::min());
	EXPECT_EQ(configSystem.Get<i32>(	"i32_max",	dummyDefaultValue),	std::numeric_limits<i32>::max());
	EXPECT_EQ(configSystem.Get<i64>(	"i64_zero",	dummyDefaultValue),	0);
	EXPECT_EQ(configSystem.Get<i64>(	"i64_min",	dummyDefaultValue),	std::numeric_limits<i64>::min());
	EXPECT_EQ(configSystem.Get<i64>(	"i64_max",	dummyDefaultValue),	std::numeric_limits<i64>::max());
}

TEST(ConfigurationSerializationTests, floating_values_are_serialized_and_deserialized_correctly)
{
	constexpr auto dummyDefaultValue = 1.f;

	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<f32>(			"f32_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<f32>(			"f32_min",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<f32>(			"f32_max",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<f64>(			"f64_zero",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<f64>(			"f64_min",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<f64>(			"f64_max",		dummyDefaultValue,	locTestConfigGroup);

	configSystem.SetOverride<f32>(	"f32_zero",		0.f);
	configSystem.SetOverride<f32>(	"f32_min",		std::numeric_limits<f32>::min());
	configSystem.SetOverride<f32>(	"f32_max",		std::numeric_limits<f32>::max());
	configSystem.SetOverride<f64>(	"f64_zero",		0.f);
	configSystem.SetOverride<f64>(	"f64_min",		std::numeric_limits<f64>::min());
	configSystem.SetOverride<f64>(	"f64_max",		std::numeric_limits<f64>::max());

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promiseSerialize, futureSerialize] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promiseSerialize), locTestConfigGroup);

	configSystem.ResetOverride("f32_zero");
	configSystem.ResetOverride("f32_min");
	configSystem.ResetOverride("f32_max");
	configSystem.ResetOverride("f64_zero");
	configSystem.ResetOverride("f64_min");
	configSystem.ResetOverride("f64_max");

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promiseDeserialize, futureDeserialize] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promiseDeserialize), locTestConfigGroup, *futureSerialize.GetResult());

	EXPECT_EQ(configSystem.Get<f32>(	"f32_zero",	dummyDefaultValue),	0.f);
	EXPECT_EQ(configSystem.Get<f32>(	"f32_min",	dummyDefaultValue),	std::numeric_limits<f32>::min());
	EXPECT_EQ(configSystem.Get<f32>(	"f32_max",	dummyDefaultValue),	std::numeric_limits<f32>::max());
	EXPECT_EQ(configSystem.Get<f64>(	"f64_zero",	dummyDefaultValue),	0.f);
	EXPECT_EQ(configSystem.Get<f64>(	"f64_min",	dummyDefaultValue),	std::numeric_limits<f64>::min());
	EXPECT_EQ(configSystem.Get<f64>(	"f64_max",	dummyDefaultValue),	std::numeric_limits<f64>::max());
}

TEST(ConfigurationSerializationTests, string_values_are_serialized_and_deserialized_correctly)
{
	constexpr auto dummyDefaultValue = "default_string";

	NiceMock<ProcessSystemMock> processSystem;

	ConfigurationSystem::InjectDependencies(std::tie(processSystem));

	ConfigurationSystem configSystem;

	configSystem.Set<std::string>("string_empty",		dummyDefaultValue,	locTestConfigGroup);
	configSystem.Set<std::string>("string_panagram",	dummyDefaultValue,	locTestConfigGroup);

	configSystem.SetOverride<std::string>("string_empty",		"");
	configSystem.SetOverride<std::string>("string_panagram",	"sphinx of black quartz, judge my vow.");

	SerializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	SerializeConfigurationJob serializeJob;
	auto [promiseSerialize, futureSerialize] = MakeAsync<SerializeConfigurationJob::Output>();

	serializeJob.Run(std::move(promiseSerialize), locTestConfigGroup);

	configSystem.ResetOverride("string_empty");
	configSystem.ResetOverride("string_panagram");

	DeserializeConfigurationJob::InjectDependencies(std::tie(configSystem));

	DeserializeConfigurationJob deserializeJob;
	auto [promiseDeserialize, futureDeserialize] = MakeAsync<DeserializeConfigurationJob::Output>();

	deserializeJob.Run(std::move(promiseDeserialize), locTestConfigGroup, *futureSerialize.GetResult());

	EXPECT_EQ(configSystem.Get<std::string>("string_empty",		dummyDefaultValue),	"");
	EXPECT_EQ(configSystem.Get<std::string>("string_panagram",	dummyDefaultValue),	"sphinx of black quartz, judge my vow.");
}
