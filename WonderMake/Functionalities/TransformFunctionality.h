#pragma once
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"

#include "Utilities/Matrix.h"

#include "Imgui/ImguiInclude.h"

template<typename TRepVector, typename TRepRotation>
class _TransformFunctionality2D
	: public Functionality<
		Policy::Set<
			PAdd<STransformComponent2D, PWrite>>>
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

	void Rotate(const RepRotation aRotation) noexcept
	{
		Get<STransformComponent2D>().Rotation += aRotation;
	}

	void RotateTowards(const SVector2f aPosition, const RepRotation aSpeed, const f32 aDeltaTime) noexcept
	{
		const auto deltaPosition = aPosition - GetPosition();
		const auto finalRotation = deltaPosition.GetAngle<RepRotation>(RepVector(0, 1));

		auto deltaRotation = finalRotation - Get<STransformComponent2D>().Rotation;

		while (deltaRotation > SDegreeF32(180.f))
			deltaRotation -= SDegreeF32(360.f);

		while (deltaRotation < SDegreeF32(-180.f))
			deltaRotation += SDegreeF32(360.f);

		const auto sign = WmMath::Sign(deltaRotation);
		const auto rotationAmount = (WmMath::Abs(deltaRotation) < 1.0f ? deltaRotation : sign) * aSpeed * aDeltaTime;

		//dont overcompensate
		if (WmMath::Abs(deltaRotation) < WmMath::Abs(rotationAmount))
			Get<STransformComponent2D>().Rotation += deltaRotation;
		else
			Get<STransformComponent2D>().Rotation += rotationAmount;
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
