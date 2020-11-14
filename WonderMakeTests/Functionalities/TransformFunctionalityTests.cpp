#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Functionalities/TransformFunctionality.h"

TEST_CASE("TransformFunctionality basic functionality", "[TransformFunctionality]")
{
	Object object;
	STransformComponent transformComp;

	TransformFunctionality::InjectDependencies(std::tie(transformComp));

	TransformFunctionality transformFunc;

	constexpr auto closeEnough = [](const auto aValue, const auto aCompare)
	{
		constexpr auto epsilon = 0.001;

		return aValue > aCompare - epsilon
			&& aValue < aCompare + epsilon;
	};
	constexpr auto degreeToRadian = [](const auto aDegree)
	{
		return static_cast<decltype(aDegree)>((std::numbers::pi / 180.0) * aDegree);
	};

	SECTION("Setting and getting position")
	{
		CHECK(transformFunc.GetPosition() == SVector2f::Zero());

		constexpr SVector2f position(10, 5);

		transformFunc.SetPosition(position);

		CHECK(transformFunc.GetPosition() == position);
	}

	SECTION("Move position")
	{
		constexpr SVector2f origin(5, 10);
		constexpr SVector2f tranlation(-2, 4);
		constexpr SVector2f expected(3, 14);

		transformFunc.SetPosition(origin);

		transformFunc.Move(tranlation);

		CHECK(transformFunc.GetPosition() == expected);
	}

	SECTION("Setting and getting rotation")
	{
		CHECK(transformFunc.GetRotation() == 0);

		constexpr f32 rotation = 1;

		transformFunc.SetRotation(rotation);

		CHECK(transformFunc.GetRotation() == rotation);
	}

	SECTION("Facing position")
	{
		constexpr SVector2f originA(0, 0);
		constexpr SVector2f targetA(0, -1);
		constexpr f32 expectedRotationA = 0;

		transformFunc.SetPosition(originA);

		transformFunc.FacePosition(targetA);

		CHECK(closeEnough(expectedRotationA, transformFunc.GetRotation()));

		constexpr SVector2f originB(3, 2);
		constexpr SVector2f targetB(3, 1);
		constexpr f32 expectedRotationB = 0;

		transformFunc.SetPosition(originB);

		transformFunc.FacePosition(targetB);

		CHECK(closeEnough(expectedRotationB, transformFunc.GetRotation()));

		constexpr SVector2f originC(2, 2);
		constexpr SVector2f targetC(-1, -1);
		constexpr f32 expectedRotationC = degreeToRadian(45.f);

		transformFunc.SetPosition(originC);

		transformFunc.FacePosition(targetC);

		CHECK(closeEnough(expectedRotationC, transformFunc.GetRotation()));
	}

	SECTION("Facing direction")
	{
		constexpr SVector2f directionA(0, -1);
		constexpr f32 expectedRotationA = 0;

		transformFunc.FaceDirection(directionA);

		CHECK(closeEnough(expectedRotationA, transformFunc.GetRotation()));

		constexpr SVector2f directionB(0, -2.43f);
		constexpr f32 expectedRotationB = 0;

		transformFunc.FacePosition(directionB);

		CHECK(closeEnough(expectedRotationB, transformFunc.GetRotation()));

		constexpr SVector2f directionC(-1, -1);
		constexpr f32 expectedRotationC = degreeToRadian(45.f);

		transformFunc.FacePosition(directionC);

		CHECK(closeEnough(expectedRotationC, transformFunc.GetRotation()));
	}

	SECTION("Get facing vectors")
	{
		constexpr auto closeEnoughVector = [closeEnough](const auto aValue, const auto aCompare)
		{
			return closeEnough(aValue.X, aCompare.X)
				&& closeEnough(aValue.Y, aCompare.Y);
		};

		constexpr f32 rotationA = 0;
		constexpr SVector2f expectedForwardA(0, -1);
		constexpr SVector2f expectedRightA(-1, 0);

		transformFunc.SetRotation(rotationA);

		CHECK(closeEnoughVector(expectedForwardA,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightA,		transformFunc.GetRightVector()));

		constexpr f32 rotationB = degreeToRadian(90.f);
		constexpr SVector2f expectedForwardB(-1, 0);
		constexpr SVector2f expectedRightB(0, 1);

		transformFunc.SetRotation(rotationB);
		
		CHECK(closeEnoughVector(expectedForwardB,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightB,		transformFunc.GetRightVector()));

		constexpr f32 rotationC = degreeToRadian(180.f);
		constexpr SVector2f expectedForwardC(0, 1);
		constexpr SVector2f expectedRightC(1, 0);

		transformFunc.SetRotation(rotationC);
		
		CHECK(closeEnoughVector(expectedForwardC,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightC,		transformFunc.GetRightVector()));

		constexpr f32 rotationD = degreeToRadian(270.f);
		constexpr SVector2f expectedForwardD(1, 0);
		constexpr SVector2f expectedRightD(0, -1);

		transformFunc.SetRotation(rotationD);

		CHECK(closeEnoughVector(expectedForwardD,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightD,		transformFunc.GetRightVector()));
	}

	// TODO(Kevin): GetMatrix does not match the values returned by GetForwardVector and GetRightVector.
	// This needs to be fixed ASAP, as it's an inconsistancy that is the result of undefined or undocumented spaces in the engine.
	/*
	SECTION("Get transform matrix")
	{
		constexpr auto closeEnoughMatrix = [closeEnough](const auto aValue, const auto aCompare)
		{
			for (u32 x = 0; x < decltype(aValue)::Columns; ++x)
				for (u32 y = 0; x < decltype(aValue)::Rows; ++y)
					if (!closeEnough(aValue[x][y], aCompare[x][y]))
						return false;
			
			return true;
		};

		constexpr auto positionA = SVector2f(0, 0);
		constexpr auto rotationA = f32(0);
		constexpr auto expectedA = SMatrix33f(
			-1, 0, 0,
			0, -1, 0,
			0, 0, 0);

		transformFunc.SetPosition(positionA);
		transformFunc.SetRotation(rotationA);

		CHECK(closeEnoughMatrix(expectedA, transformFunc.GetMatrix()));
	}
	*/
}
