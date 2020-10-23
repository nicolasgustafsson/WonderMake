
#include "Matrix.h"

template<typename TRep, u32 TColumns, u32 TRows>
template<typename TAccessorRep>
constexpr SMatrix<TRep, TColumns, TRows>::ColumnAccessor<TAccessorRep>::ColumnAccessor(std::array<TAccessorRep*, TRows> aMemberArray) noexcept
	: myMemberArray(std::move(aMemberArray))
{}

template<typename TRep, u32 TColumns, u32 TRows>
template<typename TAccessorRep>
[[nodiscard]] constexpr TAccessorRep& SMatrix<TRep, TColumns, TRows>::ColumnAccessor<TAccessorRep>::operator[] (const u32 aY) const noexcept
{
	return *myMemberArray[aY];
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SMatrix<TRep, TColumns, TRows>::ColumnAccessor<TRep> SMatrix<TRep, TColumns, TRows>::operator[](const u32 aX) noexcept
{
	std::array<TRep*, TRows> members = {};

	for (auto y = 0; y < TRows; ++y)
		members[y] = &MemberArray[static_cast<size_t>(y * TColumns + aX)];

	return ColumnAccessor<TRep>(members);
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SMatrix<TRep, TColumns, TRows>::ColumnAccessor<const TRep> SMatrix<TRep, TColumns, TRows>::operator[](const u32 aX) const noexcept
{
	std::array<const TRep*, TRows> members = {};

	for (auto y = 0; y < TRows; ++y)
		members[y] = &MemberArray[static_cast<size_t>(y * TColumns + aX)];

	return ColumnAccessor<const TRep>(members);
}

template<typename TRep, u32 TColumns, u32 TRows>
template<typename ...TArgs> requires std::conjunction_v<std::is_convertible<TArgs, TRep>...>
constexpr SMatrix<TRep, TColumns, TRows>::SMatrix(TArgs... aArgs) noexcept
{
	u32 i = 0;
		
	((MemberArray[i++] = static_cast<TRep>(aArgs)), ...);
}

template<typename TRep, u32 TColumns, u32 TRows>
constexpr SMatrix<TRep, TColumns, TRows>::SMatrix(std::array<TRep, Size> aMemberArray) noexcept
	: MemberArray(std::move(aMemberArray))
{}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr bool SMatrix<TRep, TColumns, TRows>::operator==(const SMatrix& aRhs) const noexcept
{
	return std::equal(MemberArray.begin(), MemberArray.end(), aRhs.MemberArray.begin());
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr bool SMatrix<TRep, TColumns, TRows>::operator!=(const SMatrix& aRhs) const noexcept
{
	return !(*this == aRhs);
}

template<typename TRep, u32 TColumns, u32 TRows>
constexpr SMatrix<TRep, TColumns, TRows>& SMatrix<TRep, TColumns, TRows>::operator+=(const SMatrix& aRhs) noexcept
{
	for (auto i = 0; i < Size; ++i)
		MemberArray[i] += aRhs.MemberArray[i];

	return *this;
}

template<typename TRep, u32 TColumns, u32 TRows>
constexpr SMatrix<TRep, TColumns, TRows>& SMatrix<TRep, TColumns, TRows>::operator-=(const SMatrix& aRhs) noexcept
{
	for (auto i = 0; i < Size; ++i)
		MemberArray[i] -= aRhs.MemberArray[i];

	return *this;
}

template<typename TRep, u32 TColumns, u32 TRows>
constexpr SMatrix<TRep, TColumns, TRows>& SMatrix<TRep, TColumns, TRows>::operator*=(const SMatrix& aRhs) noexcept
{
	return (*this = *this * aRhs);
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SMatrix<TRep, TRows, TColumns> SMatrix<TRep, TColumns, TRows>::GetTransposed() const noexcept
{
	auto retMat = SMatrix<TRep, TRows, TColumns>::Zero();

	for (auto y = 0; y < TRows; ++y)
		for (auto x = 0; x < TColumns; ++x)
			retMat[y][x] = (*this)[x][y];

	return retMat;
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] std::string SMatrix<TRep, TColumns, TRows>::ToString() const noexcept
{
	std::string result;
	
	for (auto y = 0; y < TRows; ++y)
	{
		result += '[';

		for (auto x = 0; x < TColumns; ++x)
			result += std::to_string((*this)[x][y]) + ',';

		result.pop_back();

		result += "]\n";
	}

	return result;
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SMatrix<TRep, TColumns, TRows> SMatrix<TRep, TColumns, TRows>::Zero() noexcept
{
	return SMatrix<TRep, TColumns, TRows>(std::array<TRep, Size>());
}

template<typename TRep, u32 TColumns, u32 TRows>
SMatrix<TRep, TColumns, TRows> operator+(SMatrix<TRep, TColumns, TRows> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs)
{
	return aLhs += aRhs;
}

template<typename TRep, u32 TRows, u32 TColumns>
SMatrix<TRep, TColumns, TRows> operator-(SMatrix<TRep, TColumns, TRows> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs)
{
	return aLhs -= aRhs;
}

template<typename TRep, u32 TSize, u32 TLhsHeight, u32 TRhsWidth>
[[nodiscard]] constexpr SMatrix<TRep, TRhsWidth, TLhsHeight> operator*(const SMatrix<TRep, TSize, TLhsHeight>& aLhs, const SMatrix<TRep, TRhsWidth, TSize>& aRhs) noexcept
{
	auto retMat = SMatrix<TRep, TRhsWidth, TLhsHeight>::Zero();

	for (auto y = 0; y < TLhsHeight; ++y)
		for (auto x = 0; x < TRhsWidth; ++x)
			for (auto i = 0; i < TSize; ++i)
				retMat[x][y] += aLhs[i][y] * aRhs[x][i];

	return retMat;
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SVector<TRep, TColumns> operator*(SVector<TRep, TColumns> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept
{
	auto mat = SMatrix<TRep, 1, TColumns>::Zero();

	for (auto i = 0; i < TColumns; ++i)
		mat[0][i] = aLhs[i];

	mat = aRhs * mat;

	for (auto i = 0; i < TColumns; ++i)
		aLhs[i] = mat[0][i];

	return aLhs;
}

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SVector<TRep, TColumns - 1> operator*(SVector<TRep, TColumns - 1> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept
{
	return (aLhs.Promote(1) * aRhs).Demote();
}
