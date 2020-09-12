#pragma once
#include <json/json.hpp>
#include <array>
#include "Utilities/Vector.h"

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

	template<>
	inline void SerializeInput<SVector2u>(const i32 aNodeId, const i32 aSlotId, json& aJson, SVector2u aSlotValue)
	{
		std::array<u32, 2> arr{ aSlotValue.X, aSlotValue.Y };
		aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", arr} }));
	}

	//template<typename T>
	//inline void SerializeInput(const i32 aNodeId, const i32 aSlotId, json& aJson, T aSlotValue) requires std::is_enum_v<T>
	//{
	//	aJson.push_back(aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", static_cast<i32>(aSlotValue) } }));
	//}

	template<typename SRenderSettings>
	inline void SerializeInput(const i32 aNodeId, const i32 aSlotId, json& aJson, SRenderSettings aSlotValue)
	{
		auto& outer = aJson.object({ {"NodeId", aNodeId}, {"SlotId", aSlotId} });
		aJson.push_back();

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

	template<>
	inline SVector2u DeserializeInput<SVector2u>(const json& aJson)
	{
		std::array<u32, 2> arr = (aJson["Value"].get<std::array<u32, 2>>());

		return SVector2u(arr[0], arr[1]);
	}
}
