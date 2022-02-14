#pragma once

#include "wondermake-base/Configuration.h"
#include "wondermake-base/ProcessId.h"
#include "wondermake-base/System.h"

#include <unordered_map>

class ProcessSystem;

class ConfigurationSystem
	: public System<
		Policy::Set<
			PAdd<ProcessSystem, PWrite>>,
		SystemTraits::Set<
			STFoundational,
			STThreadsafe,
			STSingleton>>
{
public:
	void Initialize() override;

	template<CConfigType TConfigType, typename TId, typename TArg, typename TRestrict = Configuration::NoRestrictions>
	inline void Set(TId&& aId, TArg&& aValue, EConfigGroup aConfigGroup, TRestrict aRestrict = Configuration::NoRestrictions())
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>)
			&& (std::is_constructible_v<TConfigType, TArg&&> || std::is_convertible_v<TArg&&, TConfigType>)
			&& (std::is_same_v<TRestrict, Configuration::NoRestrictions> || std::is_same_v<TRestrict, Configuration::AllowedValues<TConfigType>>))
	{
		Set<TConfigType>(myProcessId, std::forward<TId>(aId), std::forward<TArg>(aValue), aConfigGroup, std::move(aRestrict));
	}
	template<CConfigType TConfigType, typename TId, typename TArg, typename TRestrict = Configuration::NoRestrictions>
	inline void Set(ProcessId aProcessId, TId&& aId, TArg&& aValue, EConfigGroup aConfigGroup, TRestrict aRestrict = Configuration::NoRestrictions())
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& (std::is_constructible_v<std::string, TId> || std::is_convertible_v<TId, std::string>)
			&& (std::is_constructible_v<TConfigType, TArg&&> || std::is_convertible_v<TArg&&, TConfigType>)
			&& (std::is_same_v<TRestrict, Configuration::NoRestrictions> || std::is_same_v<TRestrict, Configuration::AllowedValues<TConfigType>>))
	{
		myConfigurationSets[aProcessId].Set<TConfigType>(std::forward<TId>(aId), std::forward<TArg>(aValue), aConfigGroup, std::move(aRestrict));
	}
	template<CConfigType TConfigType, bool TRaw = false, typename TArg>
	void SetOverride(const std::string& aId, TArg&& aValue)
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& std::is_constructible_v<TConfigType, TArg>)
	{
		SetOverride<TConfigType, TRaw>(myProcessId, aId, std::forward<TArg>(aValue));
	}
	template<CConfigType TConfigType, bool TRaw = false, typename TArg>
	void SetOverride(ProcessId aProcessId, const std::string& aId, TArg&& aValue)
		requires(
			CConfig<std::remove_reference_t<TArg>>
			&& std::is_constructible_v<TConfigType, TArg>)
	{
		myConfigurationSets[aProcessId].SetOverride<TConfigType, TRaw>(aId, std::forward<TArg>(aValue));
	}

	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const char* aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return Get<TConfigType>(myProcessId, aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(std::string_view aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		return Get<TConfigType>(myProcessId, aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(const std::string& aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg&&>)
	{
		return Get<TConfigType>(myProcessId, aId, std::forward<TDefaultArg>(aDefaultValue));
	}

	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(ProcessId aProcessId, const char* aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		auto itProcess = myConfigurationSets.find(aProcessId);

		if (itProcess == myConfigurationSets.cend())
			return std::forward<TDefaultArg>(aDefaultValue);

		return itProcess->second.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(ProcessId aProcessId, std::string_view aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg>)
	{
		auto itProcess = myConfigurationSets.find(aProcessId);

		if (itProcess == myConfigurationSets.cend())
			return std::forward<TDefaultArg>(aDefaultValue);

		return itProcess->second.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	template<CConfigType TConfigType, typename TDefaultArg>
	inline [[nodiscard]] TConfigType Get(ProcessId aProcessId, const std::string& aId, TDefaultArg&& aDefaultValue) const
		requires(
			CConfig<std::remove_reference_t<TDefaultArg>>
			&& std::is_constructible_v<TConfigType, TDefaultArg&&>)
	{
		auto itProcess = myConfigurationSets.find(aProcessId);

		if (itProcess == myConfigurationSets.cend())
			return std::forward<TDefaultArg>(aDefaultValue);

		return itProcess->second.Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}
	
	void ResetOverride(const std::string& aId);
	void ResetOverride(ProcessId aProcessId, const std::string& aId);

	[[nodiscard]] std::unordered_map<std::string, Configuration::ConfigElement> GetConfigs() const;
	[[nodiscard]] std::unordered_map<std::string, Configuration::ConfigElement> GetConfigs(ProcessId aProcessId) const;

	[[nodiscard]] std::unordered_map<ProcessId, std::unordered_map<std::string, Configuration::ConfigElement>> GetAllConfigs() const;

private:
	ProcessId myProcessId;

	std::unordered_map<ProcessId, Configuration> myConfigurationSets;

};
