#pragma once

#include "wondermake-utility/TupleUtility.h"
#include "wondermake-utility/Typedefs.h"

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
	template<typename TType, typename TRequiredTypesTuple>
	struct IsAll
		: std::false_type
	{};

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
	template<typename TRequiredTypesTuple, typename... TArgs>
	struct IsAll<std::variant<TArgs...>, TRequiredTypesTuple>
		: std::bool_constant<TupleContainsAllTypesInTuple_v<std::tuple<TArgs...>, TRequiredTypesTuple>>
	{};
	
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
concept CResultGettableResult = requires(TType& aValue, const TType& aConstValue)
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
	using SuccessType	= TSuccessType;
	using ErrorType		= TErrorType;

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

	template<typename TArg>
	constexpr [[nodiscard]] bool operator==(const ResultSuccess<TArg>& aRhs) const noexcept
	{
		if (!IsOk())
			return false;

		return Unwrap() == aRhs.Value;
	}
	template<typename TArg>
	constexpr [[nodiscard]] bool operator!=(const ResultSuccess<TArg>& aRhs) const noexcept
	{
		return !(*this == aRhs);
	}

	template<typename TArg>
	constexpr [[nodiscard]] bool operator==(const ResultFailure<TArg>& aRhs) const noexcept
	{
		if (!IsErr())
			return false;

		return Err() == aRhs.Value;
	}
	template<typename TArg>
	constexpr [[nodiscard]] bool operator!=(const ResultFailure<TArg>&aRhs) const noexcept
	{
		return !(*this == aRhs);
	}

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

private:
	template<typename TRetType>
	struct SRetValueWrapper
	{
		std::optional<TRetType> RetVal;

		template<typename TArg>
		inline constexpr void Emplace(TArg&& aArg)
		{
			RetVal.emplace(std::forward<TArg>(aArg));
		}
	};

	template<>
	struct SRetValueWrapper<void>
	{
		inline constexpr void Emplace(auto&&...) noexcept {}
	};

	struct AllValues {};

	template<typename TType, typename TCheckedValues>
	struct CheckedAllValues
		: std::bool_constant<std::is_same_v<TCheckedValues, AllValues> || ResultConversions::IsAll<TType, TCheckedValues>::value>
	{};
	template<typename TType, typename>
	struct AddCheckedValue
	{
		using type = void;
	};
	template<typename TType, typename... TTypes>
	struct AddCheckedValue<TType, std::tuple<TTypes...>>
	{
		using type = std::tuple<TType, TTypes...>;
	};

public:

	template<typename TRetType, typename TCheckedSuccessValues, typename TCheckedErrorValues>
	class ResultWrapper
	{
	public:
		constexpr ResultWrapper(std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>& aResult)
			: myResult(aResult)
		{}
		constexpr ResultWrapper(std::variant<ResultSuccess<TSuccessType>, ResultFailure<TErrorType>>& aResult, SRetValueWrapper<TRetType>&& aRetVal)
			: myResult(aResult)
			, myRetVal(std::move(aRetVal))
		{}

		template<typename TType = TSuccessType, typename TCallable>
		constexpr ResultWrapper<TRetType, std::conditional_t<std::is_same_v<TType, TSuccessType>, AllValues, typename AddCheckedValue<TType, TCheckedSuccessValues>::type>, TCheckedErrorValues> AndThen(TCallable&& aCallable) &&
		{
			if (myResult.index() != IndexSuccess)
				return { myResult, std::move(myRetVal) };

			static_assert(std::is_void_v<TType> || std::is_same_v<TType, TSuccessType> || CResultGettableResult<TSuccessType, TType>);

			if constexpr (std::is_void_v<TRetType>)
			{
				if constexpr (std::is_void_v<TType>)
					(void)std::forward<TCallable>(aCallable)();
				else if constexpr (std::is_same_v<TType, TSuccessType>)
					(void)std::forward<TCallable>(aCallable)(Unwrap());
				else if constexpr (CResultGettableResult<TSuccessType, TType>)
				{
					if (ResultConversions::Has<TType>(Unwrap()))
						(void)std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Unwrap()));
				}
			}
			else
			{
				if constexpr (std::is_void_v<TType>)
					myRetVal.Emplace(std::forward<TCallable>(aCallable)());
				else if constexpr (std::is_same_v<TType, TSuccessType>)
					myRetVal.Emplace(std::forward<TCallable>(aCallable)(Unwrap()));
				else if constexpr (CResultGettableResult<TSuccessType, TType>)
				{
					if (ResultConversions::Has<TType>(Unwrap()))
						myRetVal.Emplace(std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Unwrap())));
				}
			}

			return { myResult, std::move(myRetVal) };
		}
		template<typename TType = TErrorType, typename TCallable>
		constexpr ResultWrapper<TRetType, TCheckedSuccessValues, std::conditional_t<std::is_same_v<TType, TErrorType>, AllValues, typename AddCheckedValue<TType, TCheckedErrorValues>::type>> OrElse(TCallable&& aCallable) &&
		{
			if (myResult.index() != IndexError)
				return { myResult, std::move(myRetVal) };

			static_assert(std::is_void_v<TType> || std::is_same_v<TType, TErrorType> || CResultGettableResult<TErrorType, TType>);

			if constexpr (std::is_void_v<TRetType>)
			{
				if constexpr (std::is_void_v<TType>)
					(void)std::forward<TCallable>(aCallable)();
				else if constexpr (std::is_same_v<TType, TErrorType>)
					(void)std::forward<TCallable>(aCallable)(Err());
				else if constexpr (CResultGettableResult<TErrorType, TType>)
				{
					if (ResultConversions::Has<TType>(Err()))
						(void)std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Err()));
				}
			}
			else
			{
				if constexpr (std::is_void_v<TType>)
					myRetVal.Emplace(std::forward<TCallable>(aCallable)());
				else if constexpr (std::is_same_v<TType, TErrorType>)
					myRetVal.Emplace(std::forward<TCallable>(aCallable)(Err()));
				else if constexpr (CResultGettableResult<TErrorType, TType>)
				{
					if (ResultConversions::Has<TType>(Err()))
						myRetVal.Emplace(std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Err())));
				}
			}

			return { myResult, std::move(myRetVal) };
		}

		inline constexpr [[nodiscard]] operator std::optional<TRetType>() && noexcept
			requires(!std::is_void_v<TRetType>)
		{
			return myRetVal.RetVal;
		}
		inline constexpr [[nodiscard]] operator TRetType() && noexcept
			requires(!std::is_void_v<TRetType>)
		{
			static_assert(CheckedAllValues<TSuccessType, TCheckedSuccessValues>::value && CheckedAllValues<TErrorType, TCheckedErrorValues>::value, "All result values must be checked to access return value.");

			return std::move(*myRetVal.RetVal);
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
		SRetValueWrapper<TRetType> myRetVal;

	};

	template<typename TType = TSuccessType, typename TCallable>
	constexpr auto AndThen(TCallable&& aCallable)
	{
		using RetType = std::decay_t<decltype([this, &aCallable]()
			{
				if constexpr (std::is_void_v<TType>)
					return std::forward<TCallable>(aCallable)();
				else if constexpr (std::is_same_v<TType, TSuccessType>)
					return std::forward<TCallable>(aCallable)(Unwrap());
				else if constexpr (CResultGettableResult<TSuccessType, TType>)
				{
					if (ResultConversions::Has<TType>(Unwrap()))
						return std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Unwrap()));
				}
			}())>;

		return ResultWrapper<RetType, std::tuple<>, std::tuple<>>(myResult)
			.AndThen<TType>(std::forward<TCallable>(aCallable));
	}
	template<typename TType = TErrorType, typename TCallable>
	constexpr auto OrElse(TCallable&& aCallable)
	{
		using RetType = std::decay_t<decltype([this, &aCallable]()
			{
				if constexpr (std::is_void_v<TType>)
					return std::forward<TCallable>(aCallable)();
				else if constexpr (std::is_same_v<TType, TErrorType>)
					return std::forward<TCallable>(aCallable)(Err());
				else if constexpr (CResultGettableResult<TErrorType, TType>)
				{
					if (ResultConversions::Has<TType>(Err()))
						return std::forward<TCallable>(aCallable)(ResultConversions::Get<TType>(Err()));
				}
			}())>;

		return ResultWrapper<RetType, std::tuple<>, std::tuple<>>(myResult)
			.OrElse<TType>(std::forward<TCallable>(aCallable));
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
