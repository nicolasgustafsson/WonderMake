#pragma once

#include "wondermake-base/Configuration.h"
#include "wondermake-base/ProcessId.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/BufferExecutor.h"
#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/EventSubscriber.h"

#include <unordered_map>

class ConfigurationSystem
	: public System<
		Policy::Set<>,
		SystemTraits::Set<
			STFoundational,
			STSingleton>>
{
public:
	inline ConfigurationSystem() noexcept
		: myEventList(GetExecutor())
	{}

	template<CConfigType TConfigType, typename TId, typename TArg, typename TRestrict = Configuration::NoRestrictions>
	inline void Set(TId&& aId, TArg&& aValue, EConfigGroup aConfigGroup, TRestrict aRestrict = Configuration::NoRestrictions())
		requires(
			CConfig<std::decay_t<TArg>>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>)
			&& (std::is_constructible_v<TConfigType, TArg&&> || std::is_convertible_v<TArg&&, TConfigType>)
			&& (std::is_same_v<TRestrict, Configuration::NoRestrictions> || std::is_same_v<TRestrict, Configuration::AllowedValues<TConfigType>>))
	{
		myConfiguration.Set<TConfigType>(std::forward<TId>(aId), std::forward<TArg>(aValue), aConfigGroup, std::move(aRestrict));
	}
	template<CConfigType TConfigType, bool TRaw = false, typename TArg>
	void SetOverride(const std::string& aId, TArg&& aValue)
		requires(
			CConfig<std::decay_t<TArg>>
			&& std::is_constructible_v<TConfigType, TArg>)
	{
		if constexpr (TRaw)
		{
			const auto configs = myConfiguration.GetConfigs();

			const auto it = configs.find(aId);

			if (it == configs.end())
				return;

			auto& [id, element] = *it;
			using configData = Configuration::ConfigData<TConfigType>;

			if (!std::holds_alternative<configData>(element))
				return;

			auto& data = std::get<configData>(element);

			if (data.Override)
			{
				if (*data.Override == aValue)
					return;
			}
			else if (data.Value == aValue)
				return;
		}
		else
		{
			const auto config = myConfiguration.Get<TConfigType>(aId, aValue);

			if (config == aValue)
				return;
		}
		
		myConfiguration.SetOverride<TConfigType, TRaw>(aId, aValue);

		BufferExecutor executor;
		SOverrideData overrideData = { aId };

		if constexpr (std::is_enum_v<TConfigType>)
			overrideData.Value = static_cast<std::underlying_type_t<TConfigType>>(aValue);
		else
			overrideData.Value = std::forward<TArg>(aValue);

		for (auto& subscriberData : myEventList)
			if (!subscriberData.ConfigId || *subscriberData.ConfigId == aId)
				subscriberData.Trigger.Trigger(executor, overrideData);

		executor.ExecuteAll();
	}

	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const char* aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::decay_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return myConfiguration.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(std::string_view aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::decay_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return myConfiguration.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const std::string& aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::decay_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg&&>)
	{
		return myConfiguration.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] bool Has(const char* aId) const
	{
		return myConfiguration.Has<TConfigType, TRaw>(aId);
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] bool Has(std::string_view aId) const
	{
		return myConfiguration.Has<TConfigType, TRaw>(aId);
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] bool Has(const std::string& aId) const
	{
		return myConfiguration.Has<TConfigType, TRaw>(aId);
	}

	inline [[nodiscard]] bool Has(const char* aId) const
	{
		return myConfiguration.Has(aId);
	}
	inline [[nodiscard]] bool Has(std::string_view aId) const
	{
		return myConfiguration.Has(aId);
	}
	inline [[nodiscard]] bool Has(const std::string& aId) const
	{
		return myConfiguration.Has(aId);
	}
	
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] std::optional<TConfigType> GetOverride(const char* aId) const
	{
		return myConfiguration.GetOverride<TConfigType, TRaw>(aId);
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] std::optional<TConfigType> GetOverride(std::string_view aId) const
	{
		return myConfiguration.GetOverride<TConfigType, TRaw>(aId);
	}
	template<CConfigType TConfigType, bool TRaw = false>
	inline [[nodiscard]] std::optional<TConfigType> GetOverride(const std::string& aId) const
	{
		return myConfiguration.GetOverride<TConfigType, TRaw>(aId);
	}

	inline void ResetOverride(const std::string& aId)
	{
		const auto configs = myConfiguration.GetConfigs();

		const auto it = configs.find(aId);

		if (it == configs.end())
			return;

		auto& [id, element] = *it;
		BufferExecutor executor;

		std::visit([&executor, &aId, &eventList = myEventList](const auto& aConfigData)
			{
				if (!aConfigData.Override)
					return;
				else if (*aConfigData.Override == aConfigData.Value)
					return;

				const SOverrideData overrideData = { aId, aConfigData.Value };

				for (auto& subscriberData : eventList)
					if (!subscriberData.ConfigId || *subscriberData.ConfigId == aId)
						subscriberData.Trigger.Trigger(executor, overrideData);

			}, element);
		
		myConfiguration.ResetOverride(aId);

		executor.ExecuteAll();
	}
	
	inline [[nodiscard]] std::optional<EConfigGroup> GetGroup(const char* aId) const
	{
		return myConfiguration.GetGroup(aId);
	}
	inline [[nodiscard]] std::optional<EConfigGroup> GetGroup(std::string_view aId) const
	{
		return myConfiguration.GetGroup(aId);
	}
	inline [[nodiscard]] std::optional<EConfigGroup> GetGroup(const std::string& aId) const
	{
		return myConfiguration.GetGroup(aId);
	}

	template<CExecutor TExecutor, typename TCallable>
	inline EventSubscriber OnOverrideChanged(TExecutor aExecutor, TCallable&& aCallable)
		requires(std::is_invocable_v<const TCallable, const std::string&>)
	{
		auto [trigger, subscriber] = MakeEventTrigger<SOverrideData>(std::move(aExecutor), [callable = std::forward<TCallable>(aCallable)](const auto aOverrideData)
			{
				callable(aOverrideData.Id);
			});

		myEventList.Emplace(SOverrideSubscriberData{ std::move(trigger) });

		return std::move(subscriber);
	}
	template<CConfig TConfig, typename TId, CExecutor TExecutor, typename TCallable>
	inline EventSubscriber OnOverrideChanged(TId&& aId, TExecutor aExecutor, TCallable&& aCallable)
		requires(
			std::is_invocable_v<const TCallable, const std::string&, const TConfig&>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>))
	{
		auto [trigger, subscriber] = MakeEventTrigger<SOverrideData>(std::move(aExecutor), [callable = std::forward<TCallable>(aCallable)](const auto aOverrideData)
			{
				if constexpr (std::is_enum_v<TConfig>)
					callable(aOverrideData.Id, static_cast<TConfig>(std::get<std::underlying_type_t<TConfig>>(aOverrideData.Value)));
				else if constexpr (CConfigurableValue<TConfig>)
				{
					static constexpr auto to = [](const auto& aValue) { return WmConfig<TConfig>::To(aValue); };

					using RawType = std::invoke_result_t<decltype(to), const TConfig&>;

					const auto& value = std::get<RawType>(aOverrideData.Value);

					auto result = WmConfig<TConfig>::From(value);

					if (!result)
					{
						WmLogError(TagWonderMake << TagWmConfiguration << "Failed to call override on change callback; failed to convert value " << value << " to type: " << typeid(TConfig).name() << '.');

						return;
					}

					callable(aOverrideData.Id, result.Unwrap());
				}
				else if constexpr (CMemoryUnit<TConfig>)
				{
					using RawType = typename TConfig::Rep;

					callable(aOverrideData.Id, TConfig(std::get<RawType>(aOverrideData.Value)));
				}
				else
					callable(aOverrideData.Id, std::get<TConfig>(aOverrideData.Value));
			});

		myEventList.Emplace(SOverrideSubscriberData{ std::move(trigger), static_cast<std::string>(std::forward<TId>(aId)) });

		return std::move(subscriber);
	}

	inline [[nodiscard]] std::unordered_map<std::string, Configuration::ConfigElement> GetConfigs() const
	{
		return myConfiguration.GetConfigs();
	}
	inline [[nodiscard]] const Configuration& GetConfiguration() const
	{
		return myConfiguration;
	}

private:
	struct SOverrideData
	{
		using ValueType = std::variant<bool, f32, f64, u8, u16, u32, u64, i8, i16, i32, i64, std::string, FilePath>;

		std::string	Id;
		ValueType	Value;
	};

	struct SOverrideSubscriberData
	{
		EventTrigger<SOverrideData>	Trigger;
		std::optional<std::string>	ConfigId;

		inline void OnCancel(CExecutor auto&& aExecutor, auto&& aOnCancel)
		{
			WmCallOnCancel(Trigger, std::forward<decltype(aExecutor)>(aExecutor), std::forward<decltype(aOnCancel)>(aOnCancel));
		}
	};

	Configuration myConfiguration;

	CancelableList<SOverrideSubscriberData> myEventList;

};
