#pragma once

#include "wondermake-base/Configuration.h"
#include "wondermake-base/ProcessId.h"
#include "wondermake-base/System.h"

#include <unordered_map>

class ConfigurationSystem
	: public System<
		Policy::Set<>,
		SystemTraits::Set<
			STFoundational,
			STThreadsafe,
			STSingleton>>
{
public:
	template<CConfigType TConfigType, typename TId, typename TArg, typename TRestrict = Configuration::NoRestrictions>
	inline void Set(TId&& aId, TArg&& aValue, EConfigGroup aConfigGroup, TRestrict aRestrict = Configuration::NoRestrictions())
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>)
			&& (std::is_constructible_v<TConfigType, TArg&&> || std::is_convertible_v<TArg&&, TConfigType>)
			&& (std::is_same_v<TRestrict, Configuration::NoRestrictions> || std::is_same_v<TRestrict, Configuration::AllowedValues<TConfigType>>))
	{
		myConfiguration.Set<TConfigType>(std::forward<TId>(aId), std::forward<TArg>(aValue), aConfigGroup, std::move(aRestrict));
	}
	template<CConfigType TConfigType, bool TRaw = false, typename TArg>
	void SetOverride(const std::string& aId, TArg&& aValue)
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& std::is_constructible_v<TConfigType, TArg>)
	{
		myConfiguration.SetOverride<TConfigType, TRaw>(aId, std::forward<TArg>(aValue));
	}

	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const char* aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return myConfiguration.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(std::string_view aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return myConfiguration.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const std::string& aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg&&>)
	{
		return myConfiguration.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}

	inline void ResetOverride(const std::string& aId)
	{
		myConfiguration.ResetOverride(aId);
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
	Configuration myConfiguration;

};
