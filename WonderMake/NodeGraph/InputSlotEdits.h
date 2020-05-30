#pragma once
#include <json/json.hpp>

namespace InputSlotEdits
{
	template<typename TSlotType>
	void EditInputSlot(TSlotType&)
	{
	
	}

	template<>
	inline void EditInputSlot<float>(float& aInput)
	{
		ImGui::InputFloat("", &aInput);
	}

	template<>
	inline void EditInputSlot<std::string>(std::string& aInput)
	{
		ImGui::InputText("", &aInput);
	}

	template<>
	inline void EditInputSlot<SColor>(SColor& aInput)
	{
		ImGui::ColorEdit4("", ((f32*)&aInput));
	}
};


namespace InputSlotSerialization
{
	template<typename TSlotType>
	void SerializeInput(const i32 aNodeId, const i32 aSlotId, json& aJson, TSlotType aSlotValue)
	{

	}

	template<typename TSlotType>
	concept AutomaticallySerializable = requires(const i32 aNodeId, const i32 aSlotId, json& aJson, TSlotType aSlotValue) { aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", aSlotValue} })); };

	template<typename TSlotType> requires AutomaticallySerializable<TSlotType>
	void SerializeInput(const i32 aNodeId, const i32 aSlotId, json& aJson, TSlotType aSlotValue) 
	{
		TSlotType defaultValue = {};
		if (aSlotValue == defaultValue)
			return;

		aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", aSlotValue} }));
	}

	template<typename TSlotType>
	TSlotType DeserializeInput(const json& aJson)
	{
		return TSlotType();
	}

	//[Nicos]: if we can serialize it we should be able to deserialize it
	template<typename TSlotType> requires AutomaticallySerializable<TSlotType>
	TSlotType DeserializeInput(const json& aJson)
	{
		return aJson["Value"].get<TSlotType>();
	}
}
