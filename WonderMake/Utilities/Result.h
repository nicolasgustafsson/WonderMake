#pragma once
#include <optional>

enum class EGenericError
{
	Failed
};

template <typename ErrorType = EGenericError>
class TResult
{
public:
	constexpr operator bool() { return IsSuccessful(); }

	constexpr bool IsSuccessful()
	{
		return !Error;
	};
	
	std::optional<ErrorType> Error;
};
