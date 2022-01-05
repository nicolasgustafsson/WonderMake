#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "WonderMakeBase/Vector.h"

#include <numbers>

template<typename T>
struct VecUnits
{
	static constexpr auto X2 = SVector2f(1.f, 0.f);
	static constexpr auto Y2 = SVector2f(0.f, 1.f);

	static constexpr auto X3 = SVector3f(1.f, 0.f, 0.f);
	static constexpr auto Y3 = SVector3f(0.f, 1.f, 0.f);
	static constexpr auto Z3 = SVector3f(0.f, 0.f, 1.f);

	static constexpr auto X4 = SVector4f(1.f, 0.f, 0.f, 0.f);
	static constexpr auto Y4 = SVector4f(0.f, 1.f, 0.f, 0.f);
	static constexpr auto Z4 = SVector4f(0.f, 0.f, 1.f, 0.f);
	static constexpr auto W4 = SVector4f(0.f, 0.f, 0.f, 1.f);
};

constexpr auto InvokeVec2Floating = [](const auto aCallable)
{
	aCallable(SVector2<f32>());
	aCallable(SVector2<f64>());
};
constexpr auto InvokeVec2Signed = [](const auto aCallable)
{
	aCallable(SVector2<i32>());
	aCallable(SVector2<i64>());
};
constexpr auto InvokeVec2Unsigned = [](const auto aCallable)
{
	aCallable(SVector2<u32>());
	aCallable(SVector2<u64>());
};
constexpr auto InvokeVec3Floating = [](const auto aCallable)
{
	aCallable(SVector3<f32>());
	aCallable(SVector3<f64>());
};
constexpr auto InvokeVec3Signed = [](const auto aCallable)
{
	aCallable(SVector3<i32>());
	aCallable(SVector3<i64>());
};
constexpr auto InvokeVec3Unsigned = [](const auto aCallable)
{
	aCallable(SVector3<u32>());
	aCallable(SVector3<u64>());
};
constexpr auto InvokeVec4Floating = [](const auto aCallable)
{
	aCallable(SVector4<f32>());
	aCallable(SVector4<f64>());
};
constexpr auto InvokeVec4Signed = [](const auto aCallable)
{
	aCallable(SVector4<i32>());
	aCallable(SVector4<i64>());
};
constexpr auto InvokeVec4Unsigned = [](const auto aCallable)
{
	aCallable(SVector4<u32>());
	aCallable(SVector4<u64>());
};
constexpr auto InvokeVec2 = [](const auto aCallable)
{
	InvokeVec2Floating(aCallable);
	InvokeVec2Signed(aCallable);
	InvokeVec2Unsigned(aCallable);
};
constexpr auto InvokeVec3 = [](const auto aCallable)
{
	InvokeVec3Floating(aCallable);
	InvokeVec3Signed(aCallable);
	InvokeVec3Unsigned(aCallable);
};
constexpr auto InvokeVec4 = [](const auto aCallable)
{
	InvokeVec4Floating(aCallable);
	InvokeVec4Signed(aCallable);
	InvokeVec4Unsigned(aCallable);
};
constexpr auto InvokeVec = [](const auto aCallable)
{
	InvokeVec2(aCallable);
	InvokeVec3(aCallable);
	InvokeVec4(aCallable);
};

void StaticTest_Vector_CheckName()
{
	constexpr auto checkMemberNameVec2 = [](const auto aVector)
	{
		using type = decltype(aVector);

		static_assert(aVector.GetMemberName(0) == 'X');
		static_assert(aVector.GetMemberName(1) == 'Y');
	};
	constexpr auto checkMemberNameVec3 = [](const auto aVector)
	{
		using type = decltype(aVector);

		static_assert(aVector.GetMemberName(0) == 'X');
		static_assert(aVector.GetMemberName(1) == 'Y');
		static_assert(aVector.GetMemberName(2) == 'Z');
	};
	constexpr auto checkMemberNameVec4 = [](const auto aVector)
	{
		using type = decltype(aVector);

		static_assert(aVector.GetMemberName(0) == 'X');
		static_assert(aVector.GetMemberName(1) == 'Y');
		static_assert(aVector.GetMemberName(2) == 'Z');
		static_assert(aVector.GetMemberName(3) == 'W');
	};
	constexpr auto checkMemberNameVec5 = [](const auto aVector)
	{
		using type = decltype(aVector);

		static_assert(aVector.GetMemberName(0) == 'X');
		static_assert(aVector.GetMemberName(1) == 'Y');
		static_assert(aVector.GetMemberName(2) == 'Z');
		static_assert(aVector.GetMemberName(3) == 'W');
		static_assert(aVector.GetMemberName(4) == '5');
	};
	constexpr auto checkMemberNameVec6 = [](const auto aVector)
	{
		using type = decltype(aVector);

		static_assert(aVector.GetMemberName(0) == 'X');
		static_assert(aVector.GetMemberName(1) == 'Y');
		static_assert(aVector.GetMemberName(2) == 'Z');
		static_assert(aVector.GetMemberName(3) == 'W');
		static_assert(aVector.GetMemberName(4) == '5');
		static_assert(aVector.GetMemberName(5) == '6');
	};

	InvokeVec2(checkMemberNameVec2);
	InvokeVec3(checkMemberNameVec3);
	InvokeVec4(checkMemberNameVec4);

	checkMemberNameVec5(SVector<f32, 5>());
	checkMemberNameVec5(SVector<f64, 5>());
	checkMemberNameVec5(SVector<i32, 5>());
	checkMemberNameVec5(SVector<i64, 5>());
	checkMemberNameVec5(SVector<u32, 5>());
	checkMemberNameVec5(SVector<u64, 5>());

	checkMemberNameVec6(SVector<f32, 6>());
	checkMemberNameVec6(SVector<f64, 6>());
	checkMemberNameVec6(SVector<i32, 6>());
	checkMemberNameVec6(SVector<i64, 6>());
	checkMemberNameVec6(SVector<u32, 6>());
	checkMemberNameVec6(SVector<u64, 6>());
}

void StaticTest_Vector_Operators()
{
	constexpr auto checkUnaryMinus = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vec = -type::One();

		static_assert(vec[0] == -1);
		static_assert(vec[1] == -1);
		if constexpr (type::Size >= 3)
			static_assert(vec[2] == -1);
		if constexpr (type::Size >= 4)
			static_assert(vec[3] == -1);
	};

	InvokeVec2Floating(checkUnaryMinus);
	InvokeVec2Signed(checkUnaryMinus);
	InvokeVec3Floating(checkUnaryMinus);
	InvokeVec3Signed(checkUnaryMinus);
	InvokeVec4Floating(checkUnaryMinus);
	InvokeVec4Signed(checkUnaryMinus);

	constexpr auto checkMultiplicationAssignment = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto math = []()
		{
			auto vec = type::One();

			vec *= 2;

			return vec;
		};

		constexpr auto vec = math();

		static_assert(vec[0] == 2);
		static_assert(vec[1] == 2);
		if constexpr (type::Size >= 3)
			static_assert(vec[2] == 2);
		if constexpr (type::Size >= 4)
			static_assert(vec[3] == 2);
	};
	constexpr auto checkDivisionAssignment = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto math = []()
		{
			auto vec = type::One() * type::Representation(2);

			vec /= 2;

			return vec;
		};

		constexpr auto vec = math();

		static_assert(vec[0] == 1);
		static_assert(vec[1] == 1);
		if constexpr (type::Size >= 3)
			static_assert(vec[2] == 1);
		if constexpr (type::Size >= 4)
			static_assert(vec[3] == 1);
	};
	constexpr auto checkAdditionAssignment = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto math = []()
		{
			auto vec = type::One();

			vec += type::One();

			return vec;
		};

		constexpr auto vec = math();

		static_assert(vec[0] == 2);
		static_assert(vec[1] == 2);
		if constexpr (type::Size >= 3)
			static_assert(vec[2] == 2);
		if constexpr (type::Size >= 4)
			static_assert(vec[3] == 2);
	};
	constexpr auto checkSubtractionAssignment = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto math = []()
		{
			auto vec = type::One();

			vec -= type::One();

			return vec;
		};

		constexpr auto vec = math();

		static_assert(vec[0] == 0);
		static_assert(vec[1] == 0);
		if constexpr (type::Size >= 3)
			static_assert(vec[2] == 0);
		if constexpr (type::Size >= 4)
			static_assert(vec[3] == 0);
	};
	constexpr auto checkComparison = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vecZeroA = type::Zero();
		constexpr auto vecZeroB = type::Zero();
		constexpr auto vecOneA = type::One();
		constexpr auto vecOneB = type::One();

		static_assert(vecZeroA == vecZeroA);
		static_assert(vecZeroA == vecZeroB);
		static_assert(vecZeroA != vecOneA);
		static_assert(vecZeroA != vecOneB);

		static_assert(vecZeroB == vecZeroA);
		static_assert(vecZeroB == vecZeroB);
		static_assert(vecZeroB != vecOneA);
		static_assert(vecZeroB != vecOneB);

		static_assert(vecOneA != vecZeroA);
		static_assert(vecOneA != vecZeroB);
		static_assert(vecOneA == vecOneA);
		static_assert(vecOneA == vecOneB);

		static_assert(vecOneB != vecZeroA);
		static_assert(vecOneB != vecZeroB);
		static_assert(vecOneB == vecOneA);
		static_assert(vecOneB == vecOneB);
	};

	InvokeVec(checkMultiplicationAssignment);
	InvokeVec(checkDivisionAssignment);
	InvokeVec(checkAdditionAssignment);
	InvokeVec(checkSubtractionAssignment);
	InvokeVec(checkComparison);

	constexpr auto checkSubtraction = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vecA = type::One() - type::One();
		constexpr auto vecB = type::One() - type::Zero();

		static_assert(vecA[0] == 0);
		static_assert(vecA[1] == 0);
		if constexpr (type::Size >= 3)
			static_assert(vecA[2] == 0);
		if constexpr (type::Size >= 4)
			static_assert(vecA[3] == 0);

		static_assert(vecB[0] == 1);
		static_assert(vecB[1] == 1);
		if constexpr (type::Size >= 3)
			static_assert(vecB[2] == 1);
		if constexpr (type::Size >= 4)
			static_assert(vecB[3] == 1);
	};
	constexpr auto checkAddition = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vecA = type::One() + type::One();
		constexpr auto vecB = type::One() + type::Zero();
		constexpr auto vecC = type::Zero() + type::One();

		static_assert(vecA[0] == 2);
		static_assert(vecA[1] == 2);
		if constexpr (type::Size >= 3)
			static_assert(vecA[2] == 2);
		if constexpr (type::Size >= 4)
			static_assert(vecA[3] == 2);

		static_assert(vecB[0] == 1);
		static_assert(vecB[1] == 1);
		if constexpr (type::Size >= 3)
			static_assert(vecB[2] == 1);
		if constexpr (type::Size >= 4)
			static_assert(vecB[3] == 1);

		static_assert(vecC[0] == 1);
		static_assert(vecC[1] == 1);
		if constexpr (type::Size >= 3)
			static_assert(vecC[2] == 1);
		if constexpr (type::Size >= 4)
			static_assert(vecC[3] == 1);
	};
	constexpr auto checkMultiplication = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vecA = type::One() * type::Representation(2);
		constexpr auto vecB = type::Representation(3) * type::One();

		static_assert(vecA[0] == 2);
		static_assert(vecA[1] == 2);
		if constexpr (type::Size >= 3)
			static_assert(vecA[2] == 2);
		if constexpr (type::Size >= 4)
			static_assert(vecA[3] == 2);

		static_assert(vecB[0] == 3);
		static_assert(vecB[1] == 3);
		if constexpr (type::Size >= 3)
			static_assert(vecB[2] == 3);
		if constexpr (type::Size >= 4)
			static_assert(vecB[3] == 3);
	};
	constexpr auto checkDivision = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vecA = (type::One() * type::Representation(4)) / type::Representation(2);
		constexpr auto vecB = (type::One() * type::Representation(9)) / type::Representation(3);

		static_assert(vecA[0] == 2);
		static_assert(vecA[1] == 2);
		if constexpr (type::Size >= 3)
			static_assert(vecA[2] == 2);
		if constexpr (type::Size >= 4)
			static_assert(vecA[3] == 2);

		static_assert(vecB[0] == 3);
		static_assert(vecB[1] == 3);
		if constexpr (type::Size >= 3)
			static_assert(vecB[2] == 3);
		if constexpr (type::Size >= 4)
			static_assert(vecB[3] == 3);
	};

	InvokeVec(checkSubtraction);
	InvokeVec(checkAddition);
	InvokeVec(checkMultiplication);
	InvokeVec(checkDivision);
}

void StaticTest_Vector_DimensionalTransformation()
{
	constexpr auto checkPromoteVec2ToVec3 = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vec = type(1, 2);

		constexpr auto vecWithDefault = vec.Promote();
		constexpr auto vecWithZero = vec.Promote(0);
		constexpr auto vecWithThree = vec.Promote(3);

		static_assert(vecWithDefault[0] == 1);
		static_assert(vecWithDefault[1] == 2);
		static_assert(vecWithDefault[2] == 0);

		static_assert(vecWithZero[0] == 1);
		static_assert(vecWithZero[1] == 2);
		static_assert(vecWithZero[2] == 0);

		static_assert(vecWithThree[0] == 1);
		static_assert(vecWithThree[1] == 2);
		static_assert(vecWithThree[2] == 3);
	};
	constexpr auto checkPromoteVec3ToVec4 = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vec = type(1, 2, 3);

		constexpr auto vecWithDefault = vec.Promote();
		constexpr auto vecWithZero = vec.Promote(0);
		constexpr auto vecWithFour = vec.Promote(4);

		static_assert(vecWithDefault[0] == 1);
		static_assert(vecWithDefault[1] == 2);
		static_assert(vecWithDefault[2] == 3);
		static_assert(vecWithDefault[3] == 0);

		static_assert(vecWithZero[0] == 1);
		static_assert(vecWithZero[1] == 2);
		static_assert(vecWithZero[2] == 3);
		static_assert(vecWithZero[3] == 0);

		static_assert(vecWithFour[0] == 1);
		static_assert(vecWithFour[1] == 2);
		static_assert(vecWithFour[2] == 3);
		static_assert(vecWithFour[3] == 4);
	};

	InvokeVec2(checkPromoteVec2ToVec3);
	InvokeVec3(checkPromoteVec3ToVec4);

	constexpr auto checkDemoteVec2ToScalar = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vec = type(1, 2);

		constexpr auto vecDemoted = vec.Demote();

		static_assert(vecDemoted == 1);
	};
	constexpr auto checkDemoteVec3ToVec2 = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vec = type(1, 2, 3);

		constexpr auto vecDemoted = vec.Demote();

		static_assert(vecDemoted[0] == 1);
		static_assert(vecDemoted[1] == 2);
	};
	constexpr auto checkDemoteVec4ToVec3 = [](const auto aVector)
	{
		using type = decltype(aVector);

		constexpr auto vec = type(1, 2, 3, 4);

		constexpr auto vecDemoted = vec.Demote();

		static_assert(vecDemoted[0] == 1);
		static_assert(vecDemoted[1] == 2);
		static_assert(vecDemoted[2] == 3);
	};

	InvokeVec2(checkDemoteVec2ToScalar);
	InvokeVec3(checkDemoteVec3ToVec2);
	InvokeVec4(checkDemoteVec4ToVec3);
}

TEST_CASE("Vector member access", "[Vector]")
{
	SECTION("Vector2")
	{
		const auto check = [](const auto aVector)
		{
			using type = decltype(aVector);

			auto vec = type(1, 2);

			CHECK(vec[0] == 1);
			CHECK(vec[1] == 2);

			vec[0] = 3;

			CHECK(vec[0] == 3);
			CHECK(vec[1] == 2);

			vec[1] = 4;

			CHECK(vec[0] == 3);
			CHECK(vec[1] == 4);
		};

		InvokeVec2(check);
	}

	SECTION("Vector3")
	{
		const auto check = [](const auto aVector)
		{
			using type = decltype(aVector);

			auto vec = type(1, 2, 3);

			CHECK(vec[0] == 1);
			CHECK(vec[1] == 2);
			CHECK(vec[2] == 3);

			vec[0] = 4;

			CHECK(vec[0] == 4);
			CHECK(vec[1] == 2);
			CHECK(vec[2] == 3);

			vec[1] = 5;

			CHECK(vec[0] == 4);
			CHECK(vec[1] == 5);
			CHECK(vec[2] == 3);

			vec[2] = 6;

			CHECK(vec[0] == 4);
			CHECK(vec[1] == 5);
			CHECK(vec[2] == 6);
		};

		InvokeVec3(check);
	}

	SECTION("Vector4")
	{
		const auto check = [](const auto aVector)
		{
			using type = decltype(aVector);

			auto vec = type(1, 2, 3, 4);

			CHECK(vec[0] == 1);
			CHECK(vec[1] == 2);
			CHECK(vec[2] == 3);
			CHECK(vec[3] == 4);

			vec[0] = 5;

			CHECK(vec[0] == 5);
			CHECK(vec[1] == 2);
			CHECK(vec[2] == 3);
			CHECK(vec[3] == 4);

			vec[1] = 6;

			CHECK(vec[0] == 5);
			CHECK(vec[1] == 6);
			CHECK(vec[2] == 3);
			CHECK(vec[3] == 4);

			vec[2] = 7;

			CHECK(vec[0] == 5);
			CHECK(vec[1] == 6);
			CHECK(vec[2] == 7);
			CHECK(vec[3] == 4);

			vec[3] = 8;

			CHECK(vec[0] == 5);
			CHECK(vec[1] == 6);
			CHECK(vec[2] == 7);
			CHECK(vec[3] == 8);
		};

		InvokeVec4(check);
	}
}

TEST_CASE("Vector rotate", "[Vector]")
{
	const auto closeEnough = [](const auto aValue, const auto aCompare)
	{
		return aValue > aCompare - 0.001
			&& aValue < aCompare + 0.001;
	};

	SECTION("Vector2")
	{
		const auto checkAll = [&closeEnough](const auto aVector)
		{
			using rep = typename decltype(aVector)::Representation;

			auto unitX = VecUnits<rep>::X2 * 10.f;
			auto unitY = VecUnits<rep>::Y2 * 10.f;

			unitX.RotateCounterClockwise(-90.f);
			unitY.RotateCounterClockwise(90.f);

			CHECK(closeEnough(unitX[0], 0));
			CHECK(closeEnough(unitX[1], -10));
			CHECK(closeEnough(unitY[0], -10));
			CHECK(closeEnough(unitY[1], 0));
		};

		const auto checkNegative = [&closeEnough](const auto aVector)
		{
			using rep = typename decltype(aVector)::Representation;

			auto unitX = VecUnits<rep>::X2 * 10.f;
			auto unitY = VecUnits<rep>::Y2 * 10.f;

			unitX.RotateCounterClockwise(90.f);
			unitY.RotateCounterClockwise(-90.f);

			CHECK(closeEnough(unitX[0], 0));
			CHECK(closeEnough(unitX[1], 10));
			CHECK(closeEnough(unitY[0], 10));
			CHECK(closeEnough(unitY[1], 0));
		};

		InvokeVec2Floating(checkAll);
		InvokeVec2Floating(checkNegative);
	}
}

TEST_CASE("Vector normalize", "[Vector]")
{
	SECTION("Vector2")
	{
		const auto check = [](const auto aVector)
		{
			using rep = typename decltype(aVector)::Representation;

			auto vec2X				= VecUnits<rep>::X2;
			auto vec2Y				= VecUnits<rep>::Y2;

			auto vec2DoubleX		= VecUnits<rep>::X2 * 2.f;
			auto vec2DoubleY		= VecUnits<rep>::Y2 * 2.f;

			const auto vec2GetNormX	= VecUnits<rep>::X2.GetNormalized();
			const auto vec2GetNormY	= VecUnits<rep>::Y2.GetNormalized();

			vec2X.Normalize();
			vec2Y.Normalize();

			vec2DoubleX.Normalize();
			vec2DoubleY.Normalize();

			CHECK(vec2X			== VecUnits<rep>::X2);
			CHECK(vec2Y			== VecUnits<rep>::Y2);

			CHECK(vec2DoubleX	== VecUnits<rep>::X2);
			CHECK(vec2DoubleY	== VecUnits<rep>::Y2);

			CHECK(vec2GetNormX	== VecUnits<rep>::X2);
			CHECK(vec2GetNormY	== VecUnits<rep>::Y2);
		};

		InvokeVec2(check);
	}

	SECTION("Vector3")
	{
		const auto check = [](const auto aVector)
		{
			using rep = typename decltype(aVector)::Representation;

			auto vec3X				= VecUnits<rep>::X3;
			auto vec3Y				= VecUnits<rep>::Y3;
			auto vec3Z				= VecUnits<rep>::Z3;

			auto vec3DoubleX		= VecUnits<rep>::X3 * 2.f;
			auto vec3DoubleY		= VecUnits<rep>::Y3 * 2.f;
			auto vec3DoubleZ		= VecUnits<rep>::Z3 * 2.f;

			const auto vec3GetNormX	= VecUnits<rep>::X3.GetNormalized();
			const auto vec3GetNormY	= VecUnits<rep>::Y3.GetNormalized();
			const auto vec3GetNormZ	= VecUnits<rep>::Z3.GetNormalized();

			vec3X.Normalize();
			vec3Y.Normalize();
			vec3Z.Normalize();

			vec3DoubleX.Normalize();
			vec3DoubleY.Normalize();
			vec3DoubleZ.Normalize();

			CHECK(vec3X			== VecUnits<rep>::X3);
			CHECK(vec3Y			== VecUnits<rep>::Y3);
			CHECK(vec3Z			== VecUnits<rep>::Z3);

			CHECK(vec3DoubleX	== VecUnits<rep>::X3);
			CHECK(vec3DoubleY	== VecUnits<rep>::Y3);
			CHECK(vec3DoubleZ	== VecUnits<rep>::Z3);

			CHECK(vec3GetNormX	== VecUnits<rep>::X3);
			CHECK(vec3GetNormY	== VecUnits<rep>::Y3);
			CHECK(vec3GetNormZ	== VecUnits<rep>::Z3);
		};

		InvokeVec3(check);
	}

	SECTION("Vector4")
	{
		const auto check = [](const auto aVector)
		{
			using rep = typename decltype(aVector)::Representation;

			auto vec4X				= VecUnits<rep>::X4;
			auto vec4Y				= VecUnits<rep>::Y4;
			auto vec4Z				= VecUnits<rep>::Z4;
			auto vec4W				= VecUnits<rep>::W4;

			auto vec4DoubleX		= VecUnits<rep>::X4 * 2.f;
			auto vec4DoubleY		= VecUnits<rep>::Y4 * 2.f;
			auto vec4DoubleZ		= VecUnits<rep>::Z4 * 2.f;
			auto vec4DoubleW		= VecUnits<rep>::W4 * 2.f;

			const auto vec4GetNormX	= VecUnits<rep>::X4.GetNormalized();
			const auto vec4GetNormY	= VecUnits<rep>::Y4.GetNormalized();
			const auto vec4GetNormZ	= VecUnits<rep>::Z4.GetNormalized();
			const auto vec4GetNormW	= VecUnits<rep>::W4.GetNormalized();

			vec4X.Normalize();
			vec4Y.Normalize();
			vec4Z.Normalize();
			vec4W.Normalize();

			vec4DoubleX.Normalize();
			vec4DoubleY.Normalize();
			vec4DoubleZ.Normalize();
			vec4DoubleW.Normalize();

			CHECK(vec4X			== VecUnits<rep>::X4);
			CHECK(vec4Y			== VecUnits<rep>::Y4);
			CHECK(vec4Z			== VecUnits<rep>::Z4);
			CHECK(vec4W			== VecUnits<rep>::W4);

			CHECK(vec4DoubleX	== VecUnits<rep>::X4);
			CHECK(vec4DoubleY	== VecUnits<rep>::Y4);
			CHECK(vec4DoubleZ	== VecUnits<rep>::Z4);
			CHECK(vec4DoubleW	== VecUnits<rep>::W4);

			CHECK(vec4GetNormX	== VecUnits<rep>::X4);
			CHECK(vec4GetNormY	== VecUnits<rep>::Y4);
			CHECK(vec4GetNormZ	== VecUnits<rep>::Z4);
			CHECK(vec4GetNormW	== VecUnits<rep>::W4);
		};

		InvokeVec4(check);
	}
}

TEST_CASE("Vector length", "[Vector]")
{
	const auto closeEnough = [](const auto aValue, const auto aCompare)
	{
		return aValue > aCompare - 0.001
			&& aValue < aCompare + 0.001;
	};

	SECTION("Vector2")
	{
		const auto check = [&closeEnough](const auto aVector)
		{
			using type = decltype(aVector);
			
			constexpr auto vec = type::One();

			const auto lenSquared = vec.LengthSquared();
			const auto len = vec.Length();
			const auto distance = vec.DistanceTo(type::One() * type::Representation(2));

			CHECK(lenSquared == 2);
			CHECK(closeEnough(len, 1.4142));
			CHECK(closeEnough(distance, 1.4142));
		};

		InvokeVec2Floating(check);
	}

	SECTION("Vector3")
	{
		const auto check = [&closeEnough](const auto aVector)
		{
			using type = decltype(aVector);

			constexpr auto vec = type::One();

			const auto lenSquared = vec.LengthSquared();
			const auto len = vec.Length();
			const auto distance = vec.DistanceTo(type::One() * type::Representation(2));

			CHECK(lenSquared == 3);
			CHECK(closeEnough(len, 1.7320));
			CHECK(closeEnough(distance, 1.7320));
		};

		InvokeVec3Floating(check);
	}

	SECTION("Vector4")
	{
		const auto check = [&closeEnough](const auto aVector)
		{
			using type = decltype(aVector);

			constexpr auto vec = type::One();

			const auto lenSquared = vec.LengthSquared();
			const auto len = vec.Length();
			const auto distance = vec.DistanceTo(type::One() * type::Representation(2));

			CHECK(lenSquared == 4);
			CHECK(closeEnough(len, 2));
			CHECK(closeEnough(distance, 2));
		};

		InvokeVec4Floating(check);
	}
}

TEST_CASE("Vector dot", "[Vector]")
{
	const auto checkDot = [](const auto aProjector, const auto aBaseLine, const auto aExpectedValue)
	{
		const auto closeEnough = [](const auto aValue, const auto aCompare)
		{
			return aValue > aCompare - 0.001
				&& aValue < aCompare + 0.001;
		};

		CHECK(closeEnough(aProjector.Dot(aBaseLine), aExpectedValue));
	};

	SECTION("Vector2")
	{
		const auto check = [&checkDot](const auto aVector)
		{
			using type = decltype(aVector);

			checkDot(type(0, 1),	type(1, 0),		0);
			checkDot(type(3, 4),	type(1, 2),		11);
			checkDot(type(43, 23),	type(54, 2),	2368);
			checkDot(type(-2, -4),	type(1, 10),	-42);
			checkDot(type(-2, -4),	type(1, 0),		-2);
		};

		InvokeVec2Floating(check);
	}

	SECTION("Vector3")
	{
		const auto check = [&checkDot](const auto aVector)
		{
			using type = decltype(aVector);

			checkDot(type(0, 1, 2),		type(3, 4, 5),		14);
			checkDot(type(23, 12, 87),	type(59, 12, 9),	2284);
			checkDot(type(90, 2, 1),	type(12, 4, 2),		1090);
			checkDot(type(-28, 23, -6),	type(5, -87, 21),	-2267);
			checkDot(type(-2, 1, 0),	type(1, 0, 1),		-2);
		};

		InvokeVec3Floating(check);
	}

	SECTION("Vector4")
	{
		const auto check = [&checkDot](const auto aVector)
		{
			using type = decltype(aVector);

			checkDot(type(0, 1, 2, 3),		type(4, 5, 6, 7),		38);
			checkDot(type(32, 54, -1, 24),	type(56, 12, 54, 92),	4594);
			checkDot(type(7, 1, 0, 12),		type(84, 23, -22, 75),	1511);
			checkDot(type(-32, -1, 0, -12),	type(12, 43, 87, 4),	-475);
			checkDot(type(-74, -22, 12, 3),	type(-55, -10, 8, -80),	4146);
		};

		InvokeVec4Floating(check);
	}
}
