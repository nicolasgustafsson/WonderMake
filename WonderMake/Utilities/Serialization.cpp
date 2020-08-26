#include "pch.h"
#include "Serialization.h"

json Serialization::Serialize(Object& aObject) const
{
	json memberJson;

	aObject.Visit([&](const std::type_index& aTypeIndex, const auto aMember)
		{
			const auto it = mySerializeFunctions.find(aTypeIndex);

			if (it == mySerializeFunctions.cend())
			{
				return;
			}

			memberJson.emplace_back(it->second(aMember));
		});

	return memberJson;
}

bool Serialization::Deserialize(const json& aJson, Object& aObject) const
{
	if (!aJson.is_array())
	{
		WmLog("Failed to parse object json, not an array.");
		return false;
	}

	for (const auto& member : aJson)
	{
		const auto typeIt = member.find("type");

		if (typeIt == member.cend())
		{
			WmLog("Failed to parse object element, type missing.");
			continue;
		}

		const auto deserializationIt = myDeserializeFunctions.find(*typeIt);

		if (deserializationIt == myDeserializeFunctions.cend())
		{
			WmLog("Failed to parse object element, type unknown. ", *typeIt);
			continue;
		}

		const auto dataIt = member.find("data");

		deserializationIt->second(dataIt != member.cend() ? *dataIt : json(), aObject);
	}

	return true;
}
