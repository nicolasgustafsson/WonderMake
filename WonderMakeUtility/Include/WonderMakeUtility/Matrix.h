#pragma once

#include "Math.h"
#include "Rotation.h"
#include "Typedefs.h"
#include "Vector.h"

#include <algorithm>
#include <array>
#include <string>

template<typename TRep, u32 TColumns, u32 TRows>
struct SMatrix final
{
	template<typename TAccessorRep>
	class ColumnAccessor
	{
	public:
		inline constexpr ColumnAccessor(std::array<TAccessorRep*, TRows> aMemberArray) noexcept
			: myMemberArray(std::move(aMemberArray))
		{}

		inline [[nodiscard]] constexpr TAccessorRep& operator[] (const u32 aY) const noexcept
		{
			return *myMemberArray[aY];
		}

	private:
		std::array<TAccessorRep*, TRows> myMemberArray;
	};

	using Representation = TRep;
	
	constexpr static auto Columns = TColumns;
	constexpr static auto Rows = TRows;

	constexpr static auto Size = TColumns * TRows;
	constexpr static auto IsSquare = TColumns == TRows;
	constexpr static auto IsValidTransform = IsSquare || (TColumns >= 2 && TRows >= 2);
	
	template<u32 TDimensions>
	constexpr static auto HasOrientation =
		(TColumns == TDimensions && TRows == TDimensions)
		|| (TColumns == TDimensions + 1 && TRows == TDimensions + 1);

	std::array<TRep, Size> MemberArray;

	inline constexpr SMatrix() noexcept requires IsSquare
	{
		*this = Identity();
	};
	template<typename ...TArgs>
	inline constexpr SMatrix(TArgs... aArgs) noexcept
		requires std::conjunction_v<std::is_convertible<TArgs, TRep>...>
	{
		u32 i = 0;

		((MemberArray[i++] = static_cast<TRep>(aArgs)), ...);
	}
	inline constexpr SMatrix(std::array<TRep, Size> aMemberArray) noexcept
		: MemberArray(std::move(aMemberArray))
	{}

	inline [[nodiscard]] constexpr bool operator==(const SMatrix& aRhs) const noexcept
	{
		return std::equal(MemberArray.begin(), MemberArray.end(), aRhs.MemberArray.begin());
	}
	inline [[nodiscard]] constexpr bool operator!=(const SMatrix& aRhs) const noexcept
	{
		return !(*this == aRhs);
	}

	inline [[nodiscard]] constexpr ColumnAccessor<TRep> operator[](const u32 aX) noexcept
	{
		std::array<TRep*, TRows> members = {};

		for (auto y = 0; y < TRows; ++y)
			members[y] = &MemberArray[static_cast<size_t>(y * TColumns + aX)];

		return ColumnAccessor<TRep>(members);
	}
	inline [[nodiscard]] constexpr ColumnAccessor<const TRep> operator[](const u32 aX) const noexcept
	{
		std::array<const TRep*, TRows> members = {};

		for (auto y = 0; y < TRows; ++y)
			members[y] = &MemberArray[static_cast<size_t>(y * TColumns + aX)];

		return ColumnAccessor<const TRep>(members);
	}

	inline constexpr SMatrix& operator+=(const SMatrix& aRhs) noexcept
	{
		for (auto i = 0; i < Size; ++i)
			MemberArray[i] += aRhs.MemberArray[i];

		return *this;
	}
	inline constexpr SMatrix& operator-=(const SMatrix& aRhs) noexcept
	{
		for (auto i = 0; i < Size; ++i)
			MemberArray[i] -= aRhs.MemberArray[i];

		return *this;
	}
	inline constexpr SMatrix& operator*=(const SMatrix& aRhs) noexcept
	{
		return (*this = *this * aRhs);
	}

	inline constexpr SMatrix& Transpose() noexcept
		requires IsSquare
	{
		return (*this = GetTransposed());
	}
	inline [[nodiscard]] constexpr SMatrix<TRep, TRows, TColumns> GetTransposed() const noexcept
	{
		auto retMat = SMatrix<TRep, TRows, TColumns>::Zero();

		for (auto y = 0; y < TRows; ++y)
			for (auto x = 0; x < TColumns; ++x)
				retMat[y][x] = (*this)[x][y];

		return retMat;
	}

	inline constexpr SMatrix& FastInverse() noexcept
		requires IsValidTransform
	{
		return (*this = GetFastInverse());
	}
	inline [[nodiscard]] constexpr SMatrix GetFastInverse() const noexcept
		requires IsValidTransform
	{
		constexpr auto lastIndex = TRows - 1;

		auto retMat = *this;
		auto position = SMatrix<TRep, 1, TRows>::Zero();

		for (auto i = 0; i < lastIndex; ++i)
			position[0][i] = -retMat[lastIndex][i];
		position[lastIndex][0] = 1;

		for (auto i = 0; i < TRows; ++i)
			retMat[lastIndex][i] = {};

		retMat.Transpose();

		position = retMat * position;

		for (auto i = 0; i < lastIndex; ++i)
			retMat[lastIndex][i] = position[0][i];
		retMat[lastIndex][lastIndex] = 1;

		return retMat;
	}
	
	inline constexpr void SetPosition(const SVector<TRep, TRows - 1>& aPosition) noexcept
		requires IsValidTransform
	{
		for (auto i = 0; i < TRows - 1; ++i)
			(*this)[TColumns - 1][i] = aPosition[i];
	}
	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetPosition() const noexcept
		requires IsValidTransform
	{
		auto retVec = SVector<TRep, TRows - 1>();

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[TColumns - 1][i];

		return retVec;
	}

	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetForwardVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		constexpr auto x = HasOrientation<2> ? 1 : 2;
		SVector<TRep, TRows - 1> retVec;

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[x][i];

		return retVec;
	}
	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetBackwardVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		return -GetForwardVector();
	}
	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetRightVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		SVector<TRep, TRows - 1> retVec;

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[0][i];

		return retVec;
	}
	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetLeftVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		return -GetRightVector();
	}
	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetUpwardVector() const noexcept
		requires HasOrientation<3>
	{
		SVector<TRep, TRows - 1> retVec;

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[1][i];

		return retVec;
	}
	inline [[nodiscard]] constexpr SVector<TRep, TRows - 1> GetDownwardVector() const noexcept
		requires HasOrientation<3>
	{
		return -GetUpwardVector();
	}

	template<typename TRotation>
	inline SMatrix& Rotate2D(const TRotation aRotation) noexcept
		requires (HasOrientation<2> && std::is_floating_point_v<typename TRotation::Representation>)
	{
		return Rotate(SMatrix<TRep, 2, 2>::CreateRotation2D(aRotation));
	}
	template<typename TRotation>
	inline SMatrix& SetRotation2D(const TRotation aRotation) noexcept
		requires (HasOrientation<2>&& std::is_floating_point_v<typename TRotation::Representation>)
	{
		const auto cos = static_cast<TRep>(WmMath::Cos(aRotation));
		const auto sin = static_cast<TRep>(WmMath::Sin(aRotation));

		(*this)[0][0] = cos;
		(*this)[0][1] = sin;
		(*this)[1][0] = -sin;
		(*this)[1][1] = cos;

		return *this;
	}
	
	inline constexpr SMatrix& Rotate(const SMatrix<TRep, TColumns - 1, TRows - 1>& aRotation) noexcept
		requires IsValidTransform
	{
		// TODO(Kevin): Calculate without making copies.

		auto orientation = GetOrientation();

		orientation *= aRotation;

		return SetOrientation(std::move(orientation));
	}
	inline [[nodiscard]] constexpr SMatrix<TRep, TColumns - 1, TRows - 1> GetOrientation() const noexcept
		requires IsValidTransform
	{
		SMatrix<TRep, TColumns - 1, TRows - 1> retMat;

		for (auto y = 0; y < TRows - 1; ++y)
			for (auto x = 0; x < TColumns - 1; ++x)
				retMat[x][y] = (*this)[x][y];

		return retMat;
	}
	inline constexpr SMatrix& SetOrientation(SMatrix<TRep, TColumns - 1, TRows - 1> aOrientation) const noexcept
		requires IsValidTransform
	{
		for (auto y = 0; y < TRows - 1; ++y)
			for (auto x = 0; x < TColumns - 1; ++x)
				(*this)[x][y] = std::move(aOrientation[x][y]);

		return *this;
	}

	template<typename TRotation>
	inline [[nodiscard]] static SMatrix CreateRotation2D(const TRotation aRotation) noexcept
		requires (HasOrientation<2> && std::is_floating_point_v<typename TRotation::Representation>)
	{
		return SetRotation2D(aRotation);
	}

	template<typename TRotation>
	inline [[nodiscard]] static SMatrix CreateRotationX(const TRotation aRotation) noexcept
		requires (HasOrientation<3> && std::is_floating_point_v<typename TRotation::Representation>)
	{
		SMatrix retMat;

		const auto cos = static_cast<TRep>(WmMath::Cos(aRotation));
		const auto sin = static_cast<TRep>(WmMath::Sin(aRotation));

		retMat[1][1] = cos;
		retMat[1][2] = sin;
		retMat[2][1] = -sin;
		retMat[2][2] = cos;

		return retMat;
	}
	template<typename TRotation>
	inline [[nodiscard]] static SMatrix CreateRotationY(const TRotation aRotation) noexcept
		requires (HasOrientation<3> && std::is_floating_point_v<typename TRotation::Representation>)
	{
		SMatrix retMat;

		const auto cos = static_cast<TRep>(WmMath::Cos(aRotation));
		const auto sin = static_cast<TRep>(WmMath::Sin(aRotation));

		retMat[0][0] = cos;
		retMat[0][2] = -sin;
		retMat[2][0] = sin;
		retMat[2][2] = cos;

		return retMat;
	}
	template<typename TRotation>
	inline [[nodiscard]] static SMatrix CreateRotationZ(const TRotation aRotation) noexcept
		requires (HasOrientation<3> && std::is_floating_point_v<typename TRotation::Representation>)
	{
		SMatrix retMat;

		const auto cos = static_cast<TRep>(WmMath::Cos(aRotation));
		const auto sin = static_cast<TRep>(WmMath::Sin(aRotation));

		retMat[0][0] = cos;
		retMat[0][1] = sin;
		retMat[1][0] = -sin;
		retMat[1][1] = cos;

		return retMat;
	}

	inline [[nodiscard]] std::string ToString() const
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

	inline [[nodiscard]] constexpr static SMatrix Zero() noexcept
	{
		return SMatrix<TRep, TColumns, TRows>(std::array<TRep, Size>());
	}
	inline [[nodiscard]] constexpr static SMatrix Identity() noexcept
		requires IsSquare
	{
		auto retMat = Zero();

		for (auto i = 0; i < TColumns; ++i)
			retMat[i][i] = 1;

		return retMat;
	}
};

template <typename TRep>
using SMatrix33 = SMatrix<TRep, 3, 3>;
template <typename TRep>
using SMatrix44 = SMatrix<TRep, 4, 4>;

using SMatrix33f = SMatrix33<f32>;
using SMatrix44f = SMatrix44<f32>;

template<typename TRep, u32 TRows, u32 TColumns>
inline [[nodiscard]] SMatrix<TRep, TColumns, TRows> operator+(SMatrix<TRep, TColumns, TRows> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept
{
	return aLhs += aRhs;
}

template<typename TRep, u32 TColumns, u32 TRows>
inline [[nodiscard]] SMatrix<TRep, TColumns, TRows> operator-(SMatrix<TRep, TColumns, TRows> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept
{
	return aLhs -= aRhs;
}

template<typename TRep, u32 TSize, u32 TLhsHeight, u32 TRhsWidth>
inline [[nodiscard]] constexpr SMatrix<TRep, TRhsWidth, TLhsHeight> operator*(const SMatrix<TRep, TSize, TLhsHeight>& aLhs, const SMatrix<TRep, TRhsWidth, TSize>& aRhs) noexcept
{
	auto retMat = SMatrix<TRep, TRhsWidth, TLhsHeight>::Zero();

	for (auto y = 0; y < TLhsHeight; ++y)
		for (auto x = 0; x < TRhsWidth; ++x)
			for (auto i = 0; i < TSize; ++i)
				retMat[x][y] += aLhs[i][y] * aRhs[x][i];

	return retMat;
}

template<typename TRep, u32 TColumns, u32 TRows>
inline [[nodiscard]] constexpr SVector<TRep, TColumns> operator*(SVector<TRep, TColumns> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept
{
	auto retMat = SMatrix<TRep, 1, TColumns>::Zero();

	for (auto i = 0; i < TColumns; ++i)
		retMat[0][i] = aLhs[i];

	retMat = aRhs * retMat;

	for (auto i = 0; i < TColumns; ++i)
		aLhs[i] = retMat[0][i];

	return aLhs;
}

template<typename TRep, u32 TColumns, u32 TRows>
inline [[nodiscard]] constexpr SVector<TRep, TColumns - 1> operator*(SVector<TRep, TColumns - 1> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept
{
	return (aLhs.Promote(1) * aRhs).Demote();
}
