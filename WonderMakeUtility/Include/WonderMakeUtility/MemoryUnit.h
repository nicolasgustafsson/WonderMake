#pragma once

#include "Typedefs.h"
#include "Utility.h"

#include <type_traits>

enum class EMemoryRatio : u64
{
	B		= 1,
	KB		= 1000,
	MB		= 1000000,
	GB		= 1000000000,
	TB		= 1000000000000,
	KiB		= 1024,
	MiB		= 1048576,
	GiB		= 1073741824,
	TiB		= 1099511627776,

	Bytes = 1,
	KiloBytes = 1000,
	MegaBytes = 1000000,
	GigaBytes = 1000000000,
	TeraBytes = 1000000000000,
	KibiBytes = 1024,
	MibiBytes = 1048576,
	GibiBytes = 1073741824,
	TebiBytes = 1099511627776
};

template<EMemoryRatio TRatio, typename TRep>
class MemoryUnit final
{
public:
	static constexpr auto Ratio = TRatio;
	using Rep = TRep;

	constexpr MemoryUnit() noexcept = default;
	constexpr MemoryUnit(TRep aRep) noexcept(std::is_nothrow_move_constructible_v<TRep>)
		: myValue(std::move(aRep))
	{}
	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr MemoryUnit(MemoryUnit<TRhsRatio, TRhsRep> aRhs) noexcept(std::is_nothrow_constructible_v<TRhsRep, TRep>)
		: myValue(aRhs.To<TRatio>())
	{}

	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr [[nodiscard]] bool operator==(const MemoryUnit<TRhsRatio, TRhsRep>& aRhs) const noexcept
	{
		return To<EMemoryRatio::B>() == aRhs.To<EMemoryRatio::B>();
	}
	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr [[nodiscard]] bool operator!=(const MemoryUnit<TRhsRatio, TRhsRep>& aRhs) const noexcept
	{
		return To<EMemoryRatio::B>() != aRhs.To<EMemoryRatio::B>();
	}

	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr [[nodiscard]] bool operator<(const MemoryUnit<TRhsRatio, TRhsRep>& aRhs) const noexcept
	{
		return To<EMemoryRatio::B>() < aRhs.To<EMemoryRatio::B>();
	}
	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr [[nodiscard]] bool operator>(const MemoryUnit<TRhsRatio, TRhsRep>& aRhs) const noexcept
	{
		return To<EMemoryRatio::B>() > aRhs.To<EMemoryRatio::B>();
	}
	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr [[nodiscard]] bool operator<=(const MemoryUnit<TRhsRatio, TRhsRep>& aRhs) const noexcept
	{
		return To<EMemoryRatio::B>() <= aRhs.To<EMemoryRatio::B>();
	}
	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr [[nodiscard]] bool operator>=(const MemoryUnit<TRhsRatio, TRhsRep>& aRhs) const noexcept
	{
		return To<EMemoryRatio::B>() >= aRhs.To<EMemoryRatio::B>();
	}

	constexpr MemoryUnit& operator=(TRep aRhs) noexcept(std::is_nothrow_move_assignable_v<TRep>)
	{
		myValue = std::move(aRhs);

		return *this;
	}
	template<EMemoryRatio TRhsRatio, typename TRhsRep>
	constexpr MemoryUnit& operator=(MemoryUnit<TRhsRatio, TRhsRep> aRhs) noexcept(std::is_nothrow_assignable_v<TRhsRep, TRep>)
	{
		myValue = aRhs.To<TRatio>();
		
		return *this;
	}

	template<EMemoryRatio TToRatio, typename TToRep = TRep>
	constexpr [[nodiscard]] TToRep To() const noexcept
	{
		return (static_cast<TToRep>(myValue) * static_cast<TToRep>(TRatio)) / static_cast<TToRep>(TToRatio);
	}

private:
	TRep myValue = {};

};

namespace MemoryUnitLiterals
{
	constexpr [[nodiscard]] auto operator "" _B(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::B, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _KB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::KB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _MB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::MB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _GB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::GB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _TB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::TB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _KiB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::KiB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _MiB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::MiB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _GiB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::GiB, u64>(aValue);
	}
	constexpr [[nodiscard]] auto operator "" _TiB(u64 aValue)
	{
		return MemoryUnit<EMemoryRatio::TiB, u64>(aValue);
	}
}
