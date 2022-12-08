#pragma once
#include "Components/TransformComponent.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-base/Functionality.h"

#include "wondermake-utility/Matrix.h"

template<typename TRepVector, typename TRepRotation>
class _TransformFunctionality2D
	: public Functionality<
		Policy::Set<
			PAdd<STransformComponent2D, PWrite>>,
		STrait::Set<
			STWonderMake>>
{
public:
	using RepVector		= TRepVector;
	using RepPosition	= TRepVector;
	using RepRotation	= TRepRotation;

	void SetPosition(const RepPosition aPosition) noexcept
	{
		Get<STransformComponent2D>().Position = aPosition;
	}
	[[nodiscard]] RepPosition GetPosition() const noexcept
	{
		return Get<STransformComponent2D>().Position;
	}

	void FacePosition(const RepPosition aPosition) noexcept
	{
		const auto deltaPosition = aPosition - GetPosition();

		FaceDirection(deltaPosition);
	}
	void FaceDirection(const RepVector aDirection) noexcept
	{
		SetRotation(aDirection.GetAngle<RepRotation>(RepVector(0, 1)));
	}

	void SetRotation(const RepRotation aRotation) noexcept
	{
		Get<STransformComponent2D>().Rotation = aRotation;
	}
	[[nodiscard]] RepRotation GetRotation() const noexcept
	{
		return Get<STransformComponent2D>().Rotation;
	}

	[[nodiscard]] RepVector GetForwardVector() const noexcept
	{
		RepVector retVec(0, 1);

		retVec.RotateCounterClockwise(GetRotation());

		return retVec;
	}
	[[nodiscard]] RepVector GetRightVector() const noexcept
	{
		return GetForwardVector().GetPerpendicularClockWise();
	}

	void Move(const RepVector aMovement) noexcept
	{
		Get<STransformComponent2D>().Position += aMovement;
	}

	[[nodiscard]] SMatrix33f GetMatrix() const noexcept
	{
		SMatrix33f matrix;

		matrix.SetPosition(GetPosition());
		matrix.SetRotation2D(GetRotation());

		return matrix;
	}

	void Inspect()
	{
		auto& transform = Get<STransformComponent2D>();

		ImGui::Text("X: %f", transform.Position.X);
		ImGui::Text("Y: %f", transform.Position.Y);
		ImGui::Text("R: %f", transform.Rotation.Rotation);
	}
};

using TransformFunctionality2D = _TransformFunctionality2D<STransformComponent2D::RepPosition, STransformComponent2D::RepRotation>;
