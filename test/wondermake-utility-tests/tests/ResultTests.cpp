#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/Result.h"

template<typename TType>
struct ResultCallableMock
{
	MOCK_METHOD(void, Execute, (TType&));

	template<typename TArg>
	void operator()(TArg&& aArg)
	{
		Execute(std::forward<TArg>(aArg));
	}
};

template<>
struct ResultCallableMock<void>
{
	MOCK_METHOD(void, Execute, ());

	void operator()()
	{
		Execute();
	}
};

template<typename TType>
struct ResultNonCopyable
	: NonCopyable
{
	template<typename TArg>
	constexpr ResultNonCopyable(TArg&& aValue)
		noexcept(std::is_nothrow_constructible_v<TType, decltype(std::forward<TArg>(aValue))>)
		: Value(std::forward<TArg>(aValue))
	{}

	bool operator==(TType aValue) const noexcept
	{
		return Value == aValue;
	}

	TType Value;
};

inline constexpr auto NoOp = [](auto&&...) {};

void StaticTest_Result()
{
	enum class ETestA
	{
		Kalle
	};
	enum class ETestB
	{
		Hobbe
	};

	constexpr Result<void> resultOk = Ok();
	constexpr Result<void> resultErr = Err();

	static_assert(resultOk);
	static_assert(resultOk.IsOk());
	static_assert(!resultOk.IsErr());

	static_assert(!resultErr);
	static_assert(!resultErr.IsOk());
	static_assert(resultErr.IsErr());

	constexpr u32 expectedValue = 1234;

	constexpr Result<u32, u32> resultValOk	= Ok(expectedValue);
	constexpr Result<u32, u32> resultValErr	= Err(expectedValue);

	static_assert(resultValOk.Unwrap() == expectedValue);
	static_assert(resultValErr.Err() == expectedValue);

	constexpr auto nonConstTest = []()
	{
		Result<void> resultOk = Ok();
		Result<void> resultErr = Err();

		Result<u32> resultOkCopyable = Ok(1234);
		Result<void, u32> resultErrCopyable = Err(1234);

		Result<ResultNonCopyable<u32>> resultOkNonCopyable = Ok(1234);
		Result<void, ResultNonCopyable<u32>> resultErrNonCopyable = Err(1234);

		constexpr auto copyOp = [](auto& aValue)
		{
			auto v = aValue;

			v;
		};
		constexpr auto moveOp = [](auto& aValue)
		{
			auto v = std::move(aValue);

			v;
		};

		resultOk
			.AndThen(NoOp)
			.OrElse(NoOp);
		resultOk
			.OrElse(NoOp)
			.AndThen(NoOp);
		resultErr
			.AndThen(NoOp)
			.OrElse(NoOp);
		resultErr
			.OrElse(NoOp)
			.AndThen(NoOp);
		Result<void>(Ok())
			.AndThen(NoOp)
			.OrElse(NoOp);
		Result<void>(Ok())
			.OrElse(NoOp)
			.AndThen(NoOp);
		Result<void>(Err())
			.AndThen(NoOp)
			.OrElse(NoOp);
		Result<void>(Err())
			.OrElse(NoOp)
			.AndThen(NoOp);
		
		resultOkCopyable
			.AndThen(copyOp)
			.OrElse(NoOp);
		resultOkCopyable
			.OrElse(NoOp)
			.AndThen(copyOp);
		resultErrCopyable
			.AndThen(NoOp)
			.OrElse(copyOp);
		resultErrCopyable
			.OrElse(copyOp)
			.AndThen(NoOp);
		Result<u32>(Ok(1234))
			.AndThen(copyOp)
			.OrElse(NoOp);
		Result<u32>(Ok(1234))
			.OrElse(NoOp)
			.AndThen(copyOp);
		Result<void, u32>(Err(1234))
			.AndThen(NoOp)
			.OrElse(copyOp);
		Result<void, u32>(Err(1234))
			.OrElse(copyOp)
			.AndThen(NoOp);

		resultOkNonCopyable
			.AndThen(moveOp)
			.OrElse(NoOp);
		resultOkNonCopyable
			.OrElse(NoOp)
			.AndThen(moveOp);
		resultErrNonCopyable
			.AndThen(NoOp)
			.OrElse(moveOp);
		resultErrNonCopyable
			.OrElse(moveOp)
			.AndThen(NoOp);
		Result<ResultNonCopyable<u32>>(Ok(1234))
			.AndThen(moveOp)
			.OrElse(NoOp);
		Result<ResultNonCopyable<u32>>(Ok(1234))
			.OrElse(NoOp)
			.AndThen(moveOp);
		Result<void, ResultNonCopyable<u32>>(Err(1234))
			.AndThen(NoOp)
			.OrElse(moveOp);
		Result<void, ResultNonCopyable<u32>>(Err(1234))
			.OrElse(moveOp)
			.AndThen(NoOp);

		return true;
	};

	static_assert(nonConstTest());

	constexpr auto CompareOpt = [](std::optional<bool> aLhs, std::optional<bool> aRhs) -> bool
	{
		return aLhs == aRhs;
	};
	static constexpr auto OnSuccess	= [](auto&...) { return true; };
	static constexpr auto OnError	= [](auto&...) { return false; };

	static_assert(CompareOpt(Result<void>(Ok()).OrElse(OnError), std::nullopt));
	static_assert(CompareOpt(Result<void>(Ok()).OrElse(OnError), std::nullopt));
	static_assert(CompareOpt(Result<void>(Err()).AndThen(OnError), std::nullopt));
	static_assert(CompareOpt(Result<void>(Err()).AndThen(OnError), std::nullopt));
	static_assert(CompareOpt(Result<void>(Ok()).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(CompareOpt(Result<void>(Ok()).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(CompareOpt(Result<void>(Err()).AndThen(OnSuccess).OrElse(OnError), false));
	static_assert(CompareOpt(Result<void>(Err()).AndThen(OnSuccess).OrElse(OnError), false));

	static_assert(CompareOpt(Result<u32>(Err()).AndThen(OnSuccess), std::nullopt));
	static_assert(CompareOpt(Result<ResultNonCopyable<u32>>(Err()).AndThen(OnSuccess), std::nullopt));
	static_assert(CompareOpt(Result<u32>(Ok(1234)).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(CompareOpt(Result<ResultNonCopyable<u32>>(Ok(1234)).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(CompareOpt(Result<u32>(Err()).AndThen(OnSuccess).OrElse(OnError), false));
	static_assert(CompareOpt(Result<ResultNonCopyable<u32>>(Err()).AndThen(OnSuccess).OrElse(OnError), false));

	static_assert(CompareOpt(Result<void, u32>(Ok()).OrElse(OnError), std::nullopt));
	static_assert(CompareOpt(Result<void, ResultNonCopyable<u32>>(Ok()).OrElse(OnError), std::nullopt));
	static_assert(CompareOpt(Result<void, u32>(Ok()).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(CompareOpt(Result<void, ResultNonCopyable<u32>>(Ok()).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(CompareOpt(Result<void, u32>(Err(1234)).AndThen(OnSuccess).OrElse(OnError), false));
	static_assert(CompareOpt(Result<void, ResultNonCopyable<u32>>(Err(1234)).AndThen(OnSuccess).OrElse(OnError), false));

	constexpr auto Compare = [](bool aLhs, bool aRhs) -> bool
	{
		return aLhs == aRhs;
	};

	static_assert(Compare(Result<void>(Ok()).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(Compare(Result<void>(Ok()).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(Compare(Result<void>(Err()).AndThen(OnSuccess).OrElse(OnError), false));
	static_assert(Compare(Result<void>(Err()).AndThen(OnSuccess).OrElse(OnError), false));

	static_assert(Compare(Result<u32>(Ok(1234)).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(Compare(Result<ResultNonCopyable<u32>>(Ok(1234)).AndThen(OnSuccess).OrElse(OnError), true));
	static_assert(Compare(Result<u32>(Err()).AndThen(OnSuccess).OrElse(OnError), false));
	static_assert(Compare(Result<ResultNonCopyable<u32>>(Err()).AndThen(OnSuccess).OrElse(OnError), false));

	static_assert(u32(Result<void>(Ok())
		.AndThen([]() -> u32
			{
				return 1;
			})
		.OrElse([]() -> u32
			{
				return 2;
			})) == 1);
	static_assert(u32(Result<void>(Err())
		.AndThen([]() -> u32
			{
				return 1;
			})
		.OrElse([]() -> u32
			{
				return 2;
			})) == 2);
	static_assert(u32(Result<std::variant<ETestB, ETestA>>(Ok(ETestB::Hobbe))
		.AndThen<ETestA>([](auto) -> u32
			{
				return 1;
			})
		.AndThen<ETestB>([](auto) -> u32
			{
				return 2;
			})
		.OrElse([]() -> u32
			{
				return 3;
			})) == 2);
	static_assert(u32(Result<void, std::variant<ETestB, ETestA>>(Err(ETestB::Hobbe))
		.OrElse<ETestA>([](auto) -> u32
			{
				return 1;
			})
		.OrElse<ETestB>([](auto) -> u32
			{
				return 2;
			})
		.AndThen([]() -> u32
			{
				return 3;
			})) == 2);
}

TEST(ResultTests, successful_result_boolean_conversions_returns_correct_values)
{
	Result<void> result = Ok();

	EXPECT_TRUE(result);
	EXPECT_TRUE(result.IsOk());
	EXPECT_FALSE(result.IsErr());

	std::optional<bool> opt = Result<ResultNonCopyable<u32>>(Ok(1234))
		.AndThen([](auto&)
			{
				return true;
			})
		.OrElse([]()
			{
				return false;
			});
}

TEST(ResultTests, error_result_boolean_conversions_returns_correct_values)
{
	Result<void> result = Err();

	EXPECT_FALSE(result);
	EXPECT_FALSE(result.IsOk());
	EXPECT_TRUE(result.IsErr());
}

TEST(ResultTests, unwrap_returns_ok_value)
{
	constexpr auto expectedValue = "Hello World!";

	Result<std::string> result = Ok(expectedValue);

	EXPECT_EQ(result.Unwrap(), expectedValue);
}

TEST(ResultTests, unwrap_with_unique_value_returns_ok_value)
{
	constexpr auto expectedValue = "Hello World!";

	Result<ResultNonCopyable<std::string>> result = Ok(ResultNonCopyable<std::string>(expectedValue));

	EXPECT_EQ(result.Unwrap().Value, expectedValue);
}

TEST(ResultTests, unwrapor_returns_ok_value)
{
	constexpr auto expectedValue = "Hello World!";
	constexpr auto defaultValue = "Foo Bar";

	Result<std::string> result = Ok(expectedValue);

	EXPECT_EQ(result.UnwrapOr(defaultValue), expectedValue);
}

TEST(ResultTests, unwrapor_with_unique_value_returns_ok_value)
{
	constexpr auto expectedValue = "Hello World!";
	constexpr auto defaultValue = "Foo Bar";

	Result<ResultNonCopyable<std::string>> result = Ok(ResultNonCopyable<std::string>(expectedValue));

	EXPECT_EQ(std::move(result).UnwrapOr(ResultNonCopyable<std::string>(defaultValue)).Value, expectedValue);
}

TEST(ResultTests, unwrapor_returns_default_value_when_result_is_error)
{
	constexpr auto expectedValue = "Hello World!";
	constexpr auto defaultValue = "Foo Bar";

	Result<std::string> result = Err();

	EXPECT_EQ(result.UnwrapOr(defaultValue), defaultValue);
}

TEST(ResultTests, unwrapor_with_unique_value_returns_default_value_when_result_is_error)
{
	constexpr auto expectedValue = "Hello World!";
	constexpr auto defaultValue = "Foo Bar";

	Result<ResultNonCopyable<std::string>> result = Err();

	EXPECT_EQ(std::move(result).UnwrapOr(ResultNonCopyable<std::string>(defaultValue)).Value, defaultValue);
}

TEST(ResultTests, err_returns_err_value)
{
	constexpr auto expectedValue = "Hello World!";

	Result<void, std::string> result = Err(expectedValue);

	EXPECT_EQ(result.Err(), expectedValue);
}

TEST(ResultTests, err_with_unique_value_returns_err_value)
{
	constexpr auto expectedValue = "Hello World!";

	Result<void, ResultNonCopyable<std::string>> result = Err(ResultNonCopyable<std::string>(expectedValue));

	EXPECT_EQ(std::move(result).Err().Value, expectedValue);
}

TEST(ResultTests, successful_result_call_callable_in_andthen)
{
	StrictMock<ResultCallableMock<void>> mock;

	Result<void> result = Ok();

	EXPECT_CALL(mock, Execute());

	result.AndThen(mock);
}

TEST(ResultTests, unsuccessful_result_does_not_call_callable_in_andthen)
{
	StrictMock<ResultCallableMock<void>> mock;

	Result<void> result = Err();

	result.AndThen(mock);
}

TEST(ResultTests, andthen_passes_correct_value)
{
	constexpr auto expectedValue = "Hello World!";
	StrictMock<ResultCallableMock<std::string>> mock;

	Result<std::string> result = Ok(expectedValue);

	EXPECT_CALL(mock, Execute(Eq(expectedValue)));

	result.AndThen(mock);
}

TEST(ResultTests, andthen_passes_correct_unique_value)
{
	constexpr auto expectedValue = "Hello World!";
	StrictMock<ResultCallableMock<ResultNonCopyable<std::string>>> mock;

	Result<ResultNonCopyable<std::string>> result = Ok(expectedValue);

	EXPECT_CALL(mock, Execute(Eq(expectedValue)));

	std::move(result).AndThen(mock);
}

TEST(ResultTests, successful_result_does_not_call_callable_in_orelse)
{
	StrictMock<ResultCallableMock<void>> mock;

	Result<void> result = Ok();

	result.OrElse(mock);
}

TEST(ResultTests, unsuccessful_result_call_callable_in_orelse)
{
	StrictMock<ResultCallableMock<void>> mock;

	Result<void> result = Err();

	EXPECT_CALL(mock, Execute());

	result.OrElse(mock);
}

TEST(ResultTests, orelse_passes_correct_value)
{
	constexpr auto expectedValue = "Hello World!";
	StrictMock<ResultCallableMock<std::string>> mock;

	Result<void, std::string> result = Err(expectedValue);

	EXPECT_CALL(mock, Execute(Eq(expectedValue)));

	result.OrElse(mock);
}

TEST(ResultTests, orelse_passes_correct_unique_value)
{
	constexpr auto expectedValue = "Hello World!";
	StrictMock<ResultCallableMock<ResultNonCopyable<std::string>>> mock;

	Result<void, ResultNonCopyable<std::string>> result = Err(expectedValue);

	EXPECT_CALL(mock, Execute(Eq(expectedValue)));

	std::move(result).OrElse(mock);
}

TEST(ResultTests, andthen_called_when_orelse_is_ahead_in_chain_and_result_is_successful)
{
	StrictMock<ResultCallableMock<void>> mockAndThen;

	Result<void> result = Ok();

	EXPECT_CALL(mockAndThen, Execute());

	result
		.OrElse(NoOp)
		.AndThen(mockAndThen);
}

TEST(ResultTests, andthen_not_called_when_orelse_is_ahead_in_chain_and_result_is_error)
{
	StrictMock<ResultCallableMock<void>> mockAndThen;

	Result<void> result = Err();

	result
		.OrElse(NoOp)
		.AndThen(mockAndThen);
}

TEST(ResultTests, orelse_called_when_andthen_is_ahead_in_chain_and_result_is_error)
{
	StrictMock<ResultCallableMock<void>> mockOrElse;

	Result<void> result = Err();

	EXPECT_CALL(mockOrElse, Execute());

	result
		.AndThen(NoOp)
		.OrElse(mockOrElse);
}

TEST(ResultTests, orelse_not_called_when_andthen_is_ahead_in_chain_and_result_is_successful)
{
	StrictMock<ResultCallableMock<void>> mockOrElse;

	Result<void> result = Ok();

	result
		.AndThen(NoOp)
		.OrElse(mockOrElse);
}

struct ConvertedString
{
	ConvertedString(std::string aValue)
		: Value(std::move(aValue))
	{}

	bool operator==(std::string aValue) const
	{
		return Value == aValue;
	}

	std::string Value;
};

TEST(ResultTests, converted_result_unwrap_returns_correct_successful_value)
{
	constexpr auto expectedValue = "Hello World!";

	const Result<std::string> resultOriginal = Ok(expectedValue);

	Result<ConvertedString> resultConverted = resultOriginal;

	EXPECT_TRUE(resultConverted);

	EXPECT_EQ(resultConverted.Unwrap(), expectedValue);
}

TEST(ResultTests, converted_result_unwrap_returns_correct_error_value)
{
	constexpr auto expectedValue = "Hello World!";

	const Result<void, std::string> resultOriginal = Err(expectedValue);

	Result<void, ConvertedString> resultConverted = resultOriginal;

	EXPECT_FALSE(resultConverted);

	EXPECT_EQ(resultConverted.Err(), expectedValue);
}

TEST(ResultTests, moved_converted_result_unwrap_returns_correct_successful_value)
{
	constexpr auto expectedValue = "Hello World!";

	Result<std::string> resultOriginal = Ok(expectedValue);

	Result<ConvertedString> resultConverted = std::move(resultOriginal);

	EXPECT_TRUE(resultConverted);

	EXPECT_EQ(resultConverted.Unwrap(), expectedValue);
}

TEST(ResultTests, moved_converted_result_unwrap_returns_correct_error_value)
{
	constexpr auto expectedValue = "Hello World!";

	Result<void, std::string> resultOriginal = Err(expectedValue);

	Result<void, ConvertedString> resultConverted = std::move(resultOriginal);

	EXPECT_FALSE(resultConverted);

	EXPECT_EQ(resultConverted.Err(), expectedValue);
}
