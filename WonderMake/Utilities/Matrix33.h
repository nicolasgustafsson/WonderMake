#pragma once

#include <math.h>

#include "Vector.h"

template<typename TYPE>
class Matrix44;

template<typename TYPE>
class SMatrix33
{
public:

	static const SMatrix33 Identity;
	static const SMatrix33 Zero;

	static const unsigned int NumRows = 3;
	static const unsigned int NumColumns = 3;
	static const unsigned int NumElements = NumRows * NumColumns;

#pragma warning(push)
#pragma warning(disable:4201)
	union
	{
		struct
		{
			TYPE m11, m12, m13;
			TYPE m21, m22, m23;
			TYPE m31, m32, m33;
		};

		TYPE myMatrix[NumElements];
	};
#pragma warning(pop)

	SMatrix33();
	SMatrix33(const SMatrix33 & aSource);
	SMatrix33(const Matrix44<TYPE> & aSource);
	template<typename U>
	SMatrix33(const SMatrix33<U> & aSource);
	SMatrix33(const TYPE(&anArray)[NumElements]);
	SMatrix33(
		const TYPE a11, const TYPE a12, const TYPE a13,
		const TYPE a21, const TYPE a22, const TYPE a23,
		const TYPE a31, const TYPE a32, const TYPE a33) noexcept;
	SMatrix33(const SVector2<TYPE> & aPosition);

	SMatrix33& operator +=(const SMatrix33& aRight);
	SMatrix33& operator -=(const SMatrix33& aRight);
	SMatrix33& operator *=(const SMatrix33& aRight);

	SMatrix33& operator =(const SMatrix33 & aSource) noexcept;

	void SetPosition(const SVector2<TYPE> & aPosition) noexcept;
	SVector2<TYPE> GetPosition() const;

	SMatrix33& Rotate2D(const TYPE anAngle);
	SMatrix33& Set2DRotation(const TYPE anAngle);

	SMatrix33& Transpose();
	static SMatrix33 Transpose(SMatrix33 aMatrix);

	SMatrix33& Inverse();
	static SMatrix33 Inverse(SMatrix33 aMatrix);

	static SMatrix33 CreateRotateAroundX(const TYPE anAngle);
	static SMatrix33 CreateRotateAroundY(const TYPE anAngle);
	static SMatrix33 CreateRotateAroundZ(const TYPE anAngle);

	std::string ToString() const;
};

template<typename TYPE>
std::string SMatrix33<TYPE>::ToString()  const
{
	std::string String;
	for (u32 y = 0; y < NumRows; y++)
	{
		String += "[";
		for (u32 x = 0; x < NumColumns; x++)
		{
			String += std::to_string(myMatrix[y * NumRows + x]);
			String += ",";
		}
		String += "]\n";
	}

	return String;
}

using SMatrix33f = SMatrix33<float>;

template<typename TYPE>
SMatrix33<TYPE> operator +(SMatrix33<TYPE> aLeft, const SMatrix33<TYPE> & aRight)
{
	return aLeft += aRight;
}
template<typename TYPE>
SMatrix33<TYPE> operator -(SMatrix33<TYPE> aLeft, const SMatrix33<TYPE> & aRight)
{
	return aLeft -= aRight;
}

template<typename TYPE>
SMatrix33<TYPE> operator *(const SMatrix33<TYPE> & aLeft, const SMatrix33<TYPE> & aRight)
{

	SMatrix33<TYPE> matrix = SMatrix33<TYPE>::Zero;

	for (unsigned char i = 0; i < SMatrix33<TYPE>::NumElements; ++i)
	{
		for (unsigned char j = 0; j < SMatrix33<TYPE>::NumRows; ++j)
		{
			matrix.myMatrix[i] += aLeft.myMatrix[i / SMatrix33<TYPE>::NumColumns * SMatrix33<TYPE>::NumColumns + j] * aRight.myMatrix[i % SMatrix33<TYPE>::NumColumns + j * SMatrix33<TYPE>::NumColumns];
		}
	}

	return matrix;

}
template<typename TYPE>
SVector3<TYPE> operator *(SVector3<TYPE> aLeft, const SMatrix33<TYPE> & aRight)
{
	return SVector3<TYPE>{
		aLeft.X * aRight.m11 + aLeft.Y * aRight.m21 + aLeft.Z * aRight.m31,
		aLeft.X * aRight.m12 + aLeft.Y * aRight.m22 + aLeft.Z * aRight.m32,
		aLeft.X * aRight.m13 + aLeft.Y * aRight.m23 + aLeft.Z * aRight.m33
	};
}
template<typename TYPE>
SVector2<TYPE> operator *(SVector2<TYPE> aLeft, const SMatrix33<TYPE> & aRight)
{
	return (aLeft.Promote(1) * aRight).Demote();
}

template<typename TYPE>
bool operator ==(const SMatrix33<TYPE> &aLeft, const SMatrix33<TYPE> & aRight)
{
	for (unsigned char i = 0; i < SMatrix33<TYPE>::NumElements; ++i)
	{
		if (aLeft.myMatrix[i] != aRight.myMatrix[i])
		{
			return false;
		}
	}
	return true;
}
template<typename TYPE>
bool operator !=(const SMatrix33<TYPE> & aLeft, const SMatrix33<TYPE> & aRight)
{
	return !(aLeft == aRight);
}

#include "Matrix33.inl"
