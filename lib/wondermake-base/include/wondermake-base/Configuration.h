#pragma once

#include "wondermake-base/Logger.h"

#include "wondermake-utility/Typedefs.h"

#include <optional>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <variant>

template<typename TConfig>
concept CConfigType =
	!std::is_const_v<TConfig> && 
	!std::is_reference_v<TConfig> && (
		std::is_arithmetic_v<TConfig> ||
		std::is_enum_v<TConfig> ||
		std::is_same_v<std::string, TConfig>);

template<typename TConfig>
concept CConfigRaw =
	std::is_arithmetic_v<std::remove_const_t<TConfig>> ||
	std::is_same_v<std::decay_t<TConfig>, std::string>;

template<typename TConfig>
concept CConfig =
	std::is_arithmetic_v<std::remove_const_t<TConfig>> ||
	std::is_enum_v<std::remove_const_t<TConfig>> ||
	std::is_constructible_v<std::string, TConfig> ||
	std::is_convertible_v<TConfig, std::string> ||
	CConfigRaw<TConfig>;

enum class EConfigGroup
{
	Application,
	User,
	Device
};

class Configuration
{
public:
	template<CConfigRaw TConfigType>
	struct ConfigData
	{
		using ConfigType = TConfigType;

		TConfigType										Value;
		std::optional<TConfigType>						Override;
		std::unordered_map<std::string, TConfigType>	AllowedValues;
		EConfigGroup									ConfigGroup;
	};

	using ConfigElement = std::variant<
		ConfigData<bool>,
		ConfigData<f32>,
		ConfigData<f64>,
		ConfigData<u8>,
		ConfigData<u16>,
		ConfigData<u32>,
		ConfigData<u64>,
		ConfigData<i8>,
		ConfigData<i16>,
		ConfigData<i32>,
		ConfigData<i64>,
		ConfigData<std::string>>;

	struct NoRestrictions {};

	template<CConfigType TConfigType>
	struct AllowedValues
	{
		std::unordered_map<std::string, TConfigType> Map;
	};

	template<CConfigType TConfigType, typename TId, typename TArg, typename TRestrict = NoRestrictions>
	inline void Set(TId&& aId, TArg&& aValue, EConfigGroup aGroup, TRestrict aRestrict = NoRestrictions())
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>)
			&& (std::is_constructible_v<TConfigType, TArg> || std::is_convertible_v<TArg, TConfigType>)
			&& (std::is_same_v<TRestrict, NoRestrictions> || std::is_same_v<TRestrict, AllowedValues<TConfigType>>))
	{
		std::unique_lock<std::shared_mutex> lock(myMutex);

		auto getAllowedValuesMap = [&aRestrict]()
		{
			if constexpr (std::is_same_v<TRestrict, AllowedValues<TConfigType>>)
				if constexpr (std::is_enum_v<TConfigType>)
				{
					using RawType = std::underlying_type_t<TConfigType>;

					std::unordered_map<std::string, RawType> retVal;

					retVal.reserve(aRestrict.Map.size());

					for (auto&& [name, value] : aRestrict.Map)
						retVal.emplace(std::make_pair(name, static_cast<RawType>(value)));

					return retVal;
				}
				else
					return std::move(aRestrict.Map);
			else
				if constexpr (std::is_enum_v<TConfigType>)
					return std::unordered_map<std::string, std::underlying_type_t<TConfigType>>();
				else
					return std::unordered_map<std::string, TConfigType>();
		};

		if constexpr (std::is_enum_v<TConfigType>)
		{
			ConfigDataRaw data =
			{
				ConfigData<std::underlying_type_t<TConfigType>>
				{
					static_cast<std::underlying_type_t<TConfigType>>(aValue),
					std::nullopt,
					getAllowedValuesMap(),
					aGroup
				},
				typeid(TConfigType)
			};

			myConfigs.insert(std::make_pair(static_cast<std::string>(std::forward<TId>(aId)), std::move(data)));
		}
		else if constexpr (std::is_arithmetic_v<TConfigType>)
		{
			ConfigDataRaw data =
			{
				ConfigData<TConfigType>
				{
					static_cast<TConfigType>(std::move(aValue)),
					std::nullopt,
					getAllowedValuesMap(),
					aGroup
				},
				typeid(TConfigType)
			};

			myConfigs.insert(std::make_pair(static_cast<std::string>(std::forward<TId>(aId)), std::move(data)));
		}
		else if constexpr (std::is_constructible_v<std::string, TConfigType>)
		{
			ConfigDataRaw data =
			{
				ConfigData<std::string>
				{
					std::string(std::forward<TArg>(aValue)),
					std::nullopt,
					getAllowedValuesMap(),
					aGroup
				},
				typeid(std::string)
			};

			myConfigs.insert(std::make_pair(static_cast<std::string>(std::forward<TId>(aId)), std::move(data)));
		}
		else if constexpr (std::is_convertible_v<TConfigType, std::string>)
		{
			ConfigDataRaw data =
			{
				ConfigData<std::string>
				{
					static_cast<std::string>(std::forward<TArg>(aValue)),
					std::nullopt,
					getAllowedValuesMap(),
					aGroup
				},
				typeid(std::string)
			};

			myConfigs.insert(std::make_pair(static_cast<std::string>(std::forward<TId>(aId)), std::move(data)));
		}
	}
	template<CConfigType TConfigType, bool TRaw = false, typename TArg>
	void SetOverride(const std::string& aId, TArg&& aValue)
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& std::is_constructible_v<TConfigType, TArg&&>)
	{
		std::unique_lock<std::shared_mutex> lock(myMutex);

		auto it = myConfigs.find(aId);
		auto&& overrideType = typeid(TConfigType);

		if (it == myConfigs.end())
		{
			WM_LOG_ERROR("Config missing, attempted to set override type ", overrideType.name(), " for id ", aId, ".");

			return;
		}

		auto&& configRaw = it->second;

		if (!TRaw && configRaw.Type != overrideType)
		{
			WM_LOG_ERROR("Config type mismatch, attempted to set override type ", overrideType.name(), " for id ", aId, ". Set type: ", configRaw.Type.name(), ".");

			return;
		}

		if constexpr (std::is_enum_v<TConfigType>)
		{
			using RawType = std::underlying_type_t<TConfigType>;

			if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
			{
				WM_LOG_ERROR("Config element type mismatch, attempted to set override type ", overrideType.name(), " for id ", aId, ". Set type: ", configRaw.Type.name(), ".");

				return;
			}

			auto&& config = std::get<ConfigData<RawType>>(configRaw.Config);
			
			config.Override = static_cast<std::underlying_type_t<TConfigType>>(aValue);
		}
		else
		{
			if (!std::holds_alternative<ConfigData<TConfigType>>(configRaw.Config))
			{
				WM_LOG_ERROR("Config element type mismatch, attempted to set override type ", overrideType.name(), " for id ", aId, ". Set type: ", configRaw.Type.name(), ".");

				return;
			}

			auto&& config = std::get<ConfigData<TConfigType>>(configRaw.Config);

			if constexpr (std::is_arithmetic_v<TConfigType>)
				config.Override = static_cast<TConfigType>(std::forward<TArg>(aValue));
			else if constexpr (std::is_constructible_v<std::string, TConfigType>)
				config.Override = std::string(std::forward<TArg>(aValue));
			else if constexpr (std::is_convertible_v<TConfigType, std::string>)
				config.Override = static_cast<std::string>(std::forward<TArg>(aValue));
		}
	}

	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const char* aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return Get<TConfigType>(std::string_view(aId), std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(std::string_view aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		thread_local std::string buffer;

		buffer.reserve(aId.size() + 1);

		buffer.assign(aId.data(), aId.size());

		return Get<TConfigType>(buffer, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const std::string& aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		auto it = myConfigs.find(aId);
		auto&& configFetchType = typeid(TConfigType);

		if (it == myConfigs.end())
		{
			WM_LOG_WARNING("Config missing, attempted to get type ", configFetchType.name(), " from id ", aId, ".");

			return std::forward<TDefaultArg>(aDefaultValue);
		}

		auto&& configRaw = it->second;

		if (configRaw.Type != typeid(TConfigType))
		{
			WM_LOG_ERROR("Config type mismatch, attempted to get type ", configFetchType.name(), " from id ", aId, ". Set type: ", configRaw.Type.name(), ".");

			return std::forward<TDefaultArg>(aDefaultValue);
		}

		if constexpr (std::is_enum_v<TConfigType>)
		{
			using RawType = std::underlying_type_t<TConfigType>;

			if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
			{
				WM_LOG_ERROR("Config element type mismatch, attempted to get type ", configFetchType.name(), " from id ", aId, ". Set type: ", configRaw.Type.name(), ".");

				return std::move(aDefaultValue);
			}

			auto&& config = std::get<ConfigData<RawType>>(configRaw.Config);

			if (config.Override)
				return static_cast<TConfigType>(*config.Override);

			return static_cast<TConfigType>(config.Value);
		}
		else
		{
			if (!std::holds_alternative<ConfigData<TConfigType>>(configRaw.Config))
			{
				WM_LOG_ERROR("Config element type mismatch, attempted to get type ", configFetchType.name(), " from id ", aId, ". Set type: ", configRaw.Type.name(), ".");

				return std::move(aDefaultValue);
			}

			auto&& config = std::get<ConfigData<TConfigType>>(configRaw.Config);

			if (config.Override)
				return *config.Override;

			return config.Value;
		}
	}
	
	void ResetOverride(const std::string& aId);

	inline [[nodiscard]] std::unordered_map<std::string, ConfigElement> GetConfigs() const
	{
		std::unordered_map<std::string, ConfigElement> retVal;

		retVal.reserve(myConfigs.size());

		for (const auto&[id, config] : myConfigs)
			retVal.emplace(std::make_pair(id, config.Config));

		return retVal;
	}

private:
	struct ConfigDataRaw
	{
		ConfigElement	Config;
		std::type_index	Type;
	};

	mutable std::shared_mutex myMutex;

	std::unordered_map<std::string, ConfigDataRaw> myConfigs;

};

template<typename TConfigType, typename... TPairs>
Configuration::AllowedValues<TConfigType> MakeAllowedValues(TPairs... aAllowedPairs)
{
	return Configuration::AllowedValues<TConfigType>{ { std::move(aAllowedPairs)... } };
}
