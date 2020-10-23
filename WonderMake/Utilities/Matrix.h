#pragma once

#include "Typedefs.h"

#include "Utilities/MathUtility.h"
#include "Utilities/Rotation.h"
#include "Utilities/Vector.h"

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
		constexpr ColumnAccessor(std::array<TAccessorRep*, TRows> aMemberArray) noexcept;

		[[nodiscard]] constexpr TAccessorRep& operator[] (const u32 aY) const noexcept;

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

	constexpr SMatrix() noexcept requires IsSquare
	{
		*this = Identity();
	};
	template<typename ...TArgs> requires std::conjunction_v<std::is_convertible<TArgs, TRep>...>
	constexpr SMatrix(TArgs... aArgs) noexcept;
	constexpr SMatrix(std::array<TRep, Size> aMemberArray) noexcept;

	[[nodiscard]] constexpr bool operator==(const SMatrix& aRhs) const noexcept;
	[[nodiscard]] constexpr bool operator!=(const SMatrix& aRhs) const noexcept;

	[[nodiscard]] constexpr ColumnAccessor<TRep> operator[](const u32 aX) noexcept;
	[[nodiscard]] constexpr ColumnAccessor<const TRep> operator[](const u32 aX) const noexcept;

	constexpr SMatrix& operator+=(const SMatrix& aRhs) noexcept;
	constexpr SMatrix& operator-=(const SMatrix& aRhs) noexcept;
	constexpr SMatrix& operator*=(const SMatrix& aRhs) noexcept;

	constexpr SMatrix& Transpose() noexcept
		requires IsSquare
	{
		return (*this = GetTransposed());
	}
	[[nodiscard]] constexpr SMatrix<TRep, TRows, TColumns> GetTransposed() const noexcept;

	constexpr SMatrix& FastInverse() noexcept
		requires IsValidTransform
	{
		return (*this = GetFastInverse());
	}
	[[nodiscard]] constexpr SMatrix GetFastInverse() const noexcept
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
	
	constexpr void SetPosition(const SVector<TRep, TRows - 1>& aPosition) noexcept
		requires IsValidTransform
	{
		for (auto i = 0; i < TRows - 1; ++i)
			(*this)[TColumns - 1][i] = aPosition[i];
	}
	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetPosition() const noexcept
		requires IsValidTransform
	{
		auto retVec = SVector<TRep, TRows - 1>();

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[TColumns - 1][i];

		return retVec;
	}

	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetForwardVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		constexpr auto x = HasOrientation<2> ? 1 : 2;
		SVector<TRep, TRows - 1> retVec;

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[x][i];

		return retVec;
	}
	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetBackwardVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		return -GetForwardVector();
	}
	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetRightVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		SVector<TRep, TRows - 1> retVec;

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[0][i];

		return retVec;
	}
	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetLeftVector() const noexcept
		requires (HasOrientation<2> || HasOrientation<3>)
	{
		return -GetRightVector();
	}
	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetUpwardVector() const noexcept
		requires HasOrientation<3>
	{
		SVector<TRep, TRows - 1> retVec;

		for (auto i = 0; i < TRows - 1; ++i)
			retVec[i] = (*this)[1][i];

		return retVec;
	}
	[[nodiscard]] constexpr SVector<TRep, TRows - 1> GetDownwardVector() const noexcept
		requires HasOrientation<3>
	{
		return -GetUpwardVector();
	}
	
	SMatrix& Rotate2D(const SRadianF32 aRotation) noexcept
		requires HasOrientation<2>
	{
		return Rotate(SMatrix<TRep, 2, 2>::CreateRotation2D(aRotation));
	}
	SMatrix& SetRotation2D(const SRadianF32 aRotation) noexcept
		requires HasOrientation<2>
	{
		const auto cos = static_cast<TRep>(MathUtility::Cos(aRotation.Rotation()));
		const auto sin = static_cast<TRep>(MathUtility::Sin(aRotation.Rotation()));

		(*this)[0][0] = cos;
		(*this)[0][1] = sin;
		(*this)[1][0] = -sin;
		(*this)[1][1] = cos;

		return *this;
	}
	
	constexpr SMatrix& Rotate(const SMatrix<TRep, TColumns - 1, TRows - 1>& aRotation) noexcept
		requires IsValidTransform
	{
		// TODO(Kevin): Calculate without making copies.

		auto orientation = GetOrientation();

		orientation *= aRotation;

		return SetOrientation(std::move(orientation));
	}
	[[nodiscard]] constexpr SMatrix<TRep, TColumns - 1, TRows - 1> GetOrientation() const noexcept
		requires IsValidTransform
	{
		SMatrix<TRep, TColumns - 1, TRows - 1> retMat;

		for (auto y = 0; y < TRows - 1; ++y)
			for (auto x = 0; x < TColumns - 1; ++x)
				retMat[x][y] = (*this)[x][y];

		return retMat;
	}
	constexpr SMatrix& SetOrientation(SMatrix<TRep, TColumns - 1, TRows - 1> aOrientation) const noexcept
		requires IsValidTransform
	{
		for (auto y = 0; y < TRows - 1; ++y)
			for (auto x = 0; x < TColumns - 1; ++x)
				(*this)[x][y] = std::move(aOrientation[x][y]);

		return *this;
	}

	[[nodiscard]] static SMatrix CreateRotation2D(const SRadianF32 aRotation) noexcept
		requires HasOrientation<2>
	{
		SMatrix retMat;

		retMat = SetRotation2D(aRotation);

		return retMat;
	}

	[[nodiscard]] static SMatrix CreateRotationX(const SRadianF32 aRotation) noexcept
		requires HasOrientation<3>
	{
		SMatrix retMat;

		const auto cos = static_cast<TRep>(MathUtility::Cos(aRotation.Rotation()));
		const auto sin = static_cast<TRep>(MathUtility::Sin(aRotation.Rotation()));

		retMat[1][1] = cos;
		retMat[1][2] = sin;
		retMat[2][1] = -sin;
		retMat[2][2] = cos;

		return retMat;
	}
	[[nodiscard]] static SMatrix CreateRotationY(const SRadianF32 aRotation) noexcept
		requires HasOrientation<3>
	{
		SMatrix retMat;

		const auto cos = static_cast<TRep>(MathUtility::Cos(aRotation.Rotation()));
		const auto sin = static_cast<TRep>(MathUtility::Sin(aRotation.Rotation()));

		retMat[0][0] = cos;
		retMat[0][2] = -sin;
		retMat[2][0] = sin;
		retMat[2][2] = cos;

		return retMat;
	}
	[[nodiscard]] static SMatrix CreateRotationZ(const SRadianF32 aRotation) noexcept
		requires HasOrientation<3>
	{
		SMatrix retMat;

		const auto cos = static_cast<TRep>(MathUtility::Cos(aRotation.Rotation()));
		const auto sin = static_cast<TRep>(MathUtility::Sin(aRotation.Rotation()));

		retMat[0][0] = cos;
		retMat[0][1] = sin;
		retMat[1][0] = -sin;
		retMat[1][1] = cos;

		return retMat;
	}

	[[nodiscard]] std::string ToString() const noexcept;

	[[nodiscard]] constexpr static SMatrix Zero() noexcept;
	[[nodiscard]] constexpr static SMatrix Identity() noexcept
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
SMatrix<TRep, TColumns, TRows> operator+(SMatrix<TRep, TColumns, TRows> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs);

template<typename TRep, u32 TColumns, u32 TRows>
SMatrix<TRep, TColumns, TRows> operator-(SMatrix<TRep, TColumns, TRows> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs);

template<typename TRep, u32 TSize, u32 TLhsHeight, u32 TRhsWidth>
[[nodiscard]] constexpr SMatrix<TRep, TRhsWidth, TLhsHeight> operator*(const SMatrix<TRep, TSize, TLhsHeight>& aLhs, const SMatrix<TRep, TRhsWidth, TSize>& aRhs) noexcept;

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SVector<TRep, TColumns> operator*(SVector<TRep, TColumns> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept;

template<typename TRep, u32 TColumns, u32 TRows>
[[nodiscard]] constexpr SVector<TRep, TColumns - 1> operator*(SVector<TRep, TColumns - 1> aLhs, const SMatrix<TRep, TColumns, TRows>& aRhs) noexcept;

#include "Matrix.inl"
