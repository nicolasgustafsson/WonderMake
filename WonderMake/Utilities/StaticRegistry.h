#pragma once
#include <string>
#include <set>
#include "FixedString.h"

class StaticRegistry
{
public:
	void AddEntry(const std::string& aCategory, const std::string& aEntry)
	{
		entries[aCategory].insert(aEntry);
	}

	std::unordered_map<std::string, std::set<std::string>> entries;
};

extern StaticRegistry& GetRegistry();

template<SFixedString Category, SFixedString Name>
class Registrator
{
public:
	Registrator()
	{
		GetRegistry().AddEntry(Category, std::string(Name));
	}
};

#define FORCE_INSTANTIATE(x) (x)

template<SFixedString Category, SFixedString Name>
class StaticRegistryRef
{
public:
	StaticRegistryRef()
	{
		FORCE_INSTANTIATE(Registrator);
	}

	inline static Registrator<Category, Name> _Registrator = Registrator<Category, Name>();
};
