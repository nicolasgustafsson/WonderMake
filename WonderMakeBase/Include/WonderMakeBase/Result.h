#pragma once

#include "Typedefs.h"

#include <optional>
#include <type_traits>
#include <variant>

struct _SuccessType
{
	constexpr [[nodiscard]] bool operator==(const _SuccessType&) const noexcept
	{
		return true;
	}
};

constexpr _SuccessType Success;

template<typename TErrorType, typename TSuccessType = _SuccessType>
class Result
{
public:
	constexpr Result(TSuccessType aSuccess, u32 aMetaValue = 0) noexcept(std::is_nothrow_move_constructible_v<TSuccessType>)
		: myResult(std::move(aSuccess))
		, myMetaValue(aMetaValue)
	{}
	constexpr Result(TErrorType aError, u32 aMetaValue = 0) noexcept(std::is_nothrow_move_constructible_v<TErrorType>)
		: myResult(std::move(aError))
		, myMetaValue(aMetaValue)
	{}

	constexpr [[nodiscard]] bool operator==(const TSuccessType& aRhs) const noexcept
	{
		return std::holds_alternative<TSuccessType>(myResult)
			&& std::get<TSuccessType>(myResult) == aRhs;
	}
	constexpr [[nodiscard]] bool operator==(const TErrorType& aRhs) const noexcept
	{
		return std::holds_alternative<TErrorType>(myResult)
			&& std::get<TErrorType>(myResult) == aRhs;
	}

	constexpr [[nodiscard]] operator bool() const
	{
		return std::holds_alternative<TSuccessType>(myResult);
	}

	constexpr [[nodiscard]] operator TErrorType() const requires(std::is_copy_constructible_v<TErrorType>&& std::is_copy_assignable_v<TErrorType>)
	{
		return std::get<TErrorType>(myResult);
	}
	constexpr [[nodiscard]] operator TErrorType()&&
	{
		return std::move(std::get<TErrorType>(myResult));
	}

	constexpr [[nodiscard]] operator TSuccessType() const requires(std::is_copy_constructible_v<TSuccessType> && std::is_copy_assignable_v<TSuccessType>)
	{
		return std::get<TSuccessType>(myResult);
	}
	constexpr [[nodiscard]] operator TSuccessType() &&
	{
		return std::move(std::get<TSuccessType>(myResult));
	}

	constexpr [[nodiscard]] u32 Meta() const
	{
		return myMetaValue;
	}

private:
	std::variant<TErrorType, TSuccessType> myResult;
	u32 myMetaValue = 0;

};
