#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/Configuration.h"

#include "ConfigurationProtoSerialization.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/ConfigurationRemote.pb.h"
#pragma warning(pop)

using ProtoConfigurationRemote::EDataType;

template<typename TType>
bool HasValue(const auto& aProtoList, const auto& aId, const std::optional<TType>& aValue, const std::optional<ProtoConfigurationRemote::EConfigGroup> aGroup = std::nullopt, const std::optional<TType>& aOverride = std::nullopt)
{
	constexpr auto getExpectedValueDataType = []()
	{
		if constexpr (std::is_same_v<TType, bool>)
			return EDataType::Bool;
		if constexpr (std::is_same_v<TType, f32>)
			return EDataType::F32;
		if constexpr (std::is_same_v<TType, f64>)
			return EDataType::F64;
		if constexpr (std::is_same_v<TType, u8>)
			return EDataType::U8;
		if constexpr (std::is_same_v<TType, u16>)
			return EDataType::U16;
		if constexpr (std::is_same_v<TType, u32>)
			return EDataType::U32;
		if constexpr (std::is_same_v<TType, u64>)
			return EDataType::U64;
		if constexpr (std::is_same_v<TType, i8>)
			return EDataType::I8;
		if constexpr (std::is_same_v<TType, i16>)
			return EDataType::I16;
		if constexpr (std::is_same_v<TType, i32>)
			return EDataType::I32;
		if constexpr (std::is_same_v<TType, i64>)
			return EDataType::I64;
		if constexpr (std::is_same_v<TType, std::string>)
			return EDataType::String;
	};
	constexpr auto getConfigValue = [](const auto& aProto)
	{
		if constexpr (std::is_same_v<TType, bool>)
			return aProto.config_bool();
		else if constexpr (std::is_floating_point_v<TType>)
			return aProto.config_f64();
		else if constexpr (std::is_unsigned_v<TType>)
			return aProto.config_u64();
		else if constexpr (std::is_signed_v<TType>)
			return aProto.config_i64();
		else if constexpr (std::is_same_v<TType, std::string>)
			return aProto.config_string();
	};
	constexpr auto getOverrideValue = [](const auto& aProto) -> std::optional<TType>
	{
		if constexpr (std::is_same_v<TType, bool>)
		{
			if (aProto.has_override_bool())
				return aProto.override_bool();
			else
				return std::nullopt;
		}
		if constexpr (std::is_floating_point_v<TType>)
		{
			if (aProto.has_override_f64())
				return static_cast<TType>(aProto.override_f64());
			else
				return std::nullopt;
		}
		if constexpr (std::is_unsigned_v<TType>)
		{
			if (aProto.has_override_u64())
				return static_cast<TType>(aProto.override_u64());
			else
				return std::nullopt;
		}
		if constexpr (std::is_signed_v<TType>)
		{
			if (aProto.has_override_i64())
				return static_cast<TType>(aProto.override_i64());
			else
				return std::nullopt;
		}
		if constexpr (std::is_same_v<TType, std::string>)
		{
			if (aProto.has_override_string())
				return aProto.override_string();
			else
				return std::nullopt;
		}
	};
	
	const auto it = aProtoList.find(aId);

	if (it == aProtoList.end())
		return false;

	const auto& protoConfig = it->second;

	return (protoConfig.value_data_type() == getExpectedValueDataType())
		&& (!aValue || getConfigValue(protoConfig) == aValue)
		&& (!aGroup || protoConfig.config_group() == aGroup)
		&& getOverrideValue(protoConfig) == aOverride;
}

template<typename TType>
bool HasRestrictValue(const auto& aProtoList, const auto& aId, const auto& aName, const TType& aValue)
{
	constexpr auto getExpectedValueDataType = []()
	{
		if constexpr (std::is_same_v<TType, bool>)
			return EDataType::Bool;
		if constexpr (std::is_same_v<TType, f32>)
			return EDataType::F32;
		if constexpr (std::is_same_v<TType, f64>)
			return EDataType::F64;
		if constexpr (std::is_same_v<TType, u8>)
			return EDataType::U8;
		if constexpr (std::is_same_v<TType, u16>)
			return EDataType::U16;
		if constexpr (std::is_same_v<TType, u32>)
			return EDataType::U32;
		if constexpr (std::is_same_v<TType, u64>)
			return EDataType::U64;
		if constexpr (std::is_same_v<TType, i8>)
			return EDataType::I8;
		if constexpr (std::is_same_v<TType, i16>)
			return EDataType::I16;
		if constexpr (std::is_same_v<TType, i32>)
			return EDataType::I32;
		if constexpr (std::is_same_v<TType, i64>)
			return EDataType::I64;
		if constexpr (std::is_same_v<TType, std::string>)
			return EDataType::String;
	};
	constexpr auto getRestrictValue = [](const auto& aProto)
	{
		if constexpr (std::is_same_v<TType, bool>)
			return aProto.value_bool();
		else if constexpr (std::is_floating_point_v<TType>)
			return aProto.value_f64();
		else if constexpr (std::is_unsigned_v<TType>)
			return aProto.value_u64();
		else if constexpr (std::is_signed_v<TType>)
			return aProto.value_i64();
		else if constexpr (std::is_same_v<TType, std::string>)
			return aProto.value_string();
	};

	const auto it = aProtoList.find(aId);

	if (it == aProtoList.end())
		return false;

	const auto& protoConfig = it->second;

	if (protoConfig.value_data_type() != getExpectedValueDataType())
		return false;

	const auto& protoAllowedValues = protoConfig.restriction_allowed_values().allowed_values();

	const auto allowedValuesIt = protoAllowedValues.find(aName);

	if (allowedValuesIt == protoAllowedValues.end())
		return false;

	return (getRestrictValue(allowedValuesIt->second) == aValue);
}
size_t RestrictValueCount(const auto& aProtoList, const auto& aId)
{
	const auto it = aProtoList.find(aId);

	if (it == aProtoList.end())
		return 0;

	const auto& protoConfig = it->second;

	return protoConfig.restriction_allowed_values().allowed_values_size();
}

TEST(ConfigurationProtoSerializationTests, serializing_an_empty_configuration_returns_an_empty_proto)
{
	Configuration config;
	ProtoConfigurationRemote::InstanceConfig proto;

	SerializeConfigurationToProto(config, proto);

	EXPECT_EQ(proto.configs_size(), 0);
}

TEST(ConfigurationProtoSerializationTests, all_value_types_can_be_serialized_into_proto)
{
	Configuration config;
	ProtoConfigurationRemote::InstanceConfig proto;

	config.Set<bool>(		"id_bool",		true,		EConfigGroup::Application);
	config.Set<f32>(		"id_f32",		1.f,		EConfigGroup::Application);
	config.Set<f64>(		"id_f64",		1.,			EConfigGroup::Application);
	config.Set<u8>(			"id_u8",		1,			EConfigGroup::Application);
	config.Set<u16>(		"id_u16",		2,			EConfigGroup::Application);
	config.Set<u32>(		"id_u32",		3,			EConfigGroup::Application);
	config.Set<u64>(		"id_u64",		4,			EConfigGroup::Application);
	config.Set<i8>(			"id_i8",		-1,			EConfigGroup::Application);
	config.Set<i16>(		"id_i16",		-2,			EConfigGroup::Application);
	config.Set<i32>(		"id_i32",		-3,			EConfigGroup::Application);
	config.Set<i64>(		"id_i64",		-4,			EConfigGroup::Application);
	config.Set<std::string>("id_string",	"value",	EConfigGroup::Application);

	SerializeConfigurationToProto(config, proto);

	EXPECT_TRUE(HasValue<bool>(			proto.configs(), "id_bool",		true));
	EXPECT_TRUE(HasValue<f32>(			proto.configs(), "id_f32",		1.f));
	EXPECT_TRUE(HasValue<f64>(			proto.configs(), "id_f64",		1.));
	EXPECT_TRUE(HasValue<u8>(			proto.configs(), "id_u8",		1));
	EXPECT_TRUE(HasValue<u16>(			proto.configs(), "id_u16",		2));
	EXPECT_TRUE(HasValue<u32>(			proto.configs(), "id_u32",		3));
	EXPECT_TRUE(HasValue<u64>(			proto.configs(), "id_u64",		4));
	EXPECT_TRUE(HasValue<i8>(			proto.configs(), "id_i8",		-1));
	EXPECT_TRUE(HasValue<i16>(			proto.configs(), "id_i16",		-2));
	EXPECT_TRUE(HasValue<i32>(			proto.configs(), "id_i32",		-3));
	EXPECT_TRUE(HasValue<i64>(			proto.configs(), "id_i64",		-4));
	EXPECT_TRUE(HasValue<std::string>(	proto.configs(), "id_string",	"value"));
}

TEST(ConfigurationProtoSerializationTests, all_config_groups_can_be_serialized_into_proto)
{
	Configuration config;
	ProtoConfigurationRemote::InstanceConfig proto;

	config.Set<bool>(		"id_bool",		true,		EConfigGroup::Application);
	config.Set<f32>(		"id_f32",		1.f,		EConfigGroup::User);
	config.Set<f64>(		"id_f64",		1.,			EConfigGroup::Device);

	SerializeConfigurationToProto(config, proto);

	EXPECT_TRUE(HasValue<bool>(			proto.configs(), "id_bool",		std::nullopt,	ProtoConfigurationRemote::EConfigGroup::Application));
	EXPECT_TRUE(HasValue<f32>(			proto.configs(), "id_f32",		std::nullopt,	ProtoConfigurationRemote::EConfigGroup::User));
	EXPECT_TRUE(HasValue<f64>(			proto.configs(), "id_f64",		std::nullopt,	ProtoConfigurationRemote::EConfigGroup::Device));
}

TEST(ConfigurationProtoSerializationTests, all_override_types_can_be_serialized_into_proto)
{
	Configuration config;
	ProtoConfigurationRemote::InstanceConfig proto;

	config.Set<bool>(		"id_bool",		true,		EConfigGroup::Application);
	config.Set<f32>(		"id_f32",		1.f,		EConfigGroup::Application);
	config.Set<f64>(		"id_f64",		1.,			EConfigGroup::Application);
	config.Set<u8>(			"id_u8",		1,			EConfigGroup::Application);
	config.Set<u16>(		"id_u16",		2,			EConfigGroup::Application);
	config.Set<u32>(		"id_u32",		3,			EConfigGroup::Application);
	config.Set<u64>(		"id_u64",		4,			EConfigGroup::Application);
	config.Set<i8>(			"id_i8",		-1,			EConfigGroup::Application);
	config.Set<i16>(		"id_i16",		-2,			EConfigGroup::Application);
	config.Set<i32>(		"id_i32",		-3,			EConfigGroup::Application);
	config.Set<i64>(		"id_i64",		-4,			EConfigGroup::Application);
	config.Set<std::string>("id_string",	"value",	EConfigGroup::Application);
	
	config.SetOverride<bool>(		"id_bool",		false);
	config.SetOverride<f32>(		"id_f32",		2.f);
	config.SetOverride<f64>(		"id_f64",		2.);
	config.SetOverride<u8>(			"id_u8",		5);
	config.SetOverride<u16>(		"id_u16",		6);
	config.SetOverride<u32>(		"id_u32",		7);
	config.SetOverride<u64>(		"id_u64",		8);
	config.SetOverride<i8>(			"id_i8",		-5);
	config.SetOverride<i16>(		"id_i16",		-6);
	config.SetOverride<i32>(		"id_i32",		-7);
	config.SetOverride<i64>(		"id_i64",		-8);
	config.SetOverride<std::string>("id_string",	"override");

	SerializeConfigurationToProto(config, proto);

	EXPECT_TRUE(HasValue<bool>(			proto.configs(), "id_bool",		std::nullopt,	std::nullopt,	false));
	EXPECT_TRUE(HasValue<f32>(			proto.configs(), "id_f32",		std::nullopt,	std::nullopt,	2.f));
	EXPECT_TRUE(HasValue<f64>(			proto.configs(), "id_f64",		std::nullopt,	std::nullopt,	2.));
	EXPECT_TRUE(HasValue<u8>(			proto.configs(), "id_u8",		std::nullopt,	std::nullopt,	5));
	EXPECT_TRUE(HasValue<u16>(			proto.configs(), "id_u16",		std::nullopt,	std::nullopt,	6));
	EXPECT_TRUE(HasValue<u32>(			proto.configs(), "id_u32",		std::nullopt,	std::nullopt,	7));
	EXPECT_TRUE(HasValue<u64>(			proto.configs(), "id_u64",		std::nullopt,	std::nullopt,	8));
	EXPECT_TRUE(HasValue<i8>(			proto.configs(), "id_i8",		std::nullopt,	std::nullopt,	-5));
	EXPECT_TRUE(HasValue<i16>(			proto.configs(), "id_i16",		std::nullopt,	std::nullopt,	-6));
	EXPECT_TRUE(HasValue<i32>(			proto.configs(), "id_i32",		std::nullopt,	std::nullopt,	-7));
	EXPECT_TRUE(HasValue<i64>(			proto.configs(), "id_i64",		std::nullopt,	std::nullopt,	-8));
	EXPECT_TRUE(HasValue<std::string>(	proto.configs(), "id_string",	std::nullopt,	std::nullopt,	"override"));
}

TEST(ConfigurationProtoSerializationTests, all_restrict_types_can_be_serialized_into_proto)
{
	Configuration config;
	ProtoConfigurationRemote::InstanceConfig proto;

	config.Set<bool>(		"id_bool",		true,		EConfigGroup::Application, Configuration::AllowedValues<bool>			{ { { "TRUE", true }, { "FALSE", false } } });
	config.Set<f32>(		"id_f32",		1.f,		EConfigGroup::Application, Configuration::AllowedValues<f32>			{ { { "ZERO", 0.f }, { "ONE", 1.f } } });
	config.Set<f64>(		"id_f64",		1.,			EConfigGroup::Application, Configuration::AllowedValues<f64>			{ { { "ZERO", 0. }, { "ONE", 1. } } });
	config.Set<u8>(			"id_u8",		1,			EConfigGroup::Application, Configuration::AllowedValues<u8>				{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<u16>(		"id_u16",		2,			EConfigGroup::Application, Configuration::AllowedValues<u16>			{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<u32>(		"id_u32",		3,			EConfigGroup::Application, Configuration::AllowedValues<u32>			{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<u64>(		"id_u64",		4,			EConfigGroup::Application, Configuration::AllowedValues<u64>			{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<i8>(			"id_i8",		-1,			EConfigGroup::Application, Configuration::AllowedValues<i8>				{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<i16>(		"id_i16",		-2,			EConfigGroup::Application, Configuration::AllowedValues<i16>			{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<i32>(		"id_i32",		-3,			EConfigGroup::Application, Configuration::AllowedValues<i32>			{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<i64>(		"id_i64",		-4,			EConfigGroup::Application, Configuration::AllowedValues<i64>			{ { { "ZERO", 0 }, { "ONE", 1 } } });
	config.Set<std::string>("id_string",	"value",	EConfigGroup::Application, Configuration::AllowedValues<std::string>	{ { { "FIRST", "one" }, { "SECOND", "two" }, { "THIRD", "three" } } });

	SerializeConfigurationToProto(config, proto);

	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_bool"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_f32"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_f64"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_u8"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_u16"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_u32"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_u64"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_i8"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_i16"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_i32"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_i64"),		2);
	EXPECT_EQ(RestrictValueCount(proto.configs(),	"id_string"),	3);

	EXPECT_TRUE(HasRestrictValue<bool>(			proto.configs(), "id_bool",		"TRUE",		true));
	EXPECT_TRUE(HasRestrictValue<bool>(			proto.configs(), "id_bool",		"FALSE",	false));
	EXPECT_TRUE(HasRestrictValue<f32>(			proto.configs(), "id_f32",		"ZERO",		0.f));
	EXPECT_TRUE(HasRestrictValue<f32>(			proto.configs(), "id_f32",		"ONE",		1.f));
	EXPECT_TRUE(HasRestrictValue<f64>(			proto.configs(), "id_f64",		"ZERO",		0.));
	EXPECT_TRUE(HasRestrictValue<f64>(			proto.configs(), "id_f64",		"ONE",		1.));
	EXPECT_TRUE(HasRestrictValue<u8>(			proto.configs(), "id_u8",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<u8>(			proto.configs(), "id_u8",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<u16>(			proto.configs(), "id_u16",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<u16>(			proto.configs(), "id_u16",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<u32>(			proto.configs(), "id_u32",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<u32>(			proto.configs(), "id_u32",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<u64>(			proto.configs(), "id_u64",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<u64>(			proto.configs(), "id_u64",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<i8>(			proto.configs(), "id_i8",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<i8>(			proto.configs(), "id_i8",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<i16>(			proto.configs(), "id_i16",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<i16>(			proto.configs(), "id_i16",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<i32>(			proto.configs(), "id_i32",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<i32>(			proto.configs(), "id_i32",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<i64>(			proto.configs(), "id_i64",		"ZERO",		0));
	EXPECT_TRUE(HasRestrictValue<i64>(			proto.configs(), "id_i64",		"ONE",		1));
	EXPECT_TRUE(HasRestrictValue<std::string>(	proto.configs(), "id_string",	"FIRST",	"one"));
	EXPECT_TRUE(HasRestrictValue<std::string>(	proto.configs(), "id_string",	"SECOND",	"two"));
	EXPECT_TRUE(HasRestrictValue<std::string>(	proto.configs(), "id_string",	"THIRD",	"three"));
}

TEST(ConfigurationProtoSerializationTests, all_memory_unit_ratios_can_be_serialized_into_proto)
{
	Configuration config;
	ProtoConfigurationRemote::InstanceConfig proto;

	config.Set<MemoryUnit<EMemoryRatio::B,		u64>>(	"id_B",		0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::KB,		u64>>(	"id_KB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::MB,		u64>>(	"id_MB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::GB,		u64>>(	"id_GB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::TB,		u64>>(	"id_TB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::KiB,	u64>>(	"id_KiB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::MiB,	u64>>(	"id_MiB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::GiB,	u64>>(	"id_GiB",	0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::TiB,	u64>>(	"id_TiB",	0, EConfigGroup::Application);

	SerializeConfigurationToProto(config, proto);

	const auto& configs = proto.configs();
	static constexpr auto hasRatio = [](const auto& aConfigs, const auto aId, EMemoryRatio aRatio)
	{
		const auto it = aConfigs.find(aId);

		return it != aConfigs.end()
			&& it->second.has_type_data_memory_unit()
			&& it->second.type_data_memory_unit().ratio() == static_cast<std::underlying_type_t<EMemoryRatio>>(aRatio);
	};

	EXPECT_TRUE(hasRatio(configs, "id_B",	EMemoryRatio::B));
	EXPECT_TRUE(hasRatio(configs, "id_KB",	EMemoryRatio::KB));
	EXPECT_TRUE(hasRatio(configs, "id_MB",	EMemoryRatio::MB));
	EXPECT_TRUE(hasRatio(configs, "id_GB",	EMemoryRatio::GB));
	EXPECT_TRUE(hasRatio(configs, "id_TB",	EMemoryRatio::TB));
	EXPECT_TRUE(hasRatio(configs, "id_KiB",	EMemoryRatio::KiB));
	EXPECT_TRUE(hasRatio(configs, "id_MiB",	EMemoryRatio::MiB));
	EXPECT_TRUE(hasRatio(configs, "id_GiB",	EMemoryRatio::GiB));
	EXPECT_TRUE(hasRatio(configs, "id_TiB",	EMemoryRatio::TiB));
}

TEST(ConfigurationProtoSerializationTests, deserializing_an_empty_proto_returns_an_empty_configuration)
{
	ProtoConfigurationRemote::InstanceConfig proto;
	Configuration config;

	DeserializeConfigurationFromProto(proto, config);

	EXPECT_TRUE(config.GetConfigs().empty());
}

TEST(ConfigurationProtoSerializationTests, entire_config_proto_can_be_deserialized_into_configuration)
{
	ProtoConfigurationRemote::InstanceConfig proto;
	Configuration config;
	Configuration expectedConfig;
	
	expectedConfig.Set<bool>(		"id_user",		false,		EConfigGroup::User);
	expectedConfig.Set<bool>(		"id_device",	false,		EConfigGroup::Device);
	expectedConfig.Set<bool>(		"id_bool",		true,		EConfigGroup::Application, Configuration::AllowedValues<bool>			{ { { "TRUE", true },	{ "FALSE", false } } });
	expectedConfig.Set<f32>(		"id_f32",		1.1f,		EConfigGroup::Application, Configuration::AllowedValues<f32>			{ { { "ZERO", 0.f },	{ "ONE", 1.f } } });
	expectedConfig.Set<f64>(		"id_f64",		1.2,		EConfigGroup::Application, Configuration::AllowedValues<f64>			{ { { "ZERO", 0. },		{ "ONE", 1. } } });
	expectedConfig.Set<u8>(			"id_u8",		1,			EConfigGroup::Application, Configuration::AllowedValues<u8>				{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<u16>(		"id_u16",		2,			EConfigGroup::Application, Configuration::AllowedValues<u16>			{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<u32>(		"id_u32",		3,			EConfigGroup::Application, Configuration::AllowedValues<u32>			{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<u64>(		"id_u64",		4,			EConfigGroup::Application, Configuration::AllowedValues<u64>			{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<i8>(			"id_i8",		-1,			EConfigGroup::Application, Configuration::AllowedValues<i8>				{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<i16>(		"id_i16",		-2,			EConfigGroup::Application, Configuration::AllowedValues<i16>			{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<i32>(		"id_i32",		-3,			EConfigGroup::Application, Configuration::AllowedValues<i32>			{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<i64>(		"id_i64",		-4,			EConfigGroup::Application, Configuration::AllowedValues<i64>			{ { { "ZERO", 0 },		{ "ONE", 1 } } });
	expectedConfig.Set<std::string>("id_string",	"value",	EConfigGroup::Application, Configuration::AllowedValues<std::string>	{ { { "FIRST", "one" },	{ "SECOND", "two" }, { "THIRD", "three" } } });
	
	expectedConfig.SetOverride<bool>(		"id_bool",		false);
	expectedConfig.SetOverride<f32>(		"id_f32",		2.1f);
	expectedConfig.SetOverride<f64>(		"id_f64",		2.2);
	expectedConfig.SetOverride<u8>(			"id_u8",		5);
	expectedConfig.SetOverride<u16>(		"id_u16",		6);
	expectedConfig.SetOverride<u32>(		"id_u32",		7);
	expectedConfig.SetOverride<u64>(		"id_u64",		8);
	expectedConfig.SetOverride<i8>(			"id_i8",		-5);
	expectedConfig.SetOverride<i16>(		"id_i16",		-6);
	expectedConfig.SetOverride<i32>(		"id_i32",		-7);
	expectedConfig.SetOverride<i64>(		"id_i64",		-8);
	expectedConfig.SetOverride<std::string>("id_string",	"override");

	{
		auto& protoConfig = (*proto.mutable_configs())["id_user"];

		protoConfig.set_id("id_user");
		protoConfig.set_value_data_type(EDataType::Bool);
		protoConfig.set_config_bool(false);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::User);
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_device"];

		protoConfig.set_id("id_device");
		protoConfig.set_value_data_type(EDataType::Bool);
		protoConfig.set_config_bool(false);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Device);
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_bool"];

		protoConfig.set_id("id_bool");
		protoConfig.set_value_data_type(EDataType::Bool);
		protoConfig.set_config_bool(true);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_bool(false);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["TRUE"];

			allowedValue.set_value_bool(true);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["FALSE"];

			allowedValue.set_value_bool(false);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_f32"];

		protoConfig.set_id("id_f32");
		protoConfig.set_value_data_type(EDataType::F32);
		protoConfig.set_config_f64(1.1f);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_f64(2.1f);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_f64(0.f);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_f64(1.f);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_f64"];

		protoConfig.set_id("id_f64");
		protoConfig.set_value_data_type(EDataType::F64);
		protoConfig.set_config_f64(1.2);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_f64(2.2);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_f64(0.f);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_f64(1.f);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_u8"];

		protoConfig.set_id("id_u8");
		protoConfig.set_value_data_type(EDataType::U8);
		protoConfig.set_config_u64(1);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_u64(5);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_u64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_u64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_u16"];

		protoConfig.set_id("id_u16");
		protoConfig.set_value_data_type(EDataType::U16);
		protoConfig.set_config_u64(2);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_u64(6);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_u64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_u64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_u32"];

		protoConfig.set_id("id_u32");
		protoConfig.set_value_data_type(EDataType::U32);
		protoConfig.set_config_u64(3);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_u64(7);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_u64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_u64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_u64"];

		protoConfig.set_id("id_u64");
		protoConfig.set_value_data_type(EDataType::U64);
		protoConfig.set_config_u64(4);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_u64(8);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_u64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_u64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_i8"];

		protoConfig.set_id("id_i8");
		protoConfig.set_value_data_type(EDataType::I8);
		protoConfig.set_config_i64(-1);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_i64(-5);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_i64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_i64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_i16"];

		protoConfig.set_id("id_i16");
		protoConfig.set_value_data_type(EDataType::I16);
		protoConfig.set_config_i64(-2);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_i64(-6);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_i64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_i64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_i32"];

		protoConfig.set_id("id_i32");
		protoConfig.set_value_data_type(EDataType::I32);
		protoConfig.set_config_i64(-3);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_i64(-7);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_i64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_i64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_i64"];

		protoConfig.set_id("id_i64");
		protoConfig.set_value_data_type(EDataType::I64);
		protoConfig.set_config_i64(-4);
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_i64(-8);
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ZERO"];

			allowedValue.set_value_i64(0);
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["ONE"];

			allowedValue.set_value_i64(1);
		}
	}
	{
		auto& protoConfig = (*proto.mutable_configs())["id_string"];

		protoConfig.set_id("id_string");
		protoConfig.set_value_data_type(EDataType::String);
		protoConfig.set_config_string("value");
		protoConfig.set_config_group(ProtoConfigurationRemote::EConfigGroup::Application);
		protoConfig.set_override_string("override");
		auto& restrictedValue = *protoConfig.mutable_restriction_allowed_values();

		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["FIRST"];

			allowedValue.set_value_string("one");
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["SECOND"];

			allowedValue.set_value_string("two");
		}
		{
			auto& allowedValue = (*restrictedValue.mutable_allowed_values())["THIRD"];

			allowedValue.set_value_string("three");
		}
	}

	DeserializeConfigurationFromProto(proto, config);

	const auto& configValues = config.GetConfigs();

	EXPECT_EQ(config, expectedConfig);
}

TEST(ConfigurationProtoSerializationTests, all_memory_unit_ratios_can_be_deserialized_into_configuration)
{
	Configuration config;
	Configuration configOut;
	ProtoConfigurationRemote::InstanceConfig proto;

	config.Set<MemoryUnit<EMemoryRatio::B, u64>>("id_B", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::KB, u64>>("id_KB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::MB, u64>>("id_MB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::GB, u64>>("id_GB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::TB, u64>>("id_TB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::KiB, u64>>("id_KiB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::MiB, u64>>("id_MiB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::GiB, u64>>("id_GiB", 0, EConfigGroup::Application);
	config.Set<MemoryUnit<EMemoryRatio::TiB, u64>>("id_TiB", 0, EConfigGroup::Application);

	SerializeConfigurationToProto(config, proto);

	DeserializeConfigurationFromProto(proto, configOut);

	EXPECT_EQ(config, configOut);
}
