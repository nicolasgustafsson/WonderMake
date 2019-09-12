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

class BaseFunctionality;
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

};

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

		if constexpr (std::is_base_of<SComponent, TType>::value)
		{
			const TType& component = *static_cast<const TType*>(reinterpret_cast<const SComponent*>(aMember));
			json& dataJson = memberJson["data"];

			meta::doForAllMembers<TType>([&component, &dataJson](const auto& aMember)
				{
					auto& valueName = dataJson[aMember.getName()];
					if (aMember.canGetConstRef())
					{
						valueName = aMember.get(component);
					}
					else if (aMember.hasGetter())
					{
						valueName = aMember.getCopy(component);
					}
				});
		}

		return memberJson;
	};
	myDeserializeFunctions[name] = [](const json& aDataJson, Object& aObject)
	{
		aDataJson;
		TType& member = aObject.Add<TType>();
		member;

		if constexpr (std::is_base_of<SComponent, TType>::value)
		{
			meta::doForAllMembers<TType>([&member, &aDataJson](auto& aMember)
				{
					const auto it = aDataJson.find(aMember.getName());
					if (it == aDataJson.cend()
						|| it->is_null())
					{
						return;
					}

					using MemberT = meta::get_member_type<decltype(aMember)>;
					if (aMember.hasSetter())
					{
						aMember.set(member, it->template get<MemberT>());
					}
					else if (aMember.canGetRef())
					{
						aMember.getRef(member) = it->template get<MemberT>();
					}
				});
		}
	};
}
