#pragma once

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/MemoryUnit.h"
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
		CMemoryUnit<TConfig> ||
		std::is_same_v<TConfig, FilePath> ||
		std::is_same_v<std::string, TConfig>);

template<typename TConfig>
concept CConfigRaw =
	std::is_arithmetic_v<std::remove_const_t<TConfig>> ||
	std::is_same_v<std::decay_t<TConfig>, std::string> ||
	std::is_same_v<std::decay_t<TConfig>, FilePath>;

template<typename TConfig>
concept CConfig =
	std::is_arithmetic_v<std::remove_const_t<TConfig>> ||
	std::is_enum_v<std::remove_const_t<TConfig>> ||
	CMemoryUnit<TConfig> ||
	std::is_same_v<TConfig, FilePath> ||
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
	static constexpr bool ConfigCanBeMemoryUnit = std::is_arithmetic_v<TConfigType> && !std::is_same_v<TConfigType, bool>;
	
	template<CConfigRaw TConfigType>
	struct ConfigDataMemoryUnit
	{
		bool operator==(const ConfigDataMemoryUnit&) const noexcept = default;
	};

	template<CConfigRaw TConfigType> requires(ConfigCanBeMemoryUnit<TConfigType>)
		struct ConfigDataMemoryUnit<TConfigType>
	{
		std::optional<EMemoryRatio>						MemoryRatio;

		bool operator==(const ConfigDataMemoryUnit&) const noexcept = default;
	};

	template<CConfigRaw TConfigType>
	struct ConfigData
		: public ConfigDataMemoryUnit<TConfigType>
	{
		using ConfigType = TConfigType;

		TConfigType										Value;
		std::optional<TConfigType>						Override;
		std::unordered_map<std::string, TConfigType>	AllowedValues;
		EConfigGroup									ConfigGroup;

		bool operator==(const ConfigData&) const noexcept = default;
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
		ConfigData<std::string>,
		ConfigData<FilePath>>;

	struct NoRestrictions {};

	template<CConfigType TConfigType>
	struct AllowedValues
	{
		std::unordered_map<std::string, TConfigType> Map;
	};

	template<CConfigType TConfigType, typename TId, typename TArg, typename TRestrict = NoRestrictions>
	inline void Set(TId&& aId, TArg&& aValue, EConfigGroup aGroup, TRestrict aRestrict = NoRestrictions())
		requires(
			CConfig<std::decay_t<TArg>>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>)
			&& (std::is_constructible_v<TConfigType, TArg> || std::is_convertible_v<TArg, TConfigType>)
			&& (std::is_same_v<TRestrict, NoRestrictions> || std::is_same_v<TRestrict, AllowedValues<TConfigType>>))
	{
		std::unique_lock<std::shared_mutex> lock(myMutex);

		auto getAllowedValuesMap = [&aRestrict]()
		{
			if constexpr (std::is_same_v<TRestrict, AllowedValues<TConfigType>>)
			{
				if constexpr (std::is_enum_v<TConfigType>)
				{
					using RawType = std::underlying_type_t<TConfigType>;

					std::unordered_map<std::string, RawType> retVal;

					retVal.reserve(aRestrict.Map.size());

					for (auto&& [name, value] : aRestrict.Map)
						retVal.emplace(std::make_pair(name, static_cast<RawType>(value)));

					return retVal;
				}
				else if constexpr (CMemoryUnit<TConfigType>)
				{
					std::unordered_map<std::string, typename TConfigType::Rep> retVal;

					retVal.reserve(aRestrict.Map.size());

					for (auto&& [name, value] : aRestrict.Map)
						retVal.emplace(std::make_pair(name, value.To<TConfigType::Ratio>()));

					return retVal;
				}
				else
					return std::move(aRestrict.Map);
			}
			else
			{
				if constexpr (std::is_enum_v<TConfigType>)
					return std::unordered_map<std::string, std::underlying_type_t<TConfigType>>();
				else if constexpr (CMemoryUnit<TConfigType>)
					return std::unordered_map<std::string, typename TConfigType::Rep>();
				else
					return std::unordered_map<std::string, TConfigType>();
			}
		};

		if constexpr (std::is_enum_v<TConfigType>)
		{
			ConfigDataRaw data =
			{
				ConfigData<std::underlying_type_t<TConfigType>>
				{
					{},
					static_cast<std::underlying_type_t<TConfigType>>(aValue),
					std::nullopt,
					getAllowedValuesMap(),
					aGroup
				},
				typeid(TConfigType)
			};

			myConfigs.insert(std::make_pair(static_cast<std::string>(std::forward<TId>(aId)), std::move(data)));
		}
		else if constexpr (CMemoryUnit<TConfigType>)
		{
			ConfigDataRaw data =
			{
				ConfigData<typename TConfigType::Rep>
				{
					{
						TConfigType::Ratio
					},
					TConfigType(aValue).To<TConfigType::Ratio>(),
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
					{},
					static_cast<TConfigType>(std::move(aValue)),
					std::nullopt,
					getAllowedValuesMap(),
					aGroup
				},
				typeid(TConfigType)
			};

			myConfigs.insert(std::make_pair(static_cast<std::string>(std::forward<TId>(aId)), std::move(data)));
		}
		else if constexpr (std::is_same_v<TConfigType, FilePath>)
		{
			ConfigDataRaw data =
			{
				ConfigData<FilePath>
				{
					{},
					std::forward<TArg>(aValue),
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
					{},
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
					{},
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
			CConfig<std::decay_t<TArg>>
			&& std::is_constructible_v<TConfigType, TArg&&>)
	{
		std::unique_lock<std::shared_mutex> lock(myMutex);

		auto it = myConfigs.find(aId);
		auto&& overrideType = typeid(TConfigType);

		if (it == myConfigs.end())
		{
			WmLogError(TagWonderMake << TagWmConfiguration << "Config missing, attempted to set override type " << overrideType.name() << " for id " << aId << '.');

			return;
		}

		auto&& configRaw = it->second;

		if constexpr (!TRaw)
			if (configRaw.Type != overrideType)
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config type mismatch, attempted to set override type " << overrideType.name() << " for id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return;
			}

		if constexpr (std::is_enum_v<TConfigType>)
		{
			using RawType = std::underlying_type_t<TConfigType>;

			if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config type mismatch, attempted to set override type " << overrideType.name() << " for id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return;
			}

			auto&& config = std::get<ConfigData<RawType>>(configRaw.Config);
			
			config.Override = static_cast<RawType>(aValue);
		}
		else if constexpr (CMemoryUnit<TConfigType>)
		{
			using RawType = typename TConfigType::Rep;

			if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config type mismatch, attempted to set override type " << overrideType.name() << " for id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return;
			}

			auto&& config = std::get<ConfigData<RawType>>(configRaw.Config);

			config.Override = TConfigType(aValue).To<TConfigType::Ratio>();
		}
		else
		{
			if (!std::holds_alternative<ConfigData<TConfigType>>(configRaw.Config))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config type mismatch, attempted to set override type " << overrideType.name() << " for id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return;
			}

			auto&& config = std::get<ConfigData<TConfigType>>(configRaw.Config);

			if constexpr (std::is_arithmetic_v<TConfigType>)
				config.Override = static_cast<TConfigType>(std::forward<TArg>(aValue));
			else if constexpr (std::is_same_v<TConfigType, FilePath>)
				config.Override = std::forward<TArg>(aValue);
			else if constexpr (std::is_constructible_v<std::string, TConfigType>)
				config.Override = std::string(std::forward<TArg>(aValue));
			else if constexpr (std::is_convertible_v<TConfigType, std::string>)
				config.Override = static_cast<std::string>(std::forward<TArg>(aValue));
		}
	}

	inline void SetMemoryRatio(const char* aId, EMemoryRatio aRatio)
	{
		SetMemoryRatio(std::string_view(aId), aRatio);
	}
	inline void SetMemoryRatio(std::string_view aId, EMemoryRatio aRatio)
	{
		thread_local std::string buffer;

		buffer.reserve(aId.size() + 1);

		buffer.assign(aId.data(), aId.size());

		SetMemoryRatio(buffer, aRatio);
	}
	inline void SetMemoryRatio(const std::string& aId, EMemoryRatio aRatio)
	{
		std::unique_lock<std::shared_mutex> lock(myMutex);

		auto it = myConfigs.find(aId);

		if (it == myConfigs.end())
		{
			WmLogError(TagWonderMake << TagWmConfiguration << "Config missing, attempted to set memory ratio for id " << aId << '.');

			return;
		}

		std::visit([&aId, aRatio, &type = it->second.Type](auto& aConfig)
			{
				using ConfigType = std::decay_t<decltype(aConfig)>::ConfigType;

				if constexpr (ConfigCanBeMemoryUnit<ConfigType>)
					aConfig.MemoryRatio = aRatio;
				else
					WmLogError(TagWonderMake << TagWmConfiguration << "Type mismatch, attempted to set memory ratio for id " << aId << " which has type: " << type.name() << '.');
			},
			it->second.Config);
	}
	
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] bool Has(const char* aId) const
	{
		return Has<TConfigType, TRaw>(std::string_view(aId));
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] bool Has(std::string_view aId) const
	{
		thread_local std::string buffer;

		buffer.reserve(aId.size() + 1);

		buffer.assign(aId.data(), aId.size());

		return Has<TConfigType, TRaw>(buffer);
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] bool Has(const std::string& aId) const
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		const auto it = myConfigs.find(aId);

		if (it == myConfigs.end())
			return false;

		const auto& configRaw = it->second;

		if constexpr (TRaw)
		{
			static constexpr auto GetDataType = []()
			{
				if constexpr (TRaw)
					return TConfigType();

				if constexpr (std::is_enum_v<TConfigType>)
					return std::underlying_type_t<TConfigType>();
				else if constexpr (CMemoryUnit<TConfigType>)
					return typename TConfigType::Rep();
				else
					return TConfigType();
			};

			using RawType = decltype(GetDataType());
			
			return std::holds_alternative<ConfigData<RawType>>(configRaw.Config);
		}
		else
			return configRaw.Type == typeid(TConfigType);
	}
	
	inline [[nodiscard]] bool Has(const char* aId) const
	{
		return Has(std::string_view(aId));
	}
	inline [[nodiscard]] bool Has(std::string_view aId) const
	{
		thread_local std::string buffer;

		buffer.reserve(aId.size() + 1);

		buffer.assign(aId.data(), aId.size());

		return Has(buffer);
	}
	inline [[nodiscard]] bool Has(const std::string& aId) const
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		return myConfigs.find(aId) != myConfigs.end();
	}
	
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const char* aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::decay_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return Get<TConfigType>(std::string_view(aId), std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(std::string_view aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::decay_t<TDefaultArg>>
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
			CConfig<std::decay_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		auto it = myConfigs.find(aId);
		auto&& configFetchType = typeid(TConfigType);

		if (it == myConfigs.end())
		{
			WmLogWarning(TagWonderMake << TagWmConfiguration << "Config missing, attempted to get type " << configFetchType.name() << " from id " << aId << '.');

			return std::forward<TDefaultArg>(aDefaultValue);
		}

		auto&& configRaw = it->second;

		if (configRaw.Type != typeid(TConfigType))
		{
			WmLogError(TagWonderMake << TagWmConfiguration << "Config type mismatch, attempted to get type " << configFetchType.name() << " from id " << aId << ". Set type: " << configRaw.Type.name() << '.');

			return std::forward<TDefaultArg>(aDefaultValue);
		}

		if constexpr (std::is_enum_v<TConfigType>)
		{
			using RawType = std::underlying_type_t<TConfigType>;

			if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config element type mismatch, attempted to get type " << configFetchType.name() << " from id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return std::move(aDefaultValue);
			}

			auto&& config = std::get<ConfigData<RawType>>(configRaw.Config);

			if (config.Override)
				return static_cast<TConfigType>(*config.Override);

			return static_cast<TConfigType>(config.Value);
		}
		else if constexpr (CMemoryUnit<TConfigType>)
		{
			using RawType = typename TConfigType::Rep;

			if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config element type mismatch, attempted to get type " << configFetchType.name() << " from id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return std::move(aDefaultValue);
			}

			auto&& config = std::get<ConfigData<RawType>>(configRaw.Config);

			if (config.Override)
				return TConfigType(*config.Override);

			return TConfigType(config.Value);
		}
		else
		{
			if (!std::holds_alternative<ConfigData<TConfigType>>(configRaw.Config))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config element type mismatch, attempted to get type " << configFetchType.name() << " from id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return std::move(aDefaultValue);
			}

			auto&& config = std::get<ConfigData<TConfigType>>(configRaw.Config);

			if (config.Override)
				return *config.Override;

			return config.Value;
		}
	}
	
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] std::optional<TConfigType> GetOverride(const char* aId) const
	{
		return GetOverride<TConfigType, TRaw>(std::string_view(aId));
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] std::optional<TConfigType> GetOverride(std::string_view aId) const
	{
		thread_local std::string buffer;

		buffer.reserve(aId.size() + 1);

		buffer.assign(aId.data(), aId.size());

		return GetOverride<TConfigType, TRaw>(buffer);
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] std::optional<TConfigType> GetOverride(const std::string& aId) const
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		const auto it = myConfigs.find(aId);
		const auto& configFetchType = typeid(TConfigType);

		if (it == myConfigs.end())
		{
			WmLogWarning(TagWonderMake << TagWmConfiguration << "Config missing, attempted to get type " << configFetchType.name() << " from id " << aId << '.');

			return std::nullopt;
		}

		const auto& configRaw = it->second;

		if constexpr (!TRaw)
			if (configRaw.Type != typeid(TConfigType))
			{
				WmLogError(TagWonderMake << TagWmConfiguration << "Config type mismatch, attempted to get type " << configFetchType.name() << " from id " << aId << ". Set type: " << configRaw.Type.name() << '.');

				return std::nullopt;
			}

		using RawType = decltype([]()
			{
				if constexpr (std::is_enum_v<TConfigType>)
					return std::underlying_type_t<TConfigType>();
				else if constexpr (CMemoryUnit<TConfigType>)
					return typename TConfigType::Rep();
				else
					return TConfigType();
			}());

		if (!std::holds_alternative<ConfigData<RawType>>(configRaw.Config))
		{
			WmLogError(TagWonderMake << TagWmConfiguration << "Config element type mismatch, attempted to get type " << configFetchType.name() << " from id " << aId << ". Set type: " << configRaw.Type.name() << '.');

			return std::nullopt;
		}

		const auto& config = std::get<ConfigData<RawType>>(configRaw.Config);

		if (!config.Override)
			return std::nullopt;

		return TConfigType(*config.Override);
	}

	void ResetOverride(const std::string& aId);

	inline [[nodiscard]] std::optional<EConfigGroup> GetGroup(const char* aId) const
	{
		return GetGroup(std::string_view(aId));
	}
	inline [[nodiscard]] std::optional<EConfigGroup> GetGroup(std::string_view aId) const
	{
		thread_local std::string buffer;

		buffer.reserve(aId.size() + 1);

		buffer.assign(aId.data(), aId.size());

		return GetGroup(buffer);
	}
	inline [[nodiscard]] std::optional<EConfigGroup> GetGroup(const std::string& aId) const
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		const auto it = myConfigs.find(aId);

		if (it == myConfigs.end())
			return std::nullopt;

		return std::visit([](const auto& aConfigData)
			{
				return aConfigData.ConfigGroup;
			},
			it->second.Config);
	}

	inline [[nodiscard]] std::unordered_map<std::string, ConfigElement> GetConfigs() const
	{
		std::shared_lock<std::shared_mutex> lock(myMutex);

		std::unordered_map<std::string, ConfigElement> retVal;

		retVal.reserve(myConfigs.size());

		for (const auto&[id, config] : myConfigs)
			retVal.emplace(std::make_pair(id, config.Config));

		return retVal;
	}

	inline [[nodiscard]] bool operator==(const Configuration& aRhs) const noexcept
	{
		std::shared_lock<std::shared_mutex> lockLhs(myMutex);
		std::shared_lock<std::shared_mutex> lockRhs(aRhs.myMutex);

		if (myConfigs.size() != aRhs.myConfigs.size())
			return false;

		for (const auto& [lhsId, lhsConfigElement] : myConfigs)
		{
			const auto it = aRhs.myConfigs.find(lhsId);

			if (it == aRhs.myConfigs.end())
				return false;

			const auto& [rhsId, rhsConfigElement] = *it;

			const bool sameValue = std::visit([&rhsConfigElement](const auto& aLhsConfigData)
				{
					if (!std::holds_alternative<std::decay_t<decltype(aLhsConfigData)>>(rhsConfigElement.Config))
						return false;

					const auto& rhsConfigData = std::get<std::decay_t<decltype(aLhsConfigData)>>(rhsConfigElement.Config);

					if (aLhsConfigData != rhsConfigData)
						return false;
					
					const auto& lhsAllowedValues = aLhsConfigData.AllowedValues;
					const auto& rhsAllowedValues = rhsConfigData.AllowedValues;

					if (lhsAllowedValues.size() != rhsAllowedValues.size())
						return false;

					const auto pred = [&rhsAllowedValues](const auto& aItem)
					{
						const auto it = rhsAllowedValues.find(aItem.first);

						return it != rhsAllowedValues.end() && it->second == aItem.second;
					};

					return std::all_of(lhsAllowedValues.begin(), lhsAllowedValues.end(), pred);
				}, lhsConfigElement.Config);

			if (!sameValue)
				return false;
		}

		return true;
	}
	inline [[nodiscard]] bool operator!=(const Configuration& aRhs) const noexcept
	{
		return !(*this == aRhs);
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
