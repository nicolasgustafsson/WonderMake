#pragma once

#include "Json/json.hpp"

#include "Message/Messages.h"

#include "Object/Object.h"

#include "System/System.h"

#include <MetaStuff/Meta.h>

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>

struct SComponent;

class SerializationSystem final
	: public System
{
public:

	json Serialize(Object& aObject) const;
	bool Deserialize(const json& aJson, Object& aObject) const;

	template<class TType>
	inline void Register() noexcept;

private:
	std::unordered_map<std::type_index, std::function<json(const void*)>> mySerializeFunctions;
	std::unordered_map<std::string, std::function<void(const json&, Object&)>> myDeserializeFunctions;

	template<class TType>
	inline static json SerializeMember(const TType& aValue) noexcept;
	template<class TType>
	inline static void DeserializeMember(const json& aJsonData, TType& aValue) noexcept;

};

template<class TType>
inline json SerializationSystem::SerializeMember(const TType& aValue) noexcept
{
	json dataJson;

	meta::doForAllMembers<TType>([&dataJson, &aValue](const auto& aMember)
		{
			using TMember = meta::get_member_type<decltype(aMember)>;

			auto& valueName = dataJson[aMember.getName()];
			
			if constexpr (meta::isRegistered<TMember>())
			{
				if (aMember.canGetConstRef())
				{
					valueName = SerializeMember(aMember.get(aValue));
				}
				else if (aMember.hasGetter())
				{
					valueName = SerializeMember(aMember.getCopy(aValue));
				}
			}
			else
			{
				if (aMember.canGetConstRef())
				{
					valueName = aMember.get(aValue);
				}
				else if (aMember.hasGetter())
				{
					valueName = aMember.getCopy(aValue);
				}
			}
		});

	return dataJson;
}

template<class TType>
inline void SerializationSystem::DeserializeMember(const json& aDataJson, TType& aValue) noexcept
{
	meta::doForAllMembers<TType>([&aValue, &aDataJson](auto& aMember)
		{
			const auto it = aDataJson.find(aMember.getName());

			if (it == aDataJson.cend()
				|| it->is_null())
			{
				return;
			}

			using TMember = meta::get_member_type<decltype(aMember)>;

			if constexpr (meta::isRegistered<TMember>())
			{
				if (aMember.canGetRef())
				{
					DeserializeMember(*it, aMember.getRef(aValue));
				}
				else if (aMember.hasSetter())
				{
					TMember member;
					DeserializeMember(*it, member);
					aMember.set(aValue, member);
				}
			}
			else
			{
				if (aMember.hasSetter())
				{
					aMember.set(aValue, it->template get<TMember>());
				}
				else if (aMember.canGetRef())
				{
					aMember.getRef(aValue) = it->template get<TMember>();
				}
			}
		});
}

template<class TType>
inline void SerializationSystem::Register() noexcept
{
	std::string name = typeid(TType).name();
	size_t spaceIndex = name.find(' ');

	if (spaceIndex != std::string::npos)
	{
		name = name.substr(spaceIndex + 1);
	}

	mySerializeFunctions[typeid(TType)] = [name](const void* aMember)
	{
		aMember;
		json memberJson;

		memberJson["type"] = name;

		if constexpr (meta::isRegistered<TType>())
		{
			const TType& component = *static_cast<const TType*>(reinterpret_cast<const SComponent*>(aMember));

			memberJson["data"] = SerializeMember(component);
		}

		return memberJson;
	};
	myDeserializeFunctions[name] = [](const json& aDataJson, Object& aObject)
	{
		aDataJson;
		TType& member = aObject.Add<TType, false>();
		member;

		if constexpr (meta::isRegistered<TType>())
		{
			DeserializeMember(aDataJson, member);
		}
	};
}
