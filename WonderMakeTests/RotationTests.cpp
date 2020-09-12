#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Utilities/Rotation.h"

static constexpr auto deg32_n360	= SDegreeF32(-360);
static constexpr auto deg32_n270	= SDegreeF32(-270);
static constexpr auto deg32_n180	= SDegreeF32(-180);
static constexpr auto deg32_n90		= SDegreeF32(-90);
static constexpr auto deg32_0		= SDegreeF32(0);
static constexpr auto deg32_90		= SDegreeF32(90);
static constexpr auto deg32_180		= SDegreeF32(180);
static constexpr auto deg32_270		= SDegreeF32(270);
static constexpr auto deg32_360		= SDegreeF32(360);

static constexpr auto deg64_n360	= SDegreeF64(-360);
static constexpr auto deg64_n270	= SDegreeF64(-270);
static constexpr auto deg64_n180	= SDegreeF64(-180);
static constexpr auto deg64_n90		= SDegreeF64(-90);
static constexpr auto deg64_0		= SDegreeF64(0);
static constexpr auto deg64_90		= SDegreeF64(90);
static constexpr auto deg64_180		= SDegreeF64(180);
static constexpr auto deg64_270		= SDegreeF64(270);
static constexpr auto deg64_360		= SDegreeF64(360);

static constexpr auto rad32_n2Pi	= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * -2.0f);
static constexpr auto rad32_n1_5Pi	= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * -1.5f);
static constexpr auto rad32_n1Pi	= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * -1.0f);
static constexpr auto rad32_n0_5Pi	= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * -0.5f);
static constexpr auto rad32_0		= SRadianF32(0);
static constexpr auto rad32_0_5Pi	= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * 0.5f);
static constexpr auto rad32_1Pi		= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * 1.0f);
static constexpr auto rad32_1_5Pi	= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * 1.5f);
static constexpr auto rad32_2Pi		= SRadianF32(std::numbers::pi_v<SRadianF32::Representation> * 2.0f);

static constexpr auto rad64_n2Pi	= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * -2.0);
static constexpr auto rad64_n1_5Pi	= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * -1.5);
static constexpr auto rad64_n1Pi	= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * -1.0);
static constexpr auto rad64_n0_5Pi	= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * -0.5);
static constexpr auto rad64_0		= SRadianF64(0);
static constexpr auto rad64_0_5Pi	= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * 0.5);
static constexpr auto rad64_1Pi		= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * 1.0);
static constexpr auto rad64_1_5Pi	= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * 1.5);
static constexpr auto rad64_2Pi		= SRadianF64(std::numbers::pi_v<SRadianF64::Representation> * 2.0);

void StaticTest_Rotation()
{
	static_assert((deg32_360	+ deg32_n360)	== deg32_0);
	static_assert((deg32_360	- deg32_360)	== deg32_0);
	static_assert((deg32_90		+ deg32_180)	== (deg32_180 + deg32_90));
	static_assert((deg32_90		- deg32_180)	!= (deg32_180 - deg32_90));

	static_assert((deg64_360	+ deg64_n360)	== deg64_0);
	static_assert((deg64_360	- deg64_360)	== deg64_0);
	static_assert((deg64_90		+ deg64_180)	== (deg64_180 + deg64_90));
	static_assert((deg64_90		- deg64_180)	!= (deg64_180 - deg64_90));
	
	static_assert((rad32_2Pi	+ rad32_n2Pi)	== rad32_0);
	static_assert((rad32_2Pi	- rad32_2Pi)	== rad32_0);
	static_assert((rad32_0_5Pi	+ rad32_1Pi)	== (rad32_1Pi + rad32_0_5Pi));
	static_assert((rad32_0_5Pi	- rad32_1Pi)	!= (rad32_1Pi - rad32_0_5Pi));
	
	static_assert((rad64_2Pi	+ rad64_n2Pi)	== rad64_0);
	static_assert((rad64_2Pi	- rad64_2Pi)	== rad64_0);
	static_assert((rad64_0_5Pi	+ rad64_1Pi)	== (rad64_1Pi + rad64_0_5Pi));
	static_assert((rad64_0_5Pi	- rad64_1Pi)	!= (rad64_1Pi - rad64_0_5Pi));

	static_assert(deg32_n360	== RotationCast<SDegreeF32>(deg64_n360));
	static_assert(deg32_n270	== RotationCast<SDegreeF32>(deg64_n270));
	static_assert(deg32_n180	== RotationCast<SDegreeF32>(deg64_n180));
	static_assert(deg32_n90		== RotationCast<SDegreeF32>(deg64_n90));
	static_assert(deg32_0		== RotationCast<SDegreeF32>(deg64_0));
	static_assert(deg32_90		== RotationCast<SDegreeF32>(deg64_90));
	static_assert(deg32_180		== RotationCast<SDegreeF32>(deg64_180));
	static_assert(deg32_270		== RotationCast<SDegreeF32>(deg64_270));
	static_assert(deg32_360		== RotationCast<SDegreeF32>(deg64_360));
	
	static_assert(rad32_n2Pi	== RotationCast<SRadianF32>(rad64_n2Pi));
	static_assert(rad32_n1_5Pi	== RotationCast<SRadianF32>(rad64_n1_5Pi));
	static_assert(rad32_n1Pi	== RotationCast<SRadianF32>(rad64_n1Pi));
	static_assert(rad32_n0_5Pi	== RotationCast<SRadianF32>(rad64_n0_5Pi));
	static_assert(rad32_0		== RotationCast<SRadianF32>(rad64_0));
	static_assert(rad32_0_5Pi	== RotationCast<SRadianF32>(rad64_0_5Pi));
	static_assert(rad32_1Pi		== RotationCast<SRadianF32>(rad64_1Pi));
	static_assert(rad32_1_5Pi	== RotationCast<SRadianF32>(rad64_1_5Pi));
	static_assert(rad32_2Pi		== RotationCast<SRadianF32>(rad64_2Pi));

	static_assert(deg32_n360	== RotationCast<SDegreeF32>(rad32_n2Pi));
	static_assert(deg32_n270	== RotationCast<SDegreeF32>(rad32_n1_5Pi));
	static_assert(deg32_n180	== RotationCast<SDegreeF32>(rad32_n1Pi));
	static_assert(deg32_n90		== RotationCast<SDegreeF32>(rad32_n0_5Pi));
	static_assert(deg32_0		== RotationCast<SDegreeF32>(rad32_0));
	static_assert(deg32_90		== RotationCast<SDegreeF32>(rad32_0_5Pi));
	static_assert(deg32_180		== RotationCast<SDegreeF32>(rad32_1Pi));
	static_assert(deg32_270		== RotationCast<SDegreeF32>(rad32_1_5Pi));
	static_assert(deg32_360		== RotationCast<SDegreeF32>(rad32_2Pi));
	
	static_assert(deg32_n360	== RotationCast<SDegreeF32>(rad64_n2Pi));
	static_assert(deg32_n270	== RotationCast<SDegreeF32>(rad64_n1_5Pi));
	static_assert(deg32_n180	== RotationCast<SDegreeF32>(rad64_n1Pi));
	static_assert(deg32_n90		== RotationCast<SDegreeF32>(rad64_n0_5Pi));
	static_assert(deg32_0		== RotationCast<SDegreeF32>(rad64_0));
	static_assert(deg32_90		== RotationCast<SDegreeF32>(rad64_0_5Pi));
	static_assert(deg32_180		== RotationCast<SDegreeF32>(rad64_1Pi));
	static_assert(deg32_270		== RotationCast<SDegreeF32>(rad64_1_5Pi));
	static_assert(deg32_360		== RotationCast<SDegreeF32>(rad64_2Pi));

	constexpr auto add = [](auto aRhs, const auto aLhs)
	{
		return aRhs += aLhs;
	};
	constexpr auto sub = [](auto aRhs, const auto aLhs)
	{
		return aRhs -= aLhs;
	};
	
	static_assert(add(deg32_360,	deg32_n360)	== deg32_0);
	static_assert(sub(deg32_360,	deg32_360)	== deg32_0);
	static_assert(add(deg32_90,		deg32_180)	== (deg32_180 + deg32_90));
	static_assert(sub(deg32_90,		deg32_180)	!= (deg32_180 - deg32_90));

	static_assert(add(deg64_360,	deg64_n360)	== deg64_0);
	static_assert(sub(deg64_360,	deg64_360)	== deg64_0);
	static_assert(add(deg64_90,		deg64_180)	== (deg64_180 + deg64_90));
	static_assert(sub(deg64_90,		deg64_180)	!= (deg64_180 - deg64_90));
	
	static_assert(add(rad32_2Pi,	rad32_n2Pi)	== rad32_0);
	static_assert(sub(rad32_2Pi,	rad32_2Pi)	== rad32_0);
	static_assert(add(rad32_0_5Pi,	rad32_1Pi)	== (rad32_1Pi + rad32_0_5Pi));
	static_assert(sub(rad32_0_5Pi,	rad32_1Pi)	!= (rad32_1Pi - rad32_0_5Pi));
	
	static_assert(add(rad64_2Pi,	rad64_n2Pi)	== rad64_0);
	static_assert(sub(rad64_2Pi,	rad64_2Pi)	== rad64_0);
	static_assert(add(rad64_0_5Pi,	rad64_1Pi)	== (rad64_1Pi + rad64_0_5Pi));
	static_assert(sub(rad64_0_5Pi,	rad64_1Pi)	!= (rad64_1Pi - rad64_0_5Pi));
}

TEST_CASE("MathUtility overloads for Rotation return correct values", "[Rotation]")
{
	const auto closeEnough = [](const auto aValue, const auto aCompare)
	{
		return aValue > aCompare - 0.001
			&& aValue < aCompare + 0.001;
	};
	const auto degreeToRadian = [](const auto aDegree)
	{
		return (std::numbers::pi / 180.0) * aDegree;
	};

	CHECK(closeEnough(MathUtility::Atan(deg32_n360),	MathUtility::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_n270),	MathUtility::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_n180),	MathUtility::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_n90),		MathUtility::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_0),		MathUtility::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_90),		MathUtility::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_180),		MathUtility::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_270),		MathUtility::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Atan(deg32_360),		MathUtility::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Atan(deg64_n360),	MathUtility::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_n270),	MathUtility::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_n180),	MathUtility::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_n90),		MathUtility::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_0),		MathUtility::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_90),		MathUtility::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_180),		MathUtility::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_270),		MathUtility::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Atan(deg64_360),		MathUtility::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Atan(rad32_n2Pi),	MathUtility::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_n1_5Pi),	MathUtility::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_n1Pi),	MathUtility::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_n0_5Pi),	MathUtility::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_0),		MathUtility::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_0_5Pi),	MathUtility::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_1Pi),		MathUtility::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_1_5Pi),	MathUtility::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Atan(rad32_2Pi),		MathUtility::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Atan(rad64_n2Pi),	MathUtility::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_n1_5Pi),	MathUtility::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_n1Pi),	MathUtility::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_n0_5Pi),	MathUtility::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_0),		MathUtility::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_0_5Pi),	MathUtility::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_1Pi),		MathUtility::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_1_5Pi),	MathUtility::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Atan(rad64_2Pi),		MathUtility::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF32>(1, 0).Rotation()),	MathUtility::Atan2(1., 0.)));
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF32>(0, 1).Rotation()),	MathUtility::Atan2(0., 1.)));
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF32>(-1, 0).Rotation()),	MathUtility::Atan2(-1., 0.)));
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF32>(0, -1).Rotation()),	MathUtility::Atan2(0., -1.)));

	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF64>(1, 0).Rotation()),	MathUtility::Atan2(1., 0.)));
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF64>(0, 1).Rotation()),	MathUtility::Atan2(0., 1.)));
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF64>(-1, 0).Rotation()),	MathUtility::Atan2(-1., 0.)));
	CHECK(closeEnough(degreeToRadian(MathUtility::Atan2<SDegreeF64>(0, -1).Rotation()),	MathUtility::Atan2(0., -1.)));
	
	CHECK(closeEnough(MathUtility::Atan2<SRadianF32>(1, 0).Rotation(),	MathUtility::Atan2(1., 0.)));
	CHECK(closeEnough(MathUtility::Atan2<SRadianF32>(0, 1).Rotation(),	MathUtility::Atan2(0., 1.)));
	CHECK(closeEnough(MathUtility::Atan2<SRadianF32>(-1, 0).Rotation(),	MathUtility::Atan2(-1., 0.)));
	CHECK(closeEnough(MathUtility::Atan2<SRadianF32>(0, -1).Rotation(),	MathUtility::Atan2(0., -1.)));
	
	CHECK(closeEnough(MathUtility::Atan2<SRadianF64>(1, 0).Rotation(),	MathUtility::Atan2(1., 0.)));
	CHECK(closeEnough(MathUtility::Atan2<SRadianF64>(0, 1).Rotation(),	MathUtility::Atan2(0., 1.)));
	CHECK(closeEnough(MathUtility::Atan2<SRadianF64>(-1, 0).Rotation(),	MathUtility::Atan2(-1., 0.)));
	CHECK(closeEnough(MathUtility::Atan2<SRadianF64>(0, -1).Rotation(),	MathUtility::Atan2(0., -1.)));

	CHECK(closeEnough(MathUtility::Cos(deg32_n360),		MathUtility::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_n270),		MathUtility::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_n180),		MathUtility::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_n90),		MathUtility::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_0),		MathUtility::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_90),		MathUtility::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_180),		MathUtility::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_270),		MathUtility::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Cos(deg32_360),		MathUtility::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Cos(deg64_n360),		MathUtility::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_n270),		MathUtility::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_n180),		MathUtility::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_n90),		MathUtility::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_0),		MathUtility::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_90),		MathUtility::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_180),		MathUtility::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_270),		MathUtility::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Cos(deg64_360),		MathUtility::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Cos(rad32_n2Pi),		MathUtility::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_n1_5Pi),	MathUtility::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_n1Pi),		MathUtility::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_n0_5Pi),	MathUtility::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_0),		MathUtility::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_0_5Pi),	MathUtility::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_1Pi),		MathUtility::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_1_5Pi),	MathUtility::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Cos(rad32_2Pi),		MathUtility::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Cos(rad64_n2Pi),		MathUtility::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_n1_5Pi),	MathUtility::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_n1Pi),		MathUtility::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_n0_5Pi),	MathUtility::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_0),		MathUtility::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_0_5Pi),	MathUtility::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_1Pi),		MathUtility::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_1_5Pi),	MathUtility::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Cos(rad64_2Pi),		MathUtility::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Sin(deg32_n360),		MathUtility::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_n270),		MathUtility::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_n180),		MathUtility::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_n90),		MathUtility::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_0),		MathUtility::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_90),		MathUtility::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_180),		MathUtility::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_270),		MathUtility::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Sin(deg32_360),		MathUtility::Sin(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Sin(deg64_n360),		MathUtility::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_n270),		MathUtility::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_n180),		MathUtility::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_n90),		MathUtility::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_0),		MathUtility::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_90),		MathUtility::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_180),		MathUtility::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_270),		MathUtility::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Sin(deg64_360),		MathUtility::Sin(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Sin(rad32_n2Pi),		MathUtility::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_n1_5Pi),	MathUtility::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_n1Pi),		MathUtility::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_n0_5Pi),	MathUtility::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_0),		MathUtility::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_0_5Pi),	MathUtility::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_1Pi),		MathUtility::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_1_5Pi),	MathUtility::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Sin(rad32_2Pi),		MathUtility::Sin(degreeToRadian(360.f))));
	
	CHECK(closeEnough(MathUtility::Sin(rad64_n2Pi),		MathUtility::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_n1_5Pi),	MathUtility::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_n1Pi),		MathUtility::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_n0_5Pi),	MathUtility::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_0),		MathUtility::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_0_5Pi),	MathUtility::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_1Pi),		MathUtility::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_1_5Pi),	MathUtility::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(MathUtility::Sin(rad64_2Pi),		MathUtility::Sin(degreeToRadian(360.f))));

}
