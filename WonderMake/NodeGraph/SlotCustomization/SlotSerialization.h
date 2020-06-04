#pragma once
#include <json/json.hpp>

namespace InputSlotSerialization
{
	template<typename TSlotType> //[Nicos]: I use maybe_unused here to keep the names as an example
	void SerializeInput([[maybe_unused]] const i32 aNodeId, [[maybe_unused]] const i32 aSlotId, [[maybe_unused]] json& aJson, [[maybe_unused]] TSlotType aSlotValue)
	{

	}

	template<typename TSlotType>
	concept AutomaticallySerializable = requires(const i32 aNodeId, const i32 aSlotId, json & aJson, TSlotType aSlotValue) 
	{ 
		aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", aSlotValue} })); 
	};

	template<typename TSlotType> requires AutomaticallySerializable<TSlotType>
	void SerializeInput(const i32 aNodeId, const i32 aSlotId, json& aJson, TSlotType aSlotValue)
	{
		aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", aSlotValue} }));
	}

	template<>
	inline void SerializeInput<std::filesystem::path>(const i32 aNodeId, const i32 aSlotId, json& aJson, std::filesystem::path aSlotValue)
	{
		aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", aSlotValue.string()} }));
	}

	template<typename TSlotType>
	TSlotType DeserializeInput(const json&)
	{
		return TSlotType();
	}

	//[Nicos]: if we can serialize it we should be able to deserialize it
	template<typename TSlotType> requires AutomaticallySerializable<TSlotType> 
		TSlotType DeserializeInput(const json& aJson)
		{
			return aJson["Value"].get<TSlotType>();
		}

		template<>
		inline std::filesystem::path DeserializeInput<std::filesystem::path>(const json& aJson)
			{
				return std::filesystem::path(aJson["Value"].get<std::string>());
			}
}
