#define _ITERATOR_DEBUG_LEVEL 0

#include "Utilities/Matrix.h"

void StaticTest_Matrix()
{
	constexpr auto mat33Default = SMatrix33f();
	constexpr auto mat44Default = SMatrix44f();
	constexpr auto mat33Identity = SMatrix33f(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1);
	constexpr auto mat44Identity = SMatrix44f(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	static_assert(mat33Default == mat33Identity);
	static_assert(mat44Default == mat44Identity);

	constexpr auto mat32 = SMatrix<i32, 3, 2>(
		1, 2, 3,
		4, 5, 6);
	constexpr auto mat23 = SMatrix<i32, 2, 3>(
		1, 2,
		3, 4,
		5, 6);

	constexpr auto mat32x23Result = mat32 * mat23;
	constexpr auto mat32x23Expected = SMatrix<i32, 2, 2>(
		22, 28,
		49, 64);

	constexpr auto mat23x32Result = mat23 * mat32;
	constexpr auto mat23x32Expected = SMatrix<i32, 3, 3>(
		9, 12, 15,
		19, 26, 33,
		29, 40, 51);

	static_assert(mat32x23Result == mat32x23Expected);
	static_assert(mat23x32Result == mat23x32Expected);
}
