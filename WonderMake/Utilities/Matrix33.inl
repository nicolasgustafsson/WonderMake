
template<typename TYPE> const SMatrix33<TYPE> SMatrix33<TYPE>::Identity = SMatrix33(1, 0, 0, 0, 1, 0, 0, 0, 1);
template<typename TYPE> const SMatrix33<TYPE> SMatrix33<TYPE>::Zero = SMatrix33(0, 0, 0, 0, 0, 0, 0, 0, 0);

template<typename TYPE>
SMatrix33<TYPE>::SMatrix33()
{
	*this = Identity;
}
template<typename TYPE>
SMatrix33<TYPE>::SMatrix33(const SMatrix33 & aSource)
{
	*this = aSource;
}
template<typename TYPE>
SMatrix33<TYPE>::SMatrix33(const Matrix44<TYPE> & aSource)
{
	m11 = aSource.m11; m12 = aSource.m12; m13 = aSource.m13;
	m21 = aSource.m21; m22 = aSource.m22; m23 = aSource.m23;
	m31 = aSource.m31; m32 = aSource.m32; m33 = aSource.m33;
}
template<typename TYPE>
template<typename U>
SMatrix33<TYPE>::SMatrix33(const SMatrix33<U> & aSource)
{
	for (unsigned char i = 0; i < NumElements; ++i)
	{
		myMatrix[i] = static_cast<TYPE>(aSource.myMatrix[i]);
	}
}
template<typename TYPE>
SMatrix33<TYPE>::SMatrix33(const TYPE(&anArray)[NumElements])
{
	for (unsigned char i = 0; i < NumElements; ++i)
	{
		myMatrix[i] = anArray[i];
	}
}
template<typename TYPE>
SMatrix33<TYPE>::SMatrix33(
	const TYPE a11, const TYPE a12, const TYPE a13,
	const TYPE a21, const TYPE a22, const TYPE a23,
	const TYPE a31, const TYPE a32, const TYPE a33)
{
	m11 = a11; m12 = a12; m13 = a13;
	m21 = a21; m22 = a22; m23 = a23;
	m31 = a31; m32 = a32; m33 = a33;
}
template<typename TYPE>
SMatrix33<TYPE>::SMatrix33(const SVector2<TYPE> & aPosition)
	: SMatrix33()
{
	SetPosition(aPosition);
}

template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::operator +=(const SMatrix33<TYPE> & aRight)
{
	for (unsigned char i = 0; i < NumElements; ++i)
	{
		myMatrix[i] += aRight.myMatrix[i];
	}
	return *this;
}
template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::operator -=(const SMatrix33<TYPE> & aRight)
{
	for (unsigned char i = 0; i < NumElements; ++i)
	{
		myMatrix[i] -= aRight.myMatrix[i];
	}
	return *this;
}
template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::operator *=(const SMatrix33<TYPE> & aRight)
{
	return *this = *this * aRight;
}

template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::operator =(const SMatrix33<TYPE> & aRight)
{
	for (unsigned char i = 0; i < NumElements; ++i)
	{
		myMatrix[i] = aRight.myMatrix[i];
	}
	return *this;
}

template<typename TYPE>
void SMatrix33<TYPE>::SetPosition(const SVector2<TYPE> & aPosition)
{
	m31 = aPosition.X;
	m32 = aPosition.Y;
}
template<typename TYPE>
SVector2<TYPE> SMatrix33<TYPE>::GetPosition() const
{
	return {m31, m32};
}

template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::Rotate2D(const TYPE anAngle)
{
	SMatrix33 matrix;
	SVector2<TYPE> postion = GetPosition();
	*this *= matrix.Set2DRotation(anAngle);
	SetPosition(postion);
	return *this;
}

template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::Set2DRotation(const TYPE anAngle)
{
	m11 = static_cast<TYPE>(cos(anAngle));
	m12 = static_cast<TYPE>(sin(anAngle));
	m21 = -m12;
	m22 = m11;
	return *this;
}

template<typename TYPE>
SMatrix33<TYPE> & SMatrix33<TYPE>::Transpose()
{
	std::swap(m12, m21);
	std::swap(m13, m31);
	std::swap(m32, m23);
	return *this;
}
template<typename TYPE>
SMatrix33<TYPE> SMatrix33<TYPE>::Transpose(SMatrix33 aMatrix)
{
	return aMatrix.Transpose();
}

template<typename TYPE>
SMatrix33<TYPE> &  SMatrix33<TYPE>::Inverse()
{
	SVector2<TYPE> position = -GetPosition();
	SetPosition(SVector2<TYPE>::Zero());
	m33 = 0;

	Transpose();

	position = position * *this;
	SetPosition(position);
	m33 = 1;

	return *this;

}
template<typename TYPE>
SMatrix33<TYPE> SMatrix33<TYPE>::Inverse(SMatrix33 aMatrix)
{
	return aMatrix.Inverse();
}

template<typename TYPE>
SMatrix33<TYPE> SMatrix33<TYPE>::CreateRotateAroundX(const TYPE anAngle)
{
	return SMatrix33(
		1, 0, 0,
		0, static_cast<TYPE>(cos(anAngle)), static_cast<TYPE>(sin(anAngle)),
		0, -static_cast<TYPE>(sin(anAngle)), static_cast<TYPE>(cos(anAngle))
		);
}
template<typename TYPE>
SMatrix33<TYPE> SMatrix33<TYPE>::CreateRotateAroundY(const TYPE anAngle)
{
	return SMatrix33(
		static_cast<TYPE>(cos(anAngle)), 0, -static_cast<TYPE>(sin(anAngle)),
		0, 1, 0,
		static_cast<TYPE>(sin(anAngle)), 0, static_cast<TYPE>(cos(anAngle))
		);
}
template<typename TYPE>
SMatrix33<TYPE> SMatrix33<TYPE>::CreateRotateAroundZ(const TYPE anAngle)
{
	return SMatrix33(
		static_cast<TYPE>(cos(anAngle)), static_cast<TYPE>(sin(anAngle)), 0,
		-static_cast<TYPE>(sin(anAngle)), static_cast<TYPE>(cos(anAngle)), 0,
		0, 0, 1
		);
}
