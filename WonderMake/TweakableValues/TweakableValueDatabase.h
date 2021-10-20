#pragma once
#include "Utilities/StaticRegistry/FixedString.h"
#include "Utilities/StaticRegistry/StaticRegistry.h"

template<typename TEntryType>
struct STweakableValueEntry
{
	std::string Type;
	std::string Name;
	TEntryType Entry;
};



class TweakableValueDatabase : public System<>
                               , public Debugged
{
public:
	TweakableValueDatabase()
		: Debugged("Tweakable Values")
	{
		std::ifstream debugSettingsFile("tweakableValues.json", std::fstream::app);

		std::string fileContents((std::istreambuf_iterator<char>(debugSettingsFile)),
			(std::istreambuf_iterator<char>()));

		if (json::accept(fileContents))
			myValues = json::parse(fileContents);
	}

	void Save();

	template<typename T, SFixedString TName>
	T GetOrAddValue(T aInitialValue);

protected:
	void Debug() override;

	void InspectValue(nlohmann::json& aValue, std::string_view name);
	//
	//template<typename T>
	//void InspectValue(std::string_view aName, T& aValue);
private:
	nlohmann::json myValues;
};

template <typename T, SFixedString TName>
T TweakableValueDatabase::GetOrAddValue(T aInitialValue)
{
	//StaticRegistryRef<"Tweakables", TName> test;

	STweakableValueEntry<T> entry;

	if (myValues.find(std::string(TName)) != myValues.end())
	{
		auto test = myValues[std::string(TName)];

		return test[1].get<T>();
	}

	myValues[std::string(TName)] = {typeid(aInitialValue).name(), aInitialValue};

	WmLog(typeid(aInitialValue).name());

	return aInitialValue;
}

template<typename T, SFixedString Name>
struct STweakable
{
public:
	STweakable(const T aInitialValue = {})
		:myInitialValue(aInitialValue)
	{
		SystemPtr<TweakableValueDatabase>()->GetOrAddValue<T, Name>(myInitialValue);
	}

	operator T() const
	{
		return SystemPtr<TweakableValueDatabase>()->GetOrAddValue<T, Name>(myInitialValue);
	}

private:
	const T myInitialValue;
};