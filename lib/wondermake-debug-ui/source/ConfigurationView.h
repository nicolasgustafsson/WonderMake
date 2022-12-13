#pragma once

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/System.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ConfigurationRemoteSystem;
class DebugSettingsSystem;
class DebugSystem;

class JobSystem;

class ConfigurationView
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<ConfigurationRemoteSystem, PWrite>,
			PAdd<DebugSettingsSystem, PWrite>,
			PAdd<DebugSystem, PWrite>,
			PAdd<JobSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
	, public std::enable_shared_from_this<ConfigurationView>
{
public:
	void Initialize() override;

private:
	struct SInstanceData
	{
		std::string									Name;
		std::reference_wrapper<const Configuration>	Configuration;
	};

	struct DifferentData {};
	template<CConfigType TConfigType>
	struct SConfigData
	{
		using ConfigType = TConfigType;

		TConfigType										ValueOriginal;
		TConfigType										ValueCurrent;
		std::vector<std::string>						AllowedValueNames;
		std::unordered_map<std::string, TConfigType>	AllowedValues;
		int												CurrentAllowedValueIndex = 0;
	};
	struct SConfig
	{
		using DataType = std::variant<
			DifferentData,
			SConfigData<bool>,
			SConfigData<f32>,
			SConfigData<f64>,
			SConfigData<u8>,
			SConfigData<u16>,
			SConfigData<u32>,
			SConfigData<u64>,
			SConfigData<i8>,
			SConfigData<i16>,
			SConfigData<i32>,
			SConfigData<i64>,
			SConfigData<std::string>,
			SConfigData<FilePath>>;

		DataType					Data;
		bool						IsOverridden;
		bool						HasMemoryRatio;

		std::vector<EMemoryRatio>	AllowedMemoryRatio;
		std::vector<std::string>	AllowedMemoryRatioNames;
		int							CurrentMemoryRatioIndex = 0;
	};
	
	struct ActionReset {};
	using ActionType = std::variant<
		ActionReset,
		bool,
		f32,
		f64,
		u8,
		u16,
		u32,
		u64,
		i8,
		i16,
		i32,
		i64,
		std::string,
		FilePath>;
	
	void Tick();

	void UpdateInstanceList();
	void UpdateInstanceSelection();
	void UpdateConfigurationList();
	void UpdateFooter();

	void ApplyChanges();
	void ResetChanges();
	void ReconstructSelectionConfigs();

	template<CConfig TConfig>
	void SetOverride(std::string aId, TConfig aConfig);
	void ResetOverride(std::string aId);

	std::map<Guid, SInstanceData>				myInstanceData;

	std::unordered_set<Guid>					mySelectedInstance;
	Guid										myLastSelectedInstance = Guid::Zero();

	std::map<std::string, SConfig>				mySelectedConfigData;
	std::unordered_map<std::string, ActionType>	myActions;
	std::string									myFilterText;


};
