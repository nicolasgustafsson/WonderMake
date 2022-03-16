#include "pch.h"
#include "TweakableValueDatabase.h"

#include <iostream>

#include "NodeGraph/SlotCustomization/SlotInputEdits.h"

REGISTER_SYSTEM(TweakableValueDatabase);

void TweakableValueDatabase::Save()
{
	std::ofstream file("tweakableValues.json");

	file << myValues.dump(4);
}

void TweakableValueDatabase::Debug()
{
	ImGui::Begin("Tweakable Values");
	for(auto&& it : myValues.get_ref<json::object_t&>())
	{
		InspectValue(it.second, it.first);
	}

	ImGui::End();
}

void TweakableValueDatabase::InspectValue(nlohmann::json& aValue, std::string_view aName)
{
	ImGui::Text(aName.data());
	ImGui::SameLine();
	ImGui::PushID(&aValue);

	bool changed = false;

	if (aValue[0].get<std::string>() == TypeString<i32>())
	{
		auto&& ref = aValue[1].get<i32>();
		auto first = ref;
		SlotInputEdits::EditInputSlot(ref);
		aValue[1] = ref;
		changed = first != ref;
	}
	else if (aValue[0].get<std::string>() == TypeString<f32>())
	{
		auto&& ref = aValue[1].get<f32>();
		auto first = ref;
		SlotInputEdits::EditInputSlot(ref);
		aValue[1] = ref;
		changed = first != ref;
	}
	else if (aValue[0].get<std::string>() == TypeString<SVector2f>())
	{
		auto&& ref = aValue[1].get<SVector2f>();
		auto first = ref;
		SlotInputEdits::EditInputSlot(ref);
		aValue[1] = ref;
	}
	else if (aValue[0].get<std::string>() == TypeString<SColor>())
	{
		auto&& ref = aValue[1].get<SColor>();
		auto first = ref;
		SlotInputEdits::EditInputSlot(ref);
		aValue[1] = ref;
		changed = first != ref;
	}
	else if (aValue[0].get<std::string>() == TypeString<bool>())
	{
		auto&& ref = aValue[1].get<bool>();
		auto first = ref;
		SlotInputEdits::EditInputSlot(ref);
		aValue[1] = ref;
		changed = first != ref;
	}
	else if (aValue[0].get<std::string>() == TypeString<std::string>())
	{
		auto&& ref = aValue[1].get<std::string>();
		auto first = ref;
		SlotInputEdits::EditInputSlot(ref);
		aValue[1] = ref;
		changed = first != ref;
	}
	else
	{
		ImGui::Text("????");
	}

	if (changed)
		Save();

	ImGui::PopID();
}