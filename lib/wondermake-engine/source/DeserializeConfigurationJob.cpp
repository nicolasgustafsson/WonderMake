#include "wondermake-engine/DeserializeConfigurationJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobGlobal.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include "json/json.hpp"

#include <magic_enum.hpp>

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
		WmLogError(TagWonderMake << TagWmConfiguration << "Failed to parse string to json, " << e.what() << '.');

		aPromise.Complete(Err(EDeserializeConfigurationError::ParsingError));

		return;
	}

	auto itOverrides = j.find("overrides");

	if (itOverrides == j.cend()
		|| !itOverrides->is_object())
	{
		WmLogError(TagWonderMake << TagWmConfiguration << "Missing overrides object.");

		aPromise.Complete(Err(EDeserializeConfigurationError::InvalidJsonStructure));

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
				static constexpr bool raw = true;

				// Boolean
				if constexpr (std::is_same_v<Type, bool>)
				{
					if (value.is_boolean())
						configuration.SetOverride<bool, raw>(name, value.get<bool>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected boolean value for override: \"" << name << "\".");
				}

				// Floating-point number
				else if constexpr (std::is_same_v<Type, f32>)
				{
					if (value.is_number())
						configuration.SetOverride<f32, raw>(name, value.get<f32>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected f32 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, f64>)
				{
					if (value.is_number())
						configuration.SetOverride<f64, raw>(name, value.get<f64>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected f64 value for override: \"" << name << "\".");
				}

				// Unsigned integer
				else if constexpr (std::is_same_v<Type, u8>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u8, raw>(name, value.get<u8>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected u8 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, u16>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u16, raw>(name, value.get<u16>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected u16 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, u32>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u32, raw>(name, value.get<u32>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected u32 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, u64>)
				{
					if (value.is_number_unsigned())
						configuration.SetOverride<u64, raw>(name, value.get<u64>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected u64 value for override: \"" << name << "\".");
				}

				// Signed integer
				else if constexpr (std::is_same_v<Type, i8>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i8, raw>(name, value.get<i8>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected i8 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, i16>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i16, raw>(name, value.get<i16>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected i16 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, i32>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i32, raw>(name, value.get<i32>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected i32 value for override: \"" << name << "\".");
				}
				else if constexpr (std::is_same_v<Type, i64>)
				{
					if (value.is_number_integer())
						configuration.SetOverride<i64, raw>(name, value.get<i64>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected i64 value for override: \"" << name << "\".");
				}

				// String
				else if constexpr (std::is_same_v<Type, std::string>)
				{
					if (value.is_string())
						configuration.SetOverride<std::string, raw>(name, value.get<std::string>());
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected string value for override: \"" << name << "\".");
				}

				// FilePath
				else if constexpr (std::is_same_v<Type, FilePath>)
				{
					if (value.is_object())
					{
						const auto location = magic_enum::enum_cast<FilePath::EFolder>(value["location"].get<std::string>());

						if (!location)
							return;

						configuration.SetOverride<FilePath, raw>(name, FilePath(*location, value["path"].get<std::string>()));
					}
					else
						WmLogWarning(TagWonderMake << TagWmConfiguration << "Expected string value for override: \"" << name << "\".");
				}

				else
					static_assert(false, "Unknown configuration type.");

			}, itConfig->second);
	}

	aPromise.Complete(Ok());
}
