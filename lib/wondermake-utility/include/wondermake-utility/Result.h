#pragma once

#include "Typedefs.h"

#include <any>
#include <variant>

template<typename TType>
struct ResultSuccess final
{
	TType Value;
};
template<>
struct ResultSuccess<void> final {};

template<typename TType>
struct ResultFailure final
{
	TType Value;
};
template<>
struct ResultFailure<void> final {};

template<typename TArg>
constexpr ResultSuccess<std::decay_t<TArg>> Ok(TArg&& aValue)
{
	return ResultSuccess<std::decay_t<TArg>>{ std::forward<TArg>(aValue) };
}
template<typename TArg>
constexpr ResultFailure<std::decay_t<TArg>> Err(TArg&& aValue)
{
	return ResultFailure<std::decay_t<TArg>>{ std::forward<TArg>(aValue) };
}

constexpr [[nodiscard]] ResultSuccess<void> Ok()
{
	return ResultSuccess<void>{};
}
constexpr [[nodiscard]] ResultFailure<void> Err()
{
	return ResultFailure<void>{};
}

namespace ResultConversions
{
	template<typename TReturnType, typename... TArgs>
	constexpr bool Has(const std::variant<TArgs...>& aValue)
	{
		static_assert((std::is_same_v<TReturnType, TArgs> || ...), "Type is not part of variant.");

		return std::holds_alternative<TReturnType>(aValue);
	}
	template<typename TReturnType, typename... TArgs>
	constexpr TReturnType& Get(std::variant<TArgs...>& aValue)
	{
		static_assert((std::is_same_v<TReturnType, TArgs> || ...), "Type is not part of variant.");

		return std::get<TReturnType>(aValue);
	}
	
	template<typename TReturnType>
	bool Has(const std::any& aValue)
	{
		return aValue.has_value() && aValue.type() == typeid(TReturnType);
	}
	template<typename TReturnType>
	TReturnType& Get(std::any& aValue)
	{
		return std::any_cast<TReturnType&>(aValue);
	}
}

template<typename TType, typename TReturnType>
concept ResultGettableResult = requires(TType& aValue, const TType& aConstValue)
{
	{ ResultConversions::Has<TReturnType>(aConstValue) } -> std::convertible_to<bool>;
	{ ResultConversions::Get<TReturnType>(aValue) } -> std::convertible_to<TReturnType&>;
};

template<typename TSuccessType, typename TErrorType, typename TRhsSuccessType, typename TRhsErrorType>
concept ConvertableResultNoThrow = 
	((std::is_void_v<TSuccessType> && std::is_void_v<TRhsSuccessType>) || std::is_nothrow_constructible_v<TSuccessType, const TRhsSuccessType&>) &&
	((std::is_void_v<TErrorType> && std::is_void_v<TRhsErrorType>) || std::is_nothrow_constructible_v<TErrorType, const TRhsErrorType&>);
template<typename TSuccessType, typename TErrorType, typename TRhsSuccessType, typename TRhsErrorType>
concept ConvertableResultRValueNoThrow =
	((std::is_void_v<TSuccessType> && std::is_void_v<TRhsSuccessType>) || std::is_nothrow_constructible_v<TSuccessType, TRhsSuccessType&&>) &&
	((std::is_void_v<TErrorType> && std::is_void_v<TRhsErrorType>) || std::is_nothrow_constructible_v<TErrorType, TRhsErrorType&&>);

template<typename TSuccessType, typename TErrorType, typename TRhsSuccessType, typename TRhsErrorType>
concept ConvertableResult = 
	((std::is_void_v<TSuccessType> && std::is_void_v<TRhsSuccessType>) || std::is_constructible_v<TSuccessType, const TRhsSuccessType&>) &&
	((std::is_void_v<TErrorType> && std::is_void_v<TRhsErrorType>) || std::is_constructible_v<TErrorType, const TRhsErrorType&>);
template<typename TSuccessType, typename TErrorType, typename TRhsSuccessType, typename TRhsErrorType>
concept ConvertableResultRValue = 
	((std::is_void_v<TSuccessType> && std::is_void_v<TRhsSuccessType>) || std::is_constructible_v<TSuccessType, TRhsSuccessType&&>) &&
	((std::is_void_v<TErrorType> && std::is_void_v<TRhsErrorType>) || std::is_constructible_v<TErrorType, TRhsErrorType&&>);

template<typename TSuccessType, typename TErrorType = void>
class Result final
{
private:
	inline static constexpr size_t IndexSuccess	= 0;
	inline static constexpr size_t IndexError	= 1;

public:
	template<typename TRhsSuccessType, typename TRhsErrorType>
	constexpr Result(const Result<TRhsSuccessType, TRhsErrorType>& aResult)
		noexcept(ConvertableResultNoThrow<TSuccessType, TErrorType, TRhsSuccessType, TRhsErrorType>)
		requires(ConvertableResult<TSuccessType, TErrorType, TRhsSuccessType, TRhsErrorType>)
		: myResult(MakeResult(aResult))
	{}
	template<typename TRhsSuccessType, typename TRhsErrorType>
	constexpr Result(Result<TRhsSuccessType, TRhsErrorType>&& aResult)
		noexcept(ConvertableResultRValueNoThrow<TSuccessType, TErrorType, TRhsSuccessType&&, TRhsErrorType&&>)
		requires(ConvertableResultRValue<TSuccessType, TErrorType, TRhsSuccessType&&, TRhsErrorType&&>)
		: myResult(MakeResult(std::move(aResult)))
	{}
	template<typename TRhsSuccessType, typename TRhsErrorType>
	constexpr Result(const Result<TRhsSuccessType, TRhsErrorType>& aResult)
		requires(!std::is_constructible_v<TSuccessType, const TRhsSuccessType&> && std::is_constructible_v<TErrorType, const TRhsErrorType&>)
		: myResult(std::in_place_index<IndexError>, aResult.Err())
	{}
	template<typename TRhsSuccessType, typename TRhsErrorType>
	constexpr Result(Result<TRhsSuccessType, TRhsErrorType>&& aResult)
		requires(!std::is_constructible_v<TSuccessType, TRhsSuccessType&&> && std::is_constructible_v<TErrorType, TRhsErrorType&&>)
		: myResult(std::in_place_index<IndexError>, std::move(aResult).Err())
	{}
	
	template<typename TRhsSuccessType, typename TRhsErrorType>
	constexpr Result(const Result<TRhsSuccessType, TRhsErrorType>& aResult)
		requires(!std::is_constructible_v<TErrorType, const TRhsErrorType&> && std::is_constructible_v<TSuccessType, const TRhsSuccessType&>)
		: myResult(std::in_place_index<IndexSuccess>, aResult.Unwrap())
	{}
	template<typename TRhsSuccessType, typename TRhsErrorType>
	constexpr Result(Result<TRhsSuccessType, TRhsErrorType>&& aResult)
		requires(!std::is_constructible_v<TErrorType, TRhsErrorType&&> && std::is_constructible_v<TSuccessType, TRhsSuccessType&&>)
		: myResult(std::in_place_index<IndexSuccess>, std::move(aResult).Unwrap())
	{}
	
	template<typename TArg>
	constexpr Result(ResultSuccess<TArg> aValue) noexcept(std::is_nothrow_constructible_v<TSuccessType, TArg&&>)
		requires(!std::is_void_v<TSuccessType> && std::is_constructible_v<TSuccessType, TArg&&>)
		: myResult(std::in_place_index<IndexSuccess>, std::move(aValue.Value))
	{}
	template<typename TArg>
	constexpr Result(ResultFailure<TArg> aValue) noexcept(std::is_nothrow_constructible_v<TErrorType, TArg&&>)
		requires(!std::is_void_v<TErrorType> && std::is_constructible_v<TErrorType, TArg&&>)
		: myResult(std::in_place_index<IndexError>, std::move(aValue.Value))
	{}
	
	constexpr Result(ResultSuccess<void> aValue) noexcept
		requires(std::is_void_v<TSuccessType>)
		: myResult(std::in_place_index<IndexSuccess>, aValue)
	{}
	constexpr Result(ResultFailure<void> aValue) noexcept
		requires(std::is_void_v<TErrorType>)
		: myResult(std::in_place_index<IndexError>, aValue)
	{}

	constexpr [[nodiscard]] bool operator==(const Result&) const noexcept = default;
	constexpr [[nodiscard]] bool operator!=(const Result&) const noexcept = default;

	constexpr [[nodiscard]] operator bool() const noexcept
	{
		return IsOk();
	}

	constexpr [[nodiscard]] bool IsOk() const noexcept
	{
		return myResult.index() == IndexSuccess;
	}
	constexpr [[nodiscard]] bool IsErr() const noexcept
	{
		return myResult.index() == IndexError;
	}
	
	constexpr [[nodiscard]] const auto& Unwrap() const& noexcept
		requires(!std::is_void_v<TSuccessType>)
	{
		return std::get<IndexSuccess>(myResult).Value;
	}
	constexpr [[nodiscard]] auto& Unwrap() & noexcept
		requires(!std::is_void_v<TSuccessType>)
	{
		return std::get<IndexSuccess>(myResult).Value;
	}
	constexpr [[nodiscard]] auto Unwrap() &&
		requires(!std::is_void_v<TSuccessType>)
	{
		return std::move(std::get<IndexSuccess>(myResult).Value);
	}
	
	template<typename TArg>
	constexpr [[nodiscard]] TSuccessType UnwrapOr(TArg&& aArg) const noexcept(std::is_nothrow_constructible_v<TSuccessType, decltype(std::forward<TArg>(aArg))> && std::is_nothrow_copy_constructible_v<TSuccessType>)
		requires(!std::is_void_v<TSuccessType> && std::is_constructible_v<TSuccessType, decltype(std::forward<TArg>(aArg))> && std::is_copy_constructible_v<TSuccessType>)
	{
		if (myResult.index() != IndexSuccess)
			return std::forward<TArg>(aArg);

		return std::get<IndexSuccess>(myResult).Value;
	}
	template<typename TArg>
	constexpr [[nodiscard]] TSuccessType UnwrapOr(TArg&& aArg) && noexcept(std::is_nothrow_constructible_v<TSuccessType, decltype(std::forward<TArg>(aArg))>)
		requires(!std::is_void_v<TSuccessType> && std::is_constructible_v<TSuccessType, decltype(std::forward<TArg>(aArg))>)
	{
		if (myResult.index() != IndexSuccess)
			return std::forward<TArg>(aArg);

		return std::move(std::get<IndexSuccess>(myResult).Value);
	}
	
	constexpr [[nodiscard]] const auto& Err() const&
		requires(!std::is_void_v<TErrorType>)
	{
		return std::get<IndexError>(myResult).Value;
	}
	constexpr [[nodiscard]] auto& Err() &
		requires(!std::is_void_v<TErrorType>)
	{
		return std::get<IndexError>(myResult).Value;
	}
	constexpr [[nodiscard]] auto Err() &&
		requires(!std::is_void_v<TErrorType>)
	{
		return std::move(std::get<IndexError>(myResult).Value);
	}
	
	class ResultWrapper
	{
	public:
		constexpr ResultWrapper(std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>& aResult)
			: myResult(aResult)
		{}

		template<typename TType = TSuccessType, typename TCallable>
		constexpr ResultWrapper AndThen(TCallable&& aCallable)
		{
			if (myResult.index() != IndexSuccess)
				return myResult;

			static_assert(std::is_void_v<TType> || std::is_same_v<TType, TSuccessType> || ResultGettableResult<TSuccessType, TType>);

			if constexpr (std::is_void_v<TType>)
				(void)std::forward<TCallable>(aCallable)();
			else if constexpr (std::is_same_v<TType, TSuccessType>)
				(void)std::forward<TCallable>(aCallable)(Unwrap());
			else if constexpr (ResultGettableResult<TSuccessType, TType>)
			{
				if (ResultConversions::Has<TType>(Unwrap()))
					(void)std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Unwrap()));
			}

			return myResult;
		}
		template<typename TType = TErrorType, typename TCallable>
		constexpr ResultWrapper OrElse(TCallable&& aCallable)
		{
			if (myResult.index() != IndexError)
				return myResult;

			static_assert(std::is_void_v<TType> || std::is_same_v<TType, TErrorType> || ResultGettableResult<TErrorType, TType>);

			if constexpr (std::is_void_v<TType>)
				(void)std::forward<TCallable>(aCallable)();
			else if constexpr (std::is_same_v<TType, TErrorType>)
				(void)std::forward<TCallable>(aCallable)(Err());
			else if constexpr (ResultGettableResult<TErrorType, TType>)
			{
				if (ResultConversions::Has<TType>(Err()))
					(void)std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Err()));
			}

			return myResult;
		}

	private:
		constexpr auto& Unwrap()
			requires(!std::is_void_v<TSuccessType>)
		{
			return std::get<IndexSuccess>(myResult).Value;
		};
		constexpr auto& Err()
			requires(!std::is_void_v<TErrorType>)
		{
			return std::get<IndexError>(myResult).Value;
		};

		std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>& myResult;

	};

	template<typename TType = TSuccessType, typename TCallable>
	constexpr ResultWrapper AndThen(TCallable&& aCallable)
	{
		return ResultWrapper(myResult).AndThen<TType>(std::forward<TCallable>(aCallable));
	}
	template<typename TType = TErrorType, typename TCallable>
	constexpr ResultWrapper OrElse(TCallable&& aCallable)
	{
		return ResultWrapper(myResult).OrElse<TType>(std::forward<TCallable>(aCallable));
	}

private:
	template<typename TRhsSuccessType, typename TRhsErrorType>
	inline static std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>> MakeResult(const Result<TRhsSuccessType, TRhsErrorType>& aResult)
	{
		if (aResult)
		{
			if constexpr (std::is_void_v<TSuccessType>)
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexSuccess>);
			else
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexSuccess>, aResult.Unwrap());
		}
		else
		{
			if constexpr (std::is_void_v<TErrorType>)
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexError>);
			else
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexError>, aResult.Err());
		}
	}
	template<typename TRhsSuccessType, typename TRhsErrorType>
	inline static std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>> MakeResult(Result<TRhsSuccessType, TRhsErrorType>&& aResult)
	{
		if (aResult)
		{
			if constexpr (std::is_void_v<TSuccessType>)
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexSuccess>);
			else
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexSuccess>, std::move(aResult).Unwrap());
		}
		else
		{
			if constexpr (std::is_void_v<TErrorType>)
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexError>);
			else
				return std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>(std::in_place_index<IndexError>, std::move(aResult).Err());
		}
	}

	std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>> myResult;

};
