#pragma once

#include <typeindex>

class SystemId final
{
public:
	template<typename TSystem>
    inline static SystemId Create() noexcept;

	[[nodiscard]] inline bool operator==(const SystemId aRhs) const noexcept;
	[[nodiscard]] inline bool operator!=(const SystemId aRhs) const noexcept;

	[[nodiscard]] inline size_t Hash() const noexcept;

private:
	template<typename TDependency>
	class Key {};

	size_t myTypeHash = 0;

	inline SystemId(const size_t aTypeHash) noexcept;
};

namespace std
{
	template <>
	struct hash<SystemId>
	{
		[[nodiscard]] inline size_t operator()(const SystemId aId) const noexcept
		{
			return aId.Hash();
		}
	};
}

template<typename TSystem>
inline SystemId SystemId::Create() noexcept
{
	return SystemId(typeid(Key<TSystem>).hash_code());
}

[[nodiscard]] inline bool SystemId::operator==(const SystemId aRhs) const noexcept
{
	return myTypeHash == aRhs.myTypeHash;
}

[[nodiscard]] inline bool SystemId::operator!=(const SystemId aRhs) const noexcept
{
	return myTypeHash != aRhs.myTypeHash;
}

[[nodiscard]] inline size_t SystemId::Hash() const noexcept
{
	return myTypeHash;
}

inline SystemId::SystemId(const size_t aTypeHash) noexcept
	: myTypeHash(aTypeHash)
{}
