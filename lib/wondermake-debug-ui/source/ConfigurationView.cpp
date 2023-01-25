#include "ConfigurationView.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/DebugSystem.h"
#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-engine/ConfigurationEngine.h"
#include "wondermake-engine/ConfigurationRemoteSystem.h"
#include "wondermake-engine/SerializeConfigurationJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/Math.h"

WM_REGISTER_SYSTEM(ConfigurationView);

inline constexpr float locFooterHeight = 50.f;
inline constexpr float locMemoryComboboxWidth = 50.f;
inline constexpr float locMemoryComboboxPadding = 8.f;
inline constexpr float locFilePathLocationComboboxWidth = 85.f;

template<CConfigType TConfigType>
int GetValueIndexInNameList(const std::unordered_map<std::string, TConfigType>& aValueMap, const std::vector<std::string>& aNameList, const TConfigType& aValue)
{
	auto it = std::find_if(aValueMap.begin(), aValueMap.end(), [&aValue](auto&& aPair)
		{
			return aPair.second == aValue;
		});

	if (it == aValueMap.end())
		return 0;

	const auto& name = it->first;

	for (size_t i = 0; i < aNameList.size(); ++i)
		if (aNameList[i] == name)
			return static_cast<int>(i);

	return 0;
}

void ConfigurationView::Initialize()
{
	auto& configSys = Get<ConfigurationSystem>();
	const auto& configRemoteSys = Get<ConfigurationRemoteSystem>();

	const Guid id = configRemoteSys.GetCurrentInstanceId();

	myInstanceData.emplace(
		std::make_pair(
			id,
			SInstanceData
			{
				static_cast<std::string>(configRemoteSys.GetCurrentInstanceName()),
				configSys.GetConfiguration()
			}));
	mySelectedInstance.emplace(id);
	myLastSelectedInstance = id;

	ReconstructSelectionConfigs();

	Get<DebugSystem>().AddDebugWindow("Configuration", GetExecutor(), Bind(&ConfigurationView::Tick, weak_from_this()))
		.Detach();
}

void ConfigurationView::Tick()
{
	UpdateInstanceList();

	ImGui::Begin("Configuration");

	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, buttons and text input

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_AlwaysVerticalScrollbar); // Leave room for 1 separator + buttons and text input

	UpdateInstanceSelection();

	UpdateConfigurationList();

	ImGui::EndChild();

	UpdateFooter();

	ImGui::End();
}

void ConfigurationView::UpdateInstanceList()
{
	const auto& configRemoteSys = Get<ConfigurationRemoteSystem>();

	const Guid currentId = configRemoteSys.GetCurrentInstanceId();

	{
		auto& name = myInstanceData.find(currentId)->second.Name;
		auto newName = configRemoteSys.GetCurrentInstanceName();

		if (name != newName)
			name = static_cast<std::string>(newName);
	}

	auto begin	= configRemoteSys.begin();
	auto end	= configRemoteSys.end();
	
	bool updatedSelection = false;

	for (auto it = myInstanceData.begin(); it != myInstanceData.end();)
	{
		if (it->first == configRemoteSys.GetCurrentInstanceId())
		{
			++it;

			continue;
		}

		const auto pred = [&id = it->first](const auto& aElement) -> bool
		{
			return aElement.first == id;
		};

		if (std::find_if(begin, end, pred) == end)
		{
			const Guid id = it->first;

			if (mySelectedInstance.contains(id))
			{
				mySelectedInstance.erase(id);
				updatedSelection = true;
			}

			it = myInstanceData.erase(it);

			if (myLastSelectedInstance == id)
				myLastSelectedInstance = it->first;
		}
		else
			++it;
	}

	if (std::distance(begin, end) == static_cast<std::iterator_traits<decltype(begin)>::difference_type>(myInstanceData.size() - 1))
	{
		if (updatedSelection)
			ReconstructSelectionConfigs();

		return;
	}

	const bool selectedAll = myInstanceData.size() == mySelectedInstance.size();

	for (auto& [id, instance] : configRemoteSys)
	{
		auto it = myInstanceData.find(id);

		if (it != myInstanceData.end())
			continue;

		myInstanceData.emplace(
			std::make_pair(id, SInstanceData
			{
				instance.InstanceName,
				instance.Configuration
			}));
		if (selectedAll)
		{
			mySelectedInstance.emplace(id);
			updatedSelection = true;
		}
	}

	if (updatedSelection)
		ReconstructSelectionConfigs();
}

void ConfigurationView::UpdateInstanceSelection()
{
	if (myInstanceData.size() == 1)
		return;

	if (!ImGui::CollapsingHeader("Instances"))
		return;

	ImGui::PushID("ConfigurationInstances");

	if (ImGui::BeginTable("ConfigurationInstancesTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
	{
		ImGui::TableSetupColumn("Name");

		ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultHide);

		ImGui::TableHeadersRow();

		for (const auto& [id, instance] : myInstanceData)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			const bool selected = mySelectedInstance.contains(id);

			if (ImGui::Selectable(instance.Name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					if (selected)
						mySelectedInstance.erase(id);
					else
					{
						mySelectedInstance.emplace(id);
						myLastSelectedInstance = id;
					}
				}
				else if (ImGui::GetIO().KeyShift)
				{
					const auto next = [increase = (myLastSelectedInstance < id)](auto& it)
					{
						if (increase)
							++it;
						else
							--it;
					};

					auto begin	= myInstanceData.find(myLastSelectedInstance);
					auto end	= myInstanceData.find(id);

					next(end);

					for (auto it = begin; it != end; next(it))
						mySelectedInstance.emplace(it->first);
				}
				else
				{
					mySelectedInstance.clear();

					mySelectedInstance.emplace(id);
					myLastSelectedInstance = id;
				}

				ReconstructSelectionConfigs();
			}

			ImGui::TableSetColumnIndex(1);

			ImGui::Text("%s ", id.ToFixedSizeString().ToCString());
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
}

void ConfigurationView::UpdateConfigurationList()
{
	if (ImGui::BeginTable("ConfigurationTable", 3, ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("ID");

		ImGui::TableSetupColumn("Value");

		ImGui::TableSetupColumn("Reset");

		ImGui::TableHeadersRow();

		for (auto& [id, config] : mySelectedConfigData)
		{
			if (!myFilterText.empty())
			{
				auto searchResult = std::search(
					id.begin(), id.end(),
					myFilterText.begin(), myFilterText.end(),
					[](char a, char b) {return std::tolower(a) == std::tolower(b); }
				);

				if (searchResult == id.end())
					continue;
			}

			ImGui::TableNextRow();

			ImGui::PushID(id.c_str());

			const bool hasChanges = myActions.find(id) != myActions.end();

			if (hasChanges)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));

			ImGui::TableSetColumnIndex(0);

			ImGui::Text(id.c_str());

			std::visit([this, &baseConfig = config, &id](auto& aConfig)
				{
					using ConfigDataType = std::decay_t<decltype(aConfig)>;

					ImGui::TableSetColumnIndex(1);

					if constexpr (std::is_same_v<ConfigDataType, DifferentData>)
						ImGui::Text("<Different Values>");
					else
					{
						using Type = ConfigDataType::ConfigType;

						if (!aConfig.AllowedValues.empty())
						{
							std::vector<const char*> nameList;

							nameList.reserve(aConfig.AllowedValues.size());

							for (auto&& name : aConfig.AllowedValueNames)
								nameList.emplace_back(name.c_str());

							ImGui::PushItemWidth(ImGui::GetColumnWidth());

							if (ImGui::Combo("##", &aConfig.CurrentAllowedValueIndex, nameList.data(), static_cast<int>(nameList.size())))
							{
								myActions[id] = aConfig.AllowedValues[nameList[aConfig.CurrentAllowedValueIndex]];
								baseConfig.IsOverridden = true;
							}

							ImGui::PopItemWidth();
						}
						else if constexpr (std::is_same_v<Type, bool>)
						{
							if (ImGui::Checkbox("##", &aConfig.ValueCurrent))
							{
								myActions[id] = aConfig.ValueCurrent;
								baseConfig.IsOverridden = true;
							}
						}
						else if constexpr (std::is_same_v<Type, std::string>)
						{
							ImGui::PushItemWidth(ImGui::GetColumnWidth());

							if (ImGui::InputText("##", &aConfig.ValueCurrent, ImGuiInputTextFlags_None))
							{
								myActions[id] = aConfig.ValueCurrent;
								baseConfig.IsOverridden = true;
							}

							ImGui::PopItemWidth();
						}
						else if constexpr (std::is_same_v<Type, FilePath>)
						{
							std::string filePathId = id + "_FilePath";
							
							ImGui::PushID(filePathId.c_str());

							ImGui::PushItemWidth(locFilePathLocationComboboxWidth);

							static int locationIndex;
							static std::string input;
							static std::vector<const char*> locationList =
							{
								"Unset",
								"Bin",
								"Data",
								"User",
								"UserData",
							};

							locationIndex = static_cast<int>(aConfig.ValueCurrent.Location);

							if (ImGui::Combo("##", &locationIndex, locationList.data(), static_cast<int>(locationList.size())))
							{
								aConfig.ValueCurrent.Location = static_cast<FilePath::EFolder>(locationIndex);

								myActions[id] = aConfig.ValueCurrent;
								baseConfig.IsOverridden = true;
							}

							ImGui::PopItemWidth();

							ImGui::PopID();

							ImGui::SameLine();

							ImGui::PushItemWidth(ImGui::GetColumnWidth());

							input = aConfig.ValueCurrent.Path.string();

							if (ImGui::InputText("", &input, ImGuiInputTextFlags_None))
							{
								aConfig.ValueCurrent.Path = input;

								myActions[id] = aConfig.ValueCurrent;
								baseConfig.IsOverridden = true;
							}

							ImGui::PopItemWidth();
						}
						else
						{
							ImGuiDataType dataType = ImGuiDataType_S8;

							if constexpr (std::is_same_v<Type, u8>)
								dataType = ImGuiDataType_U8;
							else if constexpr (std::is_same_v<Type, u16>)
								dataType = ImGuiDataType_U16;
							else if constexpr (std::is_same_v<Type, u32>)
								dataType = ImGuiDataType_U32;
							else if constexpr (std::is_same_v<Type, u64>)
								dataType = ImGuiDataType_U64;

							else if constexpr (std::is_same_v<Type, i8>)
								dataType = ImGuiDataType_S8;
							else if constexpr (std::is_same_v<Type, i16>)
								dataType = ImGuiDataType_S16;
							else if constexpr (std::is_same_v<Type, i32>)
								dataType = ImGuiDataType_S32;
							else if constexpr (std::is_same_v<Type, i64>)
								dataType = ImGuiDataType_S64;

							else if constexpr (std::is_same_v<Type, f32>)
								dataType = ImGuiDataType_Float;
							else if constexpr (std::is_same_v<Type, f64>)
								dataType = ImGuiDataType_Double;

							float width = ImGui::GetColumnWidth();

							if (baseConfig.HasMemoryRatio)
								width -= locMemoryComboboxWidth + locMemoryComboboxPadding;

							ImGui::PushItemWidth(width);

							if (ImGui::InputScalar("##", dataType, &aConfig.ValueCurrent, NULL))
							{
								if (baseConfig.HasMemoryRatio)
									myActions[id] = aConfig.ValueCurrent * static_cast<Type>(baseConfig.AllowedMemoryRatio[baseConfig.CurrentMemoryRatioIndex]);
								else
									myActions[id] = aConfig.ValueCurrent;
								baseConfig.IsOverridden = true;
							}

							ImGui::PopItemWidth();

							if (baseConfig.HasMemoryRatio)
							{
								std::vector<const char*> nameList;

								nameList.reserve(baseConfig.AllowedMemoryRatioNames.size());

								for (auto&& name : baseConfig.AllowedMemoryRatioNames)
									nameList.emplace_back(name.c_str());

								std::string comboId = id + "_MemoryRatio";
								ImGui::SameLine();

								ImGui::PushID(comboId.c_str());

								ImGui::PushItemWidth(locMemoryComboboxWidth);
								const int previousIndex = baseConfig.CurrentMemoryRatioIndex;

								if (ImGui::Combo("##", &baseConfig.CurrentMemoryRatioIndex, nameList.data(), static_cast<int>(nameList.size())))
								{
									Type newValue = aConfig.ValueCurrent * static_cast<Type>(baseConfig.AllowedMemoryRatio[previousIndex]);

									aConfig.ValueCurrent = newValue / static_cast<Type>(baseConfig.AllowedMemoryRatio[baseConfig.CurrentMemoryRatioIndex]);
									
									myActions[id] = aConfig.ValueCurrent * static_cast<Type>(baseConfig.AllowedMemoryRatio[baseConfig.CurrentMemoryRatioIndex]);
									baseConfig.IsOverridden = true;
								}

								ImGui::PopItemWidth();

								ImGui::PopID();
							}
						}
					}

				},
				config.Data);

			ImGui::TableSetColumnIndex(2);
			const bool isOverridden = config.IsOverridden;

			ImGui::PushItemWidth(ImGui::GetColumnWidth());
			if (!isOverridden)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button("Reset"))
			{
				myActions[id] = ActionReset();
				config.IsOverridden = false;
				std::visit([&baseConfig = config](auto& aConfig)
					{
						using ConfigDataType = std::decay_t<decltype(aConfig)>;

						if constexpr (!std::is_same_v<ConfigDataType, DifferentData>)
						{
							using Type = ConfigDataType::ConfigType;

							aConfig.ValueCurrent = aConfig.ValueOriginal;
							aConfig.CurrentAllowedValueIndex = GetValueIndexInNameList(aConfig.AllowedValues, aConfig.AllowedValueNames, aConfig.ValueOriginal);

							if constexpr (std::is_arithmetic_v<Type> && !std::is_same_v<Type, bool>)
							{
								if (!baseConfig.HasMemoryRatio)
									return;

								for (size_t i = 0; i < baseConfig.AllowedMemoryRatio.size(); ++i)
								{
									const auto ratioInt = static_cast<std::underlying_type_t<EMemoryRatio>>(baseConfig.AllowedMemoryRatio[i]);

									if (WmMath::Mod(aConfig.ValueOriginal, static_cast<Type>(ratioInt)) != 0)
										continue;

									baseConfig.CurrentMemoryRatioIndex = static_cast<int>(i);
									aConfig.ValueCurrent = aConfig.ValueOriginal / static_cast<Type>(ratioInt);
								}
							}
						}
					},
					config.Data);
			}

			ImGui::PopItemWidth();
			if (!isOverridden)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			if (hasChanges)
				ImGui::PopStyleColor();

			ImGui::PopID();
		}

		ImGui::EndTable();
	}
}

void ConfigurationView::UpdateFooter()
{
	ImGui::Separator();

	ImGui::Text("Filter");
	
	ImGui::SameLine();

	ImGui::InputText("##", &myFilterText, ImGuiInputTextFlags_AutoSelectAll);

	ImGui::SameLine();

	const bool hasChanges = !myActions.empty();

	if (!hasChanges)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Apply Changes"))
		ApplyChanges();

	ImGui::SameLine();

	if (ImGui::Button("Reset Changes"))
		ResetChanges();

	if (!hasChanges)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

void ConfigurationView::ApplyChanges()
{
	for (auto& [id, action] : myActions)
	{
		std::visit([this, &id](auto& aAction)
			{
				using Type = std::decay_t<decltype(aAction)>;

				if constexpr (std::is_same_v<Type, ActionReset>)
					ResetOverride(id);
				else
					SetOverride<Type>(id, std::move(aAction));
			},
			action);
	}

	myActions.clear();
}

void ConfigurationView::ResetChanges()
{
	ReconstructSelectionConfigs();
}

void ConfigurationView::ReconstructSelectionConfigs()
{
	mySelectedConfigData.clear();
	myActions.clear();

	for (const auto& instanceId : mySelectedInstance)
	{
		auto it = myInstanceData.find(instanceId);

		if (it == myInstanceData.end())
			continue;

		const Configuration& configuration = it->second.Configuration;

		for (auto&& [id, configData] : configuration.GetConfigs())
		{
			std::visit([&outConfig = mySelectedConfigData, &id](auto&& aConfigData)
				{
					using Type = std::decay_t<decltype(aConfigData)>::ConfigType;

					const bool isOverridden = static_cast<bool>(aConfigData.Override);

					auto valueCurrent = isOverridden ? std::move(*aConfigData.Override) : aConfigData.Value;

					std::vector<std::string> allowedValueNames;

					for (auto&& [name, _] : aConfigData.AllowedValues)
						allowedValueNames.emplace_back(name);

					const int allowedValueIndex = GetValueIndexInNameList(aConfigData.AllowedValues, allowedValueNames, valueCurrent);

					bool						hasMemoryRatio = false;
					std::vector<EMemoryRatio>	allowedMemoryRatio;
					std::vector<std::string>	allowedMemoryRatioNames;
					int							currentMemoryRatioIndex = 0;

					if constexpr (Configuration::ConfigCanBeMemoryUnit<Type>)
						if (aConfigData.MemoryRatio)
						{
							hasMemoryRatio = true;
							const auto ratio = static_cast<std::underlying_type_t<EMemoryRatio>>(*aConfigData.MemoryRatio);

							const auto addRatioIfAllowed = [valueCurrent, &allowedMemoryRatio, &allowedMemoryRatioNames, &currentMemoryRatioIndex, ratio](EMemoryRatio aRatio, auto aName)
							{
								const auto selectedRatio = static_cast<std::underlying_type_t<EMemoryRatio>>(aRatio);

								if (selectedRatio > static_cast<std::underlying_type_t<EMemoryRatio>>(std::numeric_limits<Type>::max())
									|| selectedRatio % ratio != 0)
									return;

								if (WmMath::Mod(static_cast<Type>(valueCurrent * ratio), static_cast<Type>(selectedRatio)) == 0)
									currentMemoryRatioIndex = static_cast<int>(allowedMemoryRatio.size());

								allowedMemoryRatio.emplace_back(aRatio);
								allowedMemoryRatioNames.emplace_back(std::move(aName));
							};

							addRatioIfAllowed(EMemoryRatio::B,		"B");
							addRatioIfAllowed(EMemoryRatio::KB,		"KB");
							addRatioIfAllowed(EMemoryRatio::MB,		"MB");
							addRatioIfAllowed(EMemoryRatio::GB,		"GB");
							addRatioIfAllowed(EMemoryRatio::TB,		"TB");
							addRatioIfAllowed(EMemoryRatio::KiB,	"KiB");
							addRatioIfAllowed(EMemoryRatio::MiB,	"MiB");
							addRatioIfAllowed(EMemoryRatio::GiB,	"GiB");
							addRatioIfAllowed(EMemoryRatio::TiB,	"TiB");

							valueCurrent /= static_cast<Type>(allowedMemoryRatio[currentMemoryRatioIndex]) / static_cast<Type>(ratio);
						}

					const auto it = outConfig.find(id);

					if (it == outConfig.end())
					{
						outConfig.emplace(
							std::make_pair(
								id,
								SConfig
								{
									SConfigData<Type>(
										std::move(aConfigData.Value),
										std::move(valueCurrent),
										std::move(allowedValueNames),
										std::move(aConfigData.AllowedValues),
										allowedValueIndex),
									isOverridden,
									hasMemoryRatio,

									std::move(allowedMemoryRatio),
									std::move(allowedMemoryRatioNames),
									currentMemoryRatioIndex
								}));
						
						return;
					}

					auto& [otherId, otherConfig] = *it;

					if (isOverridden)
						otherConfig.IsOverridden = true;

					if (!std::holds_alternative<SConfigData<Type>>(otherConfig.Data)
						|| std::get<SConfigData<Type>>(otherConfig.Data).ValueCurrent != valueCurrent
						|| otherConfig.HasMemoryRatio != hasMemoryRatio
						|| otherConfig.AllowedMemoryRatio != allowedMemoryRatio)
						otherConfig.Data = DifferentData{};

				}, configData);
		}
	}
}

template<CConfig TConfig>
void ConfigurationView::SetOverride(std::string aId, TConfig aConfig)
{
	static constexpr bool raw = true;
	auto& configRemoteSys = Get<ConfigurationRemoteSystem>();

	const Guid currentId = configRemoteSys.GetCurrentInstanceId();

	for (const auto instanceId : mySelectedInstance)
	{
		const auto it = myInstanceData.find(instanceId);

		if (it == myInstanceData.end())
			continue;

		const Configuration& config = it->second.Configuration;

		if (!config.Has<TConfig, raw>(aId))
			continue;

		if (instanceId == currentId)
		{
			Get<ConfigurationSystem>().
				SetOverride<TConfig, raw>(aId, std::move(aConfig));
		}
		else
		{
			configRemoteSys.
				SetOverride<TConfig>(instanceId, aId, std::move(aConfig));
		}
	}
}

void ConfigurationView::ResetOverride(std::string aId)
{
	auto& configRemoteSys = Get<ConfigurationRemoteSystem>();

	const Guid currentId = configRemoteSys.GetCurrentInstanceId();

	for (const auto instanceId : mySelectedInstance)
	{
		{
			const auto it = myInstanceData.find(instanceId);

			if (it == myInstanceData.end())
				continue;

			const Configuration& config = it->second.Configuration;

			auto configs = config.GetConfigs();

			if (configs.find(aId) == configs.end())
				continue;
		}

		if (instanceId == currentId)
		{
			Get<ConfigurationSystem>().
				ResetOverride(aId);
		}
		else
		{
			Get<ConfigurationRemoteSystem>().
				ResetOverride(instanceId, aId);
		}
	}
}
