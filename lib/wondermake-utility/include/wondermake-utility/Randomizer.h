#pragma once

#include "wondermake-utility/Typedefs.h"

#include <magic_enum.hpp>

#include <random>

class Randomizer
{
public:
	inline Randomizer()
		: myRandomGenerator(std::random_device()())
	{}
	inline Randomizer(std::seed_seq aSeedSeq)
		: myRandomGenerator(aSeedSeq)
	{}

	template <typename T = f32>
	[[nodiscard]] inline T GetRandomNumber(T aMin = 0, T aMax = 1)
	{
		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> distribution(aMin, aMax);

			return distribution(myRandomGenerator);
		}
		else
		{
			std::uniform_real_distribution<T> distribution(aMin, aMax);

			return distribution(myRandomGenerator);
		}
	}
	[[nodiscard]] inline bool GetRandomBool()
	{
		return GetRandomNumber(0, 1) == 1;
	}
	template<typename TEnum>
	[[nodiscard]] inline TEnum SelectEnum()
		requires(std::is_enum_v<TEnum>)
	{
		auto values = magic_enum::enum_values();

		return values[GetRandomNumber<size_t>(0, values.size() - 1)];
	}
	template<typename TContainer>
	[[nodiscard]] inline const auto& SelectElement(const TContainer& aContainer)
	{
		const size_t selectedIndex = GetRandomNumber<size_t>(0, aContainer.size() - 1);
		auto it = aContainer.cbegin();

		for (size_t i = 0; i < selectedIndex; ++i)
			++it;

		return *it;
	}
	template<typename TContainer>
	[[nodiscard]] inline auto& SelectElement(TContainer& aContainer)
	{
		const size_t selectedIndex = GetRandomNumber<size_t>(0, aContainer.size() - 1);
		auto it = aContainer.begin();

		for (size_t i = 0; i < selectedIndex; ++i)
			++it;

		return *it;
	}

	inline std::ostream& StreamOut(std::ostream& aStream) const
	{
		return (aStream << myRandomGenerator);
	}
	inline std::istream& StreamIn(std::istream& aStream)
	{
		return (aStream >> myRandomGenerator);
	}

private:
	std::mt19937 myRandomGenerator;

};

inline std::ostream& operator<<(std::ostream& aStream, const Randomizer& aRnd)
{
	return aRnd.StreamOut(aStream);
}
inline std::istream& operator>>(std::istream& aStream, Randomizer& aRnd)
{
	return aRnd.StreamIn(aStream);
}
