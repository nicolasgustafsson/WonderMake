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

	template<>
	inline void SerializeInput<SRenderSettings>(const i32 aNodeId, const i32 aSlotId, json& aJson, SRenderSettings aSlotValue)
	{
		i32 blendModeInt = aSlotValue.BlendMode ? static_cast<i32>(*aSlotValue.BlendMode) : -1;
		i32 depthModeInt = aSlotValue.DepthMode ? static_cast<i32>(*aSlotValue.DepthMode) : -1;
		
		json renderSettings{ {"Blendmode", blendModeInt}, {"DepthMode", depthModeInt} };
		
		aJson.push_back({ {"NodeId", aNodeId}, {"SlotId", aSlotId}, {"Value", renderSettings} });
	}

	template<typename TSlotType>
	inline TSlotType DeserializeInput(const json&)
	{
		return {};
	}

	//[Nicos]: if we can serialize it we should be able to deserialize it
	template<typename TSlotType> requires AutomaticallySerializable<TSlotType> 
	inline TSlotType DeserializeInput(const json& aJson)
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

	template<>
	inline SRenderSettings DeserializeInput<SRenderSettings>(const json& aJson)
	{
		SRenderSettings settings;

		const i32 blendmode = aJson["Value"]["Blendmode"].get<i32>();
		if (blendmode >= 0)
			settings.BlendMode = static_cast<EBlendMode>(blendmode);

		const i32 depthMode = aJson["Value"]["DepthMode"].get<i32>();
		if (depthMode >= 0)
			settings.DepthMode = static_cast<EDepthMode>(depthMode);

		return settings;
	}
}
