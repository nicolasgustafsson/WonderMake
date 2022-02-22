#include "wondermake-engine/DeserializeConfigurationJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobGlobal.h"
#include "wondermake-base/Logger.h"

#include "json/json.hpp"

WM_REGISTER_JOB(DeserializeConfigurationJob);

void DeserializeConfigurationJob::Run(Promise<Output> aPromise, EConfigGroup aConfigGroup, std::string aJsonBlob)
{
	json j;

	try
	{
		j = json::parse(std::move(aJsonBlob));
	}
	catch ([[maybe_unused]] json::exception& e)
	{
		WM_LOG_ERROR("Failed to parse string to json, ", e.what(), ".");

		aPromise.Complete(EDeserializeConfigurationError::ParsingError);

		return;
	}

	auto itOverrides = j.find("overrides");

	if (itOverrides == j.cend()
		|| !itOverrides->is_object())
	{
		WM_LOG_ERROR("Missing overrides object.");

		aPromise.Complete(EDeserializeConfigurationError::InvalidJsonStructure);

		return;
	}

	auto&& configuration = Get<ConfigurationSystem>();

	auto configs = configuration.GetConfigs();

	for (auto&& [name, value] : itOverrides->items())
	{
		auto itConfig = configs.find(name);

		if (itConfig == configs.cend())
			continue;

		const bool isOfConfigGroup = std::visit([aConfigGroup](auto&& aConfigElement)
			{
				return aConfigGroup == aConfigElement.ConfigGroup;
			}, itConfig->second);

		if (!isOfConfigGroup)
			continue;

		std::visit([&configuration, &name, &value](auto&& aConfigElement)
			{
				using Type = decltype(aConfigElement.Value);

				// Boolean
				if constexpr (std::is_same_v<Type, bool>)
				{
					if (value.is_boolean())
						configuration.SetOverride<bool>(name, value.get<bool>());
					else
						WM_LOG_WARNING("Expected boolean value for override: \"", name, "\".");
				}

				// Floating-point number
				else if constexpr (std::is_same_v<Type, f32>)
				{
					if (value.is_number())
						configuration.SetOverride<f32>(name, value.get<f32>());
					else
						WM_LOG_WARNING("Expected f32 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, f64>)
				{
					if (value.is_number())
						configuration.SetOverride<f64>(name, value.get<f64>());
					else
						WM_LOG_WARNING("Expected f64 value for override: \"", name, "\".");
				}

				// Unsigned integer
				else if constexpr (std::is_same_v<Type, u8>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u8>(name, value.get<u8>());
					else
						WM_LOG_WARNING("Expected u8 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, u16>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u16>(name, value.get<u16>());
					else
						WM_LOG_WARNING("Expected u16 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, u32>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u32>(name, value.get<u32>());
					else
						WM_LOG_WARNING("Expected u32 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, u64>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u64>(name, value.get<u64>());
					else
						WM_LOG_WARNING("Expected u64 value for override: \"", name, "\".");
				}

				// Signed integer
				else if constexpr (std::is_same_v<Type, i8>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i8>(name, value.get<i8>());
					else
						WM_LOG_WARNING("Expected i8 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, i16>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i16>(name, value.get<i16>());
					else
						WM_LOG_WARNING("Expected i16 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, i32>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i32>(name, value.get<i32>());
					else
						WM_LOG_WARNING("Expected i32 value for override: \"", name, "\".");
				}
				else if constexpr (std::is_same_v<Type, i64>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i64>(name, value.get<i64>());
					else
						WM_LOG_WARNING("Expected i64 value for override: \"", name, "\".");
				}

				// String
				else if constexpr (std::is_same_v<Type, std::string>)
				{
					if (value.is_string())
						configuration.SetOverride<std::string>(name, value.get<std::string>());
					else
						WM_LOG_WARNING("Expected string value for override: \"", name, "\".");
				}

				else
					static_assert(false, "Unknown configuration type.");

			}, itConfig->second);
	}

	aPromise.Complete(Success);
}
