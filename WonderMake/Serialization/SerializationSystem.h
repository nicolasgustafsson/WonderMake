#pragma once

#include "Json/json.hpp"

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

	template<class TType>
	inline json Serialize(Object& aObject)
	{
		json objectJson;

		aObject.Visit([&](const std::type_index& aTypeIndex, auto& aMember)
			{
				const auto it = mySerializeFunctions.find(aTypeIndex);

				if (it == mySerializeFunctions.cend())
				{
					return;
				}

				objectJson.push_back(it->second(reinterpret_cast<const void*>(&aObject)));
			});

		return objectJson;
	}

	template<class TType>
	inline bool Deserialize(const json& aJson, Object& aObject, TType*& aType)
	{
		const std::string name = typeid(TType).name();

		const auto it = myDeserializeFunctions.find(name);

		if (it == myDeserializeFunctions.cend())
		{
			return false;
		}

		aType = reinterpret_cast<TType*>(it->second(aJson, aObject));

		return aType;
	}

	template<class TType, std::enable_if_t<std::is_base_of_v<BaseFunctionality, TType>>* = nullptr>
	inline void Register() noexcept
	{
		const std::string name = typeid(TType).name();

		mySerializeFunctions[typeid(TType)] = [name](json& aJson, void* /*aFunctionality*/)
		{
			aJson.push_back(name);
		};
		myDeserializeFunctions[name] = [](json& aJson, Object& aObject)
		{
			aObject.Add<TType>();
		};
	}

	template<class TType, std::enable_if_t<std::is_base_of_v<SComponent, TType>>* = nullptr>
	inline void Register() noexcept
	{
		const std::string name = typeid(TType).name();

		mySerializeFunctions[typeid(TType)] = [name](const void* aComponent)
		{
			const TType& component = *reinterpret_cast<const TType*>(aComponent);
			json componentJson;

			componentJson["type"] = name;
			auto& dataJson = componentJson["data"];

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

			return componentJson;
		};
		myDeserializeFunctions[name] = [](const json& aJson, Object& aObject) -> void*
		{
			const auto& dataJson = aJson["data"];

			if (!dataJson.is_object())
			{
				return nullptr;
			}

			TType& component = aObject.Add<TType>();

			meta::doForAllMembers<TType>(
				[&component, &dataJson](auto& aMember)
				{
					auto& objName = dataJson[aMember.getName()];
					if (objName.is_null())
					{
						return;
					}

					using MemberT = meta::get_member_type<decltype(aMember)>;
					if (aMember.hasSetter())
					{
						aMember.set(component, objName.template get<MemberT>());
					}
					else if (aMember.canGetRef())
					{
						aMember.getRef(component) = objName.template get<MemberT>();
					}
					else
					{
						// throw std::runtime_error("Error: can't deserialize member because it's read only");
					}
				});

			return reinterpret_cast<void*>(&component);
		};
	}

private:
	std::unordered_map<std::type_index, std::function<json(const void*)>> mySerializeFunctions;
	std::unordered_map<std::string, std::function<void*(const json&, Object&)>> myDeserializeFunctions;

};
