
template<typename TRep, u32 TSize>
template<typename ...TArgs, typename>
constexpr SVector<TRep, TSize>::SVector(TArgs... aArgs) noexcept
{
	u32 i = 0;

	(((*this)[i++] = static_cast<TRep>(aArgs)), ...);
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr SVector<TRep, TSize + 1> SVector<TRep, TSize>::Promote(const TRep LastValue) const noexcept
{
	SVector<TRep, TSize + 1> ReturnVal;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] = (*this)[u];
	}

	ReturnVal[TSize] = LastValue;

	return ReturnVal;
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr TRep& SVector<TRep, TSize>::operator[](const u32 Index) noexcept
{
	assert(Index < TSize);
	return this->MemberArray[Index];
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr const TRep& SVector<TRep, TSize>::operator[](const u32 Index) const noexcept
{
	assert(Index < TSize);
	return this->MemberArray[Index];
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr char SVector<TRep, TSize>::GetMemberName(const u32 Index) const noexcept
{
	switch (Index)
	{
	case 0:
		return 'X';
	case 1:
		return 'Y';
	case 2:
		return 'Z';
	case 3:
		return 'W';
	default:
		return '1' + char(Index);
	}
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr SVector<TRep, TSize> SVector<TRep, TSize>::operator-() const noexcept
{
	SVector<TRep, TSize> ReturnVal;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] = -((*this)[u]);
	}

	return ReturnVal;
}

template<typename TRep, u32 TSize>
constexpr SVector<TRep, TSize>& SVector<TRep, TSize>::operator*=(const TRep aMultiplier) noexcept
{
	*this = *this * aMultiplier;

	return *this;
}

template<typename TRep, u32 TSize>
constexpr SVector<TRep, TSize>& SVector<TRep, TSize>::operator/=(const TRep aDivisor) noexcept
{
	*this = *this / aDivisor;

	return *this;
}

template<typename TRep, u32 TSize>
constexpr SVector<TRep, TSize>& SVector<TRep, TSize>::operator+=(const SVector<TRep, TSize>& aRightVector) noexcept
{
	*this = *this + aRightVector;

	return *this;
}

template<typename TRep, u32 TSize>
constexpr SVector<TRep, TSize>& SVector<TRep, TSize>::operator-=(const SVector<TRep, TSize>& aRightVector) noexcept
{
	*this = *this - aRightVector;

	return *this;
}

template<typename TRep, u32 TSize>
constexpr bool SVector<TRep, TSize>::operator==(const SVector<TRep, TSize>& aRightVector) const noexcept
{
	for (u32 u = 0; u < TSize; u++)
	{
		if ((*this)[u] != aRightVector[u])
			return false;
	}

	return true;
}

template<typename TRep, u32 TSize>
constexpr bool SVector<TRep, TSize>::operator!=(const SVector<TRep, TSize>& aRightVector) const noexcept
{
	return !(*this == aRightVector);
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr TRep SVector<TRep, TSize>::LengthSquared() const noexcept
{
	TRep total = 0;

	for (u32 u = 0; u < TSize; u++)
	{
		total += (*this)[u] * (*this)[u];
	}

	return total;
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr TRep SVector<TRep, TSize>::Length() const noexcept
{
	return static_cast<TRep>(std::sqrt(LengthSquared()));
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr TRep SVector<TRep, TSize>::DistanceTo(const SVector<TRep, TSize> aOther) const noexcept
{
	return (aOther - *this).Length();
}

template<typename TRep, u32 TSize>
constexpr void SVector<TRep, TSize>::Normalize() noexcept
{
	const TRep length = Length();

	if (length == 0)
		return;

	for (u32 u = 0; u < TSize; u++)
	{
		(*this)[u] /= length;
	}
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr SVector<TRep, TSize> SVector<TRep, TSize>::GetNormalized() const noexcept
{
	SVector<TRep, TSize> vec = *this;

	vec.Normalize();

	return vec;
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr TRep SVector<TRep, TSize>::Dot(const SVector<TRep, TSize> aOther) const noexcept
{
	TRep sum = 0;
	for (u32 i = 0; i < TSize; i++)
	{
		sum += (*this)[i] * aOther[i];
	}

	return sum;
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr SVector<TRep, TSize> SVector<TRep, TSize>::Zero() noexcept
{
	SVector<TRep, TSize> ReturnVal;

	return ReturnVal;
}

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr SVector<TRep, TSize> SVector<TRep, TSize>::One() noexcept
{
	SVector<TRep, TSize> ReturnVal;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] = 1;
	}

	return ReturnVal;
}

template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator-(const SVector<TRep, TSize>& aLeftVector, const SVector<TRep, TSize>& aRightVector) noexcept
{
	SVector<TRep, TSize> ReturnVal = aLeftVector;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] -= aRightVector[u];
	}

	return ReturnVal;
}

template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator+(const SVector<TRep, TSize>& aLeftVector, const SVector<TRep, TSize>& aRightVector) noexcept
{
	SVector<TRep, TSize> ReturnVal = aLeftVector;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] += aRightVector[u];
	}

	return ReturnVal;
}

template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator*(const TRep aMultiplier, const SVector<TRep, TSize>& aLeftVector) noexcept
{
	return aLeftVector * aMultiplier;
}

template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator*(const SVector<TRep, TSize>& aLeftVector, const TRep aMultiplier) noexcept
{
	SVector<TRep, TSize> ReturnVal = aLeftVector;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] *= aMultiplier;
	}

	return ReturnVal;
}

template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator/(const SVector<TRep, TSize>& aLeftVector, const TRep aDivisor) noexcept
{
	SVector<TRep, TSize> ReturnVal = aLeftVector;

	for (u32 u = 0; u < TSize; u++)
	{
		ReturnVal[u] /= aDivisor;
	}

	return ReturnVal;
}
