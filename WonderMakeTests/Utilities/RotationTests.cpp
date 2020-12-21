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

template<typename TRotation>
static constexpr auto pi_v = std::numbers::pi_v<typename TRotation::Representation>;

static constexpr auto rad32_n2Pi	= SRadianF32(pi_v<SRadianF32> * -2.0f);
static constexpr auto rad32_n1_5Pi	= SRadianF32(pi_v<SRadianF32> * -1.5f);
static constexpr auto rad32_n1Pi	= SRadianF32(pi_v<SRadianF32> * -1.0f);
static constexpr auto rad32_n0_5Pi	= SRadianF32(pi_v<SRadianF32> * -0.5f);
static constexpr auto rad32_0		= SRadianF32(0);
static constexpr auto rad32_0_5Pi	= SRadianF32(pi_v<SRadianF32> * 0.5f);
static constexpr auto rad32_1Pi		= SRadianF32(pi_v<SRadianF32> * 1.0f);
static constexpr auto rad32_1_5Pi	= SRadianF32(pi_v<SRadianF32> * 1.5f);
static constexpr auto rad32_2Pi		= SRadianF32(pi_v<SRadianF32> * 2.0f);

static constexpr auto rad64_n2Pi	= SRadianF64(pi_v<SRadianF64> * -2.0);
static constexpr auto rad64_n1_5Pi	= SRadianF64(pi_v<SRadianF64> * -1.5);
static constexpr auto rad64_n1Pi	= SRadianF64(pi_v<SRadianF64> * -1.0);
static constexpr auto rad64_n0_5Pi	= SRadianF64(pi_v<SRadianF64> * -0.5);
static constexpr auto rad64_0		= SRadianF64(0);
static constexpr auto rad64_0_5Pi	= SRadianF64(pi_v<SRadianF64> * 0.5);
static constexpr auto rad64_1Pi		= SRadianF64(pi_v<SRadianF64> * 1.0);
static constexpr auto rad64_1_5Pi	= SRadianF64(pi_v<SRadianF64> * 1.5);
static constexpr auto rad64_2Pi		= SRadianF64(pi_v<SRadianF64> * 2.0);

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

	static_assert(deg32_n360	== deg64_n360);
	static_assert(deg32_n270	== deg64_n270);
	static_assert(deg32_n180	== deg64_n180);
	static_assert(deg32_n90		== deg64_n90);
	static_assert(deg32_0		== deg64_0);
	static_assert(deg32_90		== deg64_90);
	static_assert(deg32_180		== deg64_180);
	static_assert(deg32_270		== deg64_270);
	static_assert(deg32_360		== deg64_360);
	
	static_assert(deg32_n360	== rad32_n2Pi);
	static_assert(deg32_n270	== rad32_n1_5Pi);
	static_assert(deg32_n180	== rad32_n1Pi);
	static_assert(deg32_n90		== rad32_n0_5Pi);
	static_assert(deg32_0		== rad32_0);
	static_assert(deg32_90		== rad32_0_5Pi);
	static_assert(deg32_180		== rad32_1Pi);
	static_assert(deg32_270		== rad32_1_5Pi);
	static_assert(deg32_360		== rad32_2Pi);
	
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

	constexpr auto mul = [](auto aRhs, const auto aLhs)
	{
		return aRhs *= aLhs;
	};
	constexpr auto div = [](auto aRhs, const auto aLhs)
	{
		return aRhs /= aLhs;
	};
	
	static_assert(mul(deg32_180,	2.f)	== deg32_360);
	static_assert(div(deg32_360,	2.f)	== deg32_180);
	static_assert(mul(deg32_180, deg32_180)	== (180.f * 180.f));
	static_assert(div(mul(deg32_180, deg32_180),	deg32_180) == deg32_180);

	static_assert(mul(deg64_180, 2)			== deg64_360);
	static_assert(div(deg64_360, 2)			== deg64_180);
	static_assert(mul(deg64_180, deg64_180)	== (180 * 180));
	static_assert(div(mul(deg64_180, deg64_180),	deg64_180) == deg64_180);

	static_assert(mul(rad32_1Pi, 2.f)		== rad32_2Pi);
	static_assert(div(rad32_2Pi, 2.f)		== rad32_1Pi);
	static_assert(mul(rad32_1Pi, rad32_1Pi)	== (pi_v<SRadianF32> * pi_v<SRadianF32>));
	static_assert(div(mul(rad32_1Pi, rad32_1Pi), rad32_1Pi) == rad32_1Pi);
	
	static_assert(mul(rad64_1Pi, 2)			== rad64_2Pi);
	static_assert(div(rad64_2Pi, 2)			== rad64_1Pi);
	static_assert(mul(rad64_1Pi, rad64_1Pi)	== (pi_v<SRadianF64> * pi_v<SRadianF64>));
	static_assert(div(mul(rad64_1Pi, rad64_1Pi), rad64_1Pi) == rad64_1Pi);
}

TEST_CASE("WmMath overloads for Rotation return correct values", "[Rotation]")
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

	CHECK(closeEnough(WmMath::Atan(deg32_n360),	WmMath::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_n270),	WmMath::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_n180),	WmMath::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_n90),		WmMath::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_0),		WmMath::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_90),		WmMath::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_180),		WmMath::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_270),		WmMath::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Atan(deg32_360),		WmMath::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Atan(deg64_n360),	WmMath::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_n270),	WmMath::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_n180),	WmMath::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_n90),		WmMath::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_0),		WmMath::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_90),		WmMath::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_180),		WmMath::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_270),		WmMath::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Atan(deg64_360),		WmMath::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Atan(rad32_n2Pi),	WmMath::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_n1_5Pi),	WmMath::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_n1Pi),	WmMath::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_n0_5Pi),	WmMath::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_0),		WmMath::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_0_5Pi),	WmMath::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_1Pi),		WmMath::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_1_5Pi),	WmMath::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Atan(rad32_2Pi),		WmMath::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Atan(rad64_n2Pi),	WmMath::Atan(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_n1_5Pi),	WmMath::Atan(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_n1Pi),	WmMath::Atan(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_n0_5Pi),	WmMath::Atan(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_0),		WmMath::Atan(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_0_5Pi),	WmMath::Atan(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_1Pi),		WmMath::Atan(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_1_5Pi),	WmMath::Atan(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Atan(rad64_2Pi),		WmMath::Atan(degreeToRadian(360.f))));
	
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF32>(1, 0).Rotation),	WmMath::Atan2(1, 0)));
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF32>(0, 1).Rotation),	WmMath::Atan2(0, 1)));
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF32>(-1, 0).Rotation),	WmMath::Atan2(-1, 0)));
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF32>(0, -1).Rotation),	WmMath::Atan2(0, -1)));

	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF64>(1, 0).Rotation),	WmMath::Atan2(1, 0)));
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF64>(0, 1).Rotation),	WmMath::Atan2(0, 1)));
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF64>(-1, 0).Rotation),	WmMath::Atan2(-1, 0)));
	CHECK(closeEnough(degreeToRadian(WmMath::Atan2<SDegreeF64>(0, -1).Rotation),	WmMath::Atan2(0, -1)));
	
	CHECK(closeEnough(WmMath::Atan2<SRadianF32>(1, 0).Rotation,	WmMath::Atan2(1, 0)));
	CHECK(closeEnough(WmMath::Atan2<SRadianF32>(0, 1).Rotation,	WmMath::Atan2(0, 1)));
	CHECK(closeEnough(WmMath::Atan2<SRadianF32>(-1, 0).Rotation,	WmMath::Atan2(-1, 0)));
	CHECK(closeEnough(WmMath::Atan2<SRadianF32>(0, -1).Rotation,	WmMath::Atan2(0, -1)));
	
	CHECK(closeEnough(WmMath::Atan2<SRadianF64>(1, 0).Rotation,	WmMath::Atan2(1, 0)));
	CHECK(closeEnough(WmMath::Atan2<SRadianF64>(0, 1).Rotation,	WmMath::Atan2(0, 1)));
	CHECK(closeEnough(WmMath::Atan2<SRadianF64>(-1, 0).Rotation,	WmMath::Atan2(-1, 0)));
	CHECK(closeEnough(WmMath::Atan2<SRadianF64>(0, -1).Rotation,	WmMath::Atan2(0, -1)));

	CHECK(closeEnough(WmMath::Cos(deg32_n360),		WmMath::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_n270),		WmMath::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_n180),		WmMath::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_n90),		WmMath::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_0),		WmMath::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_90),		WmMath::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_180),		WmMath::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_270),		WmMath::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Cos(deg32_360),		WmMath::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Cos(deg64_n360),		WmMath::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_n270),		WmMath::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_n180),		WmMath::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_n90),		WmMath::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_0),		WmMath::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_90),		WmMath::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_180),		WmMath::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_270),		WmMath::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Cos(deg64_360),		WmMath::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Cos(rad32_n2Pi),		WmMath::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_n1_5Pi),	WmMath::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_n1Pi),		WmMath::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_n0_5Pi),	WmMath::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_0),		WmMath::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_0_5Pi),	WmMath::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_1Pi),		WmMath::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_1_5Pi),	WmMath::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Cos(rad32_2Pi),		WmMath::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Cos(rad64_n2Pi),		WmMath::Cos(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_n1_5Pi),	WmMath::Cos(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_n1Pi),		WmMath::Cos(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_n0_5Pi),	WmMath::Cos(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_0),		WmMath::Cos(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_0_5Pi),	WmMath::Cos(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_1Pi),		WmMath::Cos(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_1_5Pi),	WmMath::Cos(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Cos(rad64_2Pi),		WmMath::Cos(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Sin(deg32_n360),		WmMath::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_n270),		WmMath::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_n180),		WmMath::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_n90),		WmMath::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_0),		WmMath::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_90),		WmMath::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_180),		WmMath::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_270),		WmMath::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Sin(deg32_360),		WmMath::Sin(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Sin(deg64_n360),		WmMath::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_n270),		WmMath::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_n180),		WmMath::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_n90),		WmMath::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_0),		WmMath::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_90),		WmMath::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_180),		WmMath::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_270),		WmMath::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Sin(deg64_360),		WmMath::Sin(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Sin(rad32_n2Pi),		WmMath::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_n1_5Pi),	WmMath::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_n1Pi),		WmMath::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_n0_5Pi),	WmMath::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_0),		WmMath::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_0_5Pi),	WmMath::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_1Pi),		WmMath::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_1_5Pi),	WmMath::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Sin(rad32_2Pi),		WmMath::Sin(degreeToRadian(360.f))));
	
	CHECK(closeEnough(WmMath::Sin(rad64_n2Pi),		WmMath::Sin(degreeToRadian(-360.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_n1_5Pi),	WmMath::Sin(degreeToRadian(-270.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_n1Pi),		WmMath::Sin(degreeToRadian(-180.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_n0_5Pi),	WmMath::Sin(degreeToRadian(-90.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_0),		WmMath::Sin(degreeToRadian(0.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_0_5Pi),	WmMath::Sin(degreeToRadian(90.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_1Pi),		WmMath::Sin(degreeToRadian(180.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_1_5Pi),	WmMath::Sin(degreeToRadian(270.f))));
	CHECK(closeEnough(WmMath::Sin(rad64_2Pi),		WmMath::Sin(degreeToRadian(360.f))));
	
}
