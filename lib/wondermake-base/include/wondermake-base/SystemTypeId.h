#pragma once

#include <typeindex>

class SystemTypeId final
{
public:
	template<typename TSystem>
	inline static SystemTypeId Create() noexcept;

	[[nodiscard]] inline bool operator==(const SystemTypeId aRhs) const noexcept;
	[[nodiscard]] inline bool operator!=(const SystemTypeId aRhs) const noexcept;

	[[nodiscard]] inline size_t Hash() const noexcept;

private:
	template<typename TDependency>
	class Key {};

	size_t myTypeHash = 0;

	inline SystemTypeId(const size_t aTypeHash) noexcept;
};

namespace std
{
	template <>
	struct hash<SystemTypeId>
	{
		[[nodiscard]] inline size_t operator()(const SystemTypeId aId) const noexcept
		{
			return aId.Hash();
		}
	};
}

template<typename TSystem>
inline static SystemTypeId SystemTypeId::Create() noexcept
{
	return SystemTypeId(typeid(Key<TSystem>).hash_code());
}

[[nodiscard]] inline bool SystemTypeId::operator==(const SystemTypeId aRhs) const noexcept
{
	return myTypeHash == aRhs.myTypeHash;
}

[[nodiscard]] inline bool SystemTypeId::operator!=(const SystemTypeId aRhs) const noexcept
{
	return myTypeHash != aRhs.myTypeHash;
}

[[nodiscard]] inline size_t SystemTypeId::Hash() const noexcept
{
	return myTypeHash;
}

inline SystemTypeId::SystemTypeId(const size_t aTypeHash) noexcept
	: myTypeHash(aTypeHash)
{}
