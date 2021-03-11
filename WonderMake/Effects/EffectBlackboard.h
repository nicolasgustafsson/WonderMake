#pragma once
#include <any>
#include <variant>

struct SEffectBlackboardEntry
{
	std::variant<f32, int, std::string, SVector2f> Entry;
};

struct SEffectBlackboard
{
	Container<SEffectBlackboardEntry, Key<std::string>, Iterable> Data;

	template<typename TEntryType>
	Container<std::string, Iterable> GetAllEntryKeysForType()
	{
		Container<std::string, Iterable> allApplicableEntries;
		for (auto&& entry : Data)
		{
			std::visit([&](const auto& aEntry)
				{

				}, entry.second.Entry);
		}

		return allApplicableEntries;
	}
};

struct SEffectBlackboardEntryRef
{
	std::string EntryName;
};