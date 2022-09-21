#include "ConfigurationProtoSerialization.h"

#include "wondermake-base/Configuration.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/ConfigurationRemote.pb.h"
#pragma warning(pop)

using ProtoConfigurationRemote::EDataType;

void SerializeConfigurationToProto(const Configuration& aConfiguration, ProtoConfigurationRemote::InstanceConfig& aOutProtoConfig)
{
	const auto& configs = aConfiguration.GetConfigs();

	for (const auto&[id, config] : configs)
	{
		auto& protoConfig = (*aOutProtoConfig.mutable_configs())[id];

		protoConfig.set_id(id);
		
		std::visit([&protoConfig](const auto& aConfigData)
			{
				using Type = std::decay_t<decltype(aConfigData.Value)>;

				constexpr auto convertConfigGroupEnum = [](const EConfigGroup aGroupConfig)
				{
					switch (aGroupConfig)
					{
					case EConfigGroup::Application:	return ProtoConfigurationRemote::EConfigGroup::Application;
					case EConfigGroup::User:		return ProtoConfigurationRemote::EConfigGroup::User;
					case EConfigGroup::Device:		return ProtoConfigurationRemote::EConfigGroup::Device;
					}

					return ProtoConfigurationRemote::EConfigGroup::Application;
				};
				constexpr auto getValueDataType = []()
				{
					if constexpr (std::is_same_v<Type, bool>)
						return EDataType::Bool;
					if constexpr (std::is_same_v<Type, f32>)
						return EDataType::F32;
					if constexpr (std::is_same_v<Type, f64>)
						return EDataType::F64;
					if constexpr (std::is_same_v<Type, u8>)
						return EDataType::U8;
					if constexpr (std::is_same_v<Type, u16>)
						return EDataType::U16;
					if constexpr (std::is_same_v<Type, u32>)
						return EDataType::U32;
					if constexpr (std::is_same_v<Type, u64>)
						return EDataType::U64;
					if constexpr (std::is_same_v<Type, i8>)
						return EDataType::I8;
					if constexpr (std::is_same_v<Type, i16>)
						return EDataType::I16;
					if constexpr (std::is_same_v<Type, i32>)
						return EDataType::I32;
					if constexpr (std::is_same_v<Type, i64>)
						return EDataType::I64;
					if constexpr (std::is_same_v<Type, std::string>)
						return EDataType::String;
				};
				constexpr auto setConfigValue = [](auto& aProto, const auto& aValue)
				{
					if constexpr (std::is_same_v<Type, bool>)
						aProto.set_config_bool(aValue);
					else if constexpr (std::is_floating_point_v<Type>)
						aProto.set_config_f64(aValue);
					else if constexpr (std::is_unsigned_v<Type>)
						aProto.set_config_u64(aValue);
					else if constexpr (std::is_signed_v<Type>)
						aProto.set_config_i64(aValue);
					else if constexpr (std::is_same_v<Type, std::string>)
							aProto.set_config_string(aValue);
				};
				constexpr auto setOverrideValue = [](auto& aProto, const auto& aValue)
				{
					if constexpr (std::is_same_v<Type, bool>)
						aProto.set_override_bool(aValue);
					else if constexpr (std::is_floating_point_v<Type>)
						aProto.set_override_f64(aValue);
					else if constexpr (std::is_unsigned_v<Type>)
						aProto.set_override_u64(aValue);
					else if constexpr (std::is_signed_v<Type>)
						aProto.set_override_i64(aValue);
					else if constexpr (std::is_same_v<Type, std::string>)
						aProto.set_override_string(aValue);
				};
				constexpr auto setRestrictionValue = [](auto& aProto, const auto& aValue)
				{
					if constexpr (std::is_same_v<Type, bool>)
						aProto.set_value_bool(aValue);
					else if constexpr (std::is_floating_point_v<Type>)
						aProto.set_value_f64(aValue);
					else if constexpr (std::is_unsigned_v<Type>)
						aProto.set_value_u64(aValue);
					else if constexpr (std::is_signed_v<Type>)
						aProto.set_value_i64(aValue);
					else if constexpr (std::is_same_v<Type, std::string>)
						aProto.set_value_string(aValue);
				};

				protoConfig.set_value_data_type(getValueDataType());
				setConfigValue(protoConfig, aConfigData.Value);
				protoConfig.set_config_group(convertConfigGroupEnum(aConfigData.ConfigGroup));
				if (aConfigData.Override)
					setOverrideValue(protoConfig, *aConfigData.Override);
				if (!aConfigData.AllowedValues.empty())
				{
					auto& restriction = *protoConfig.mutable_restriction_allowed_values();

					for (const auto&[name, value] : aConfigData.AllowedValues)
					{
						auto& allowedValue = (*restriction.mutable_allowed_values())[name];

						setRestrictionValue(allowedValue, value);
					}
				}
				if (aConfigData.MemoryRatio)
				{
					auto& memoryUnit = *protoConfig.mutable_type_data_memory_unit();

					memoryUnit.set_ratio(static_cast<std::underlying_type_t<EMemoryRatio>>(*aConfigData.MemoryRatio));
				}
			}, config);
	}
}

void DeserializeConfigurationFromProto(const ProtoConfigurationRemote::InstanceConfig& aProtoConfig, Configuration& aOutConfiguration)
{
	constexpr auto setConfiguration = [](const auto& aType, const auto& aProto, Configuration& aOutConfiguration)
	{
		using Type = std::decay_t<decltype(aType)>;

		constexpr auto convertConfigGroupEnum = [](const ProtoConfigurationRemote::EConfigGroup aGroupConfig)
		{
			switch (aGroupConfig)
			{
			case ProtoConfigurationRemote::EConfigGroup::Application:	return EConfigGroup::Application;
			case ProtoConfigurationRemote::EConfigGroup::User:			return EConfigGroup::User;
			case ProtoConfigurationRemote::EConfigGroup::Device:		return EConfigGroup::Device;
			default:													return EConfigGroup::Application;
			}
		};
		constexpr auto getConfig = [](const auto& aProto)
		{
			if constexpr (std::is_same_v<Type, bool>)
				return aProto.config_bool();
			else if constexpr (std::is_floating_point_v<Type>)
				return static_cast<Type>(aProto.config_f64());
			else if constexpr (std::is_unsigned_v<Type>)
				return static_cast<Type>(aProto.config_u64());
			else if constexpr (std::is_signed_v<Type>)
				return static_cast<Type>(aProto.config_i64());
			else if constexpr (std::is_same_v<Type, std::string>)
				return aProto.config_string();
		};
		constexpr auto getValue = [](const auto& aProto)
		{
			if constexpr (std::is_same_v<Type, bool>)
				return aProto.value_bool();
			else if constexpr (std::is_floating_point_v<Type>)
				return static_cast<Type>(aProto.value_f64());
			else if constexpr (std::is_unsigned_v<Type>)
				return static_cast<Type>(aProto.value_u64());
			else if constexpr (std::is_signed_v<Type>)
				return static_cast<Type>(aProto.value_i64());
			else if constexpr (std::is_same_v<Type, std::string>)
				return aProto.value_string();
		};
		constexpr auto getOverride = [](const auto& aProto) -> std::optional<Type>
		{
			if constexpr (std::is_same_v<Type, bool>)
			{
				if (aProto.has_override_bool())
					return aProto.override_bool();
				else
					return std::nullopt;
			}
			else if constexpr (std::is_floating_point_v<Type>)
			{
				if (aProto.has_override_f64())
					return static_cast<Type>(aProto.override_f64());
				else
					return std::nullopt;
			}
			else if constexpr (std::is_unsigned_v<Type>)
			{
				if (aProto.has_override_u64())
					return static_cast<Type>(aProto.override_u64());
				else
					return std::nullopt;
			}
			else if constexpr (std::is_signed_v<Type>)
			{
				if (aProto.has_override_i64())
					return static_cast<Type>(aProto.override_i64());
				else
					return std::nullopt;
			}
			else if constexpr (std::is_same_v<Type, std::string>)
			{
				if (aProto.has_override_string())
					return aProto.override_string();
				else
					return std::nullopt;
			}
		};
		
		const auto& id = aProto.id();

		if (aProto.has_restriction_allowed_values())
		{
			Configuration::AllowedValues<Type> allowedValues;

			for (const auto& [name, allowedValue] : aProto.restriction_allowed_values().allowed_values())
				allowedValues.Map.emplace(std::make_pair(name, getValue(allowedValue)));

			aOutConfiguration.Set<Type>(id, getConfig(aProto), convertConfigGroupEnum(aProto.config_group()), std::move(allowedValues));
		}
		else
			aOutConfiguration.Set<Type>(id, getConfig(aProto), convertConfigGroupEnum(aProto.config_group()));

		auto overrideValue = getOverride(aProto);

		if (overrideValue)
			aOutConfiguration.SetOverride<Type>(id, std::move(*overrideValue));

		if (aProto.has_type_data_memory_unit())
			aOutConfiguration.SetMemoryRatio(id, static_cast<EMemoryRatio>(aProto.type_data_memory_unit().ratio()));
	};

	for (const auto& [id, protoConfig] : aProtoConfig.configs())
	{
		switch (protoConfig.value_data_type())
		{
		case EDataType::Bool:	setConfiguration(bool(),		protoConfig, aOutConfiguration); break;
		case EDataType::F32:	setConfiguration(f32(),			protoConfig, aOutConfiguration); break;
		case EDataType::F64: 	setConfiguration(f64(),			protoConfig, aOutConfiguration); break;
		case EDataType::U8: 	setConfiguration(u8(),			protoConfig, aOutConfiguration); break;
		case EDataType::U16:	setConfiguration(u16(),			protoConfig, aOutConfiguration); break;
		case EDataType::U32:	setConfiguration(u32(),			protoConfig, aOutConfiguration); break;
		case EDataType::U64:	setConfiguration(u64(),			protoConfig, aOutConfiguration); break;
		case EDataType::I8:		setConfiguration(i8(),			protoConfig, aOutConfiguration); break;
		case EDataType::I16:	setConfiguration(i16(),			protoConfig, aOutConfiguration); break;
		case EDataType::I32:	setConfiguration(i32(),			protoConfig, aOutConfiguration); break;
		case EDataType::I64:	setConfiguration(i64(),			protoConfig, aOutConfiguration); break;
		case EDataType::String:	setConfiguration(std::string(),	protoConfig, aOutConfiguration); break;
		default: continue;
		}
	}
}
