#include <catch2/catch.hpp>

#include "Functionalities/TransformFunctionality.h"

TEST_CASE("TransformFunctionality2D basic functionality", "[TransformFunctionality2D]")
{
	Object object;
	STransformComponent2D transformComp;

	TransformFunctionality2D::InjectDependencies(std::tie(transformComp));

	TransformFunctionality2D transformFunc;

	constexpr auto closeEnough = [](const auto aValue, const auto aCompare)
	{
		constexpr auto epsilon = static_cast<decltype(aValue)>(0.001);

		return aValue > aCompare - epsilon
			&& aValue < aCompare + epsilon;
	};
	constexpr auto closeEnoughVector = [closeEnough](const auto aValue, const auto aCompare)
	{
		return closeEnough(aValue.X, aCompare.X)
			&& closeEnough(aValue.Y, aCompare.Y);
	};
	constexpr auto closeEnoughMatrix = [closeEnough](const auto aValue, const auto aCompare)
	{
		for (u32 x = 0; x < decltype(aValue)::Columns; ++x)
			for (u32 y = 0; y < decltype(aValue)::Rows; ++y)
				if (!closeEnough(aValue[x][y], aCompare[x][y]))
					return false;

		return true;
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
		CHECK(transformFunc.GetRotation() == 0.f);

		constexpr f32 rotation = 1;

		transformFunc.SetRotation(rotation);

		CHECK(transformFunc.GetRotation() == rotation);
	}

	SECTION("Facing position")
	{
		constexpr SVector2f originA(0, 0);
		constexpr SVector2f targetA(0, -1);
		constexpr f32 expectedRotationA = -180.f;

		transformFunc.SetPosition(originA);

		transformFunc.FacePosition(targetA);

		CHECK(closeEnough(expectedRotationA, transformFunc.GetRotation()));

		constexpr SVector2f originB(3, 2);
		constexpr SVector2f targetB(3, 1);
		constexpr f32 expectedRotationB = -180.f;

		transformFunc.SetPosition(originB);

		transformFunc.FacePosition(targetB);

		CHECK(closeEnough(expectedRotationB, transformFunc.GetRotation()));

		constexpr SVector2f originC(2, 2);
		constexpr SVector2f targetC(-1, -1);
		constexpr f32 expectedRotationC = 135.f;

		transformFunc.SetPosition(originC);

		transformFunc.FacePosition(targetC);

		CHECK(closeEnough(expectedRotationC, transformFunc.GetRotation()));
	}

	SECTION("Facing direction")
	{
		constexpr SVector2f directionA(0, 1);
		constexpr f32 expectedRotationA = 0;

		transformFunc.FaceDirection(directionA);

		CHECK(closeEnough(expectedRotationA, transformFunc.GetRotation()));

		constexpr SVector2f directionB(2.43f, 0);
		constexpr f32 expectedRotationB = -90.f;

		transformFunc.FaceDirection(directionB);

		CHECK(closeEnough(expectedRotationB, transformFunc.GetRotation()));

		constexpr SVector2f directionC(-1, 1);
		constexpr f32 expectedRotationC = 45.f;

		transformFunc.FaceDirection(directionC);

		CHECK(closeEnough(expectedRotationC, transformFunc.GetRotation()));
	}

	SECTION("Get facing vectors")
	{
		constexpr f32 rotationA = 0;
		constexpr SVector2f expectedForwardA(0, 1);
		constexpr SVector2f expectedRightA(1, 0);

		transformFunc.SetRotation(rotationA);

		CHECK(closeEnoughVector(expectedForwardA,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightA,		transformFunc.GetRightVector()));

		constexpr f32 rotationB = 90.f;
		constexpr SVector2f expectedForwardB(-1, 0);
		constexpr SVector2f expectedRightB(0, 1);

		transformFunc.SetRotation(rotationB);
		
		CHECK(closeEnoughVector(expectedForwardB,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightB,		transformFunc.GetRightVector()));

		constexpr f32 rotationC = 180.f;
		constexpr SVector2f expectedForwardC(0, -1);
		constexpr SVector2f expectedRightC(-1, 0);

		transformFunc.SetRotation(rotationC);
		
		CHECK(closeEnoughVector(expectedForwardC,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightC,		transformFunc.GetRightVector()));

		constexpr f32 rotationD = 270.f;
		constexpr SVector2f expectedForwardD(1, 0);
		constexpr SVector2f expectedRightD(0, -1);

		transformFunc.SetRotation(rotationD);

		CHECK(closeEnoughVector(expectedForwardD,	transformFunc.GetForwardVector()));
		CHECK(closeEnoughVector(expectedRightD,		transformFunc.GetRightVector()));
	}

	SECTION("Get transform matrix")
	{
		constexpr auto positionA = SVector2f(0, 0);
		constexpr auto rotationA = SDegreeF32(0);
		constexpr auto expectedA = SMatrix33f(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1);

		transformFunc.SetPosition(positionA);
		transformFunc.SetRotation(rotationA);

		const auto resultA = transformFunc.GetMatrix();

		CHECK(closeEnoughMatrix(expectedA, resultA));
		CHECK(closeEnoughVector(transformFunc.GetForwardVector(),	resultA.GetForwardVector()));
		CHECK(closeEnoughVector(transformFunc.GetRightVector(),		resultA.GetRightVector()));

		constexpr auto positionB = SVector2f(5, 10);
		constexpr auto rotationB = SDegreeF32(0);
		constexpr auto expectedB = SMatrix33f(
			1, 0, 5,
			0, 1, 10,
			0, 0, 1);

		transformFunc.SetPosition(positionB);
		transformFunc.SetRotation(rotationB);

		const auto resultB = transformFunc.GetMatrix();

		CHECK(closeEnoughMatrix(expectedB, resultB));
		CHECK(closeEnoughVector(transformFunc.GetForwardVector(),	resultB.GetForwardVector()));
		CHECK(closeEnoughVector(transformFunc.GetRightVector(),		resultB.GetRightVector()));

		constexpr auto positionC = SVector2f(0, 0);
		constexpr auto rotationC = SDegreeF32(90);
		constexpr auto expectedC = SMatrix33f(
			0, -1, 0,
			1, 0, 0,
			0, 0, 1);

		transformFunc.SetPosition(positionC);
		transformFunc.SetRotation(rotationC);

		const auto resultC = transformFunc.GetMatrix();

		CHECK(closeEnoughMatrix(expectedC, resultC));
		CHECK(closeEnoughVector(transformFunc.GetForwardVector(),	resultC.GetForwardVector()));
		CHECK(closeEnoughVector(transformFunc.GetRightVector(),		resultC.GetRightVector()));

		constexpr auto positionD = SVector2f(-78, 12);
		constexpr auto rotationD = SDegreeF32(180);
		constexpr auto expectedD = SMatrix33f(
			-1, 0, -78,
			0, -1, 12,
			0, 0, 1);

		transformFunc.SetPosition(positionD);
		transformFunc.SetRotation(rotationD);

		const auto resultD = transformFunc.GetMatrix();

		CHECK(closeEnoughMatrix(expectedD, resultD));
		CHECK(closeEnoughVector(transformFunc.GetForwardVector(),	resultD.GetForwardVector()));
		CHECK(closeEnoughVector(transformFunc.GetRightVector(),		resultD.GetRightVector()));
	}
}
