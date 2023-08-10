#pragma once

#include <type_traits>

template<typename TEnum>
    requires(std::is_enum_v<TEnum>)
struct EnableEnumBitwiseOperators
    : public std::false_type
{};

template<typename TEnum>
concept CEnumWithBitwiseOperators = EnableEnumBitwiseOperators<TEnum>::value;

template<CEnumWithBitwiseOperators TEnum>
constexpr TEnum operator|(TEnum aLhs, TEnum aRhs) noexcept
{
    using type = std::underlying_type_t<TEnum>;

    return static_cast<TEnum>(static_cast<type>(aLhs) | static_cast<type>(aRhs));
}
template<CEnumWithBitwiseOperators TEnum>
constexpr TEnum operator&(TEnum aLhs, TEnum aRhs) noexcept
{
    using type = std::underlying_type_t<TEnum>;

    return static_cast<TEnum>(static_cast<type>(aLhs) & static_cast<type>(aRhs));
}
template<CEnumWithBitwiseOperators TEnum>
constexpr TEnum operator^(TEnum aLhs, TEnum aRhs) noexcept
{
    using type = std::underlying_type_t<TEnum>;

    return static_cast<TEnum>(static_cast<type>(aLhs) ^ static_cast<type>(aRhs));
}

template<typename TEnum>
	requires(std::is_enum_v<TEnum>)
class FlagSet
{
public:
	using Enum = TEnum;
	using Type = std::underlying_type_t<TEnum>;

	constexpr FlagSet() noexcept
		: myValue(static_cast<TEnum>(0))
	{}
    constexpr FlagSet(TEnum aValue) noexcept
        : myValue(aValue)
    {}
    constexpr FlagSet(const FlagSet&) noexcept = default;

    constexpr bool operator==(TEnum aFlags) const noexcept
    {
        return myValue == aFlags;
    }
    constexpr bool operator!=(TEnum aFlags) const noexcept
    {
        return myValue == aFlags;
    }

    constexpr bool TestAny(TEnum aFlags) noexcept
    {
        return (static_cast<Type>(myValue) & static_cast<Type>(aFlags)) != 0;
    }
    constexpr bool TestAll(TEnum aFlags) noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) & static_cast<Type>(aFlags)) == aFlags;
    }

    constexpr FlagSet Or(TEnum aFlags) const noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) | static_cast<Type>(aFlags));
    }
    constexpr FlagSet And(TEnum aFlags) const noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) & static_cast<Type>(aFlags));
    }
    constexpr FlagSet Xor(TEnum aFlags) const noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) ^ static_cast<Type>(aFlags));
    }

    constexpr FlagSet& operator|=(TEnum aFlags) noexcept
    {
        myValue = static_cast<TEnum>(static_cast<Type>(myValue) | static_cast<Type>(aFlags));

        return *this;
    }
    constexpr FlagSet& operator&=(TEnum aFlags) noexcept
    {
        myValue = static_cast<TEnum>(static_cast<Type>(myValue) & static_cast<Type>(aFlags));

        return *this;
    }
    constexpr FlagSet& operator^=(TEnum aFlags) noexcept
    {
        myValue = static_cast<TEnum>(static_cast<Type>(myValue) ^ static_cast<Type>(aFlags));

        return *this;
    }
    
    constexpr FlagSet operator|(TEnum aFlags) noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) | static_cast<Type>(aFlags));
    }
    constexpr FlagSet operator&(TEnum aFlags) noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) & static_cast<Type>(aFlags));
    }
    constexpr FlagSet operator^(TEnum aFlags) noexcept
    {
        return static_cast<TEnum>(static_cast<Type>(myValue) ^ static_cast<Type>(aFlags));
    }

    constexpr operator TEnum() const noexcept
    {
        return myValue;
    }

private:
	TEnum myValue;

};
