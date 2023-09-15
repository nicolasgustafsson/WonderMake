#include "SystemRegistryView.h"

#include "wondermake-ui/DebugSystem.h"
#include "wondermake-ui/ImguiInclude.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(SystemRegistryView);

void SystemRegistryView::Initialize()
{
	auto debug = Bind(&SystemRegistryView::Debug, weak_from_this());

	Get<DebugSystem>()
		.AddDebugWindow("System Registry", GetExecutor(), std::move(debug))
		.Detach();
}

[[nodiscard]] bool operator==(const SystemCounter::SSystemInfo& aLhs, const SystemCounter::SSystemInfo& aRhs) noexcept
{
	return aLhs.TypeName == aRhs.TypeName
		&& aLhs.Dependencies == aRhs.Dependencies;
}

void SystemRegistryView::Debug(bool& aIsOpen)
{
	ImGui::Begin("System Registry", &aIsOpen);

	auto& sysRegistry = Global::GetSystemRegistry();

	auto& systemsInfo = sysRegistry.CurrentSystems();

	UpdateCache(systemsInfo);

	ImGui::Text("%s%u", "Current systems: ", static_cast<u32>(myTransformedCache.size()));

	ImGui::Separator();

	std::array<char, 256> typeNameBuffer = {};

	for (const auto& [generation, typeMap] : mySortedIds)
	{
		for (const auto& [typeName, id] : typeMap)
		{
			const auto& info = myTransformedCache.find(id)->second;

			auto fixedStringId = info.Id.Data().ToFixedSizeString();

			ImGui::PushID(fixedStringId.ToCString());

			static bool selected = false;
			bool hasSelection = mySelectedSystem.has_value();
			bool isSelected = (mySelectedSystem == id);

			selected = isSelected;

			std::snprintf(typeNameBuffer.data(), typeNameBuffer.size(), "%s%s%u%s%u", typeName.c_str(), ", refs: ", static_cast<u32>(info.Uses.size()), ", deps: ", static_cast<u32>(info.Dependencies.size()));

			bool isDep = mySelectedSystem && info.Uses.find(*mySelectedSystem) != info.Uses.end();
			bool isUse = mySelectedSystem && info.Dependencies.find(*mySelectedSystem) != info.Dependencies.end();

			if (isDep)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			else if (isUse)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 1, 1));
			else if (hasSelection && !isSelected)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, .5f));

			if (ImGui::Selectable(typeNameBuffer.data(), &selected))
				if (selected)
					mySelectedSystem = id;
				else
					mySelectedSystem.reset();

			if (isDep || isUse || (hasSelection && !isSelected))
				ImGui::PopStyleColor();

			ImGui::PopID();
		}

		ImGui::Separator();
	}

	ImGui::End();
}

void SystemRegistryView::UpdateCache(const std::unordered_map<SystemId, SystemCounter::SSystemInfo>& aInfo)
{
	if (myCache == aInfo)
		return;

	myCache = aInfo;
	mySortedIds.clear();
	myTransformedCache.clear();

	for (const auto& [id, info] : myCache)
	{
		auto& typeMap = mySortedIds[info.Generation];

		typeMap.emplace(std::make_pair(info.TypeName, id));
		myTransformedCache.emplace(std::make_pair(id,
			SInfo
			{
				.Id				= id,
				.TypeName		= info.TypeName,
				.Dependencies	= info.Dependencies
			}));
	}

	for (const auto& [id, info] : myTransformedCache)
		for (const auto& depId : info.Dependencies)
		{
			auto it = myTransformedCache.find(depId);

			if (it == myTransformedCache.end())
				continue;

			it->second.Uses.emplace(id);
		}

	if (mySelectedSystem && myCache.find(*mySelectedSystem) == myCache.end())
		mySelectedSystem.reset();
}
