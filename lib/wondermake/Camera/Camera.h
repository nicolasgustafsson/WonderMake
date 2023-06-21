#pragma once

#include "wondermake-ui/EngineUniformBuffer.h"

#include "wondermake-utility/Matrix.h"
#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/Rotation.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/Vector.h"

#include <string>

class OpenGLFacade;

class Camera final
	: public NonCopyable
	, public NonMovable
{
public:
	Camera(
		OpenGLFacade&	aOpenGlFacade,
		std::string		aName);

	void BindBuffer();

	inline [[nodiscard]] const std::string&	GetName() const noexcept { return myName; }
	inline void								SetPosition(const SVector2f aPosition) noexcept { myPosition = aPosition; }
	inline [[nodiscard]] SVector2f			GetPosition() const noexcept { return myPosition; }
	inline void								SetRotation(const SRadianF32 aRotation) noexcept { myRotation = WmMath::Mod(aRotation, SRadianF32::Full()); }
	inline [[nodiscard]] SRadianF32			GetRotation() const noexcept { return myRotation; }
	inline void								SetScale(const f32 aScale) noexcept { myScale = aScale; }
	inline [[nodiscard]] f32				GetScale() const noexcept { return myScale; }
	
	inline [[nodiscard]] SMatrix33f			GetUnscaledViewMatrix() const noexcept
	{
		const auto rotationMatrix = SMatrix33f::CreateRotationZ(myRotation);
		SMatrix33f viewMatrix;

		viewMatrix = rotationMatrix * viewMatrix;

		viewMatrix.SetPosition(myPosition);

		return viewMatrix;
	}
	inline [[nodiscard]] SMatrix33f			GetViewMatrix() const noexcept
	{
		auto viewMatrix = GetUnscaledViewMatrix();

		viewMatrix[0][0] *= myScale;
		viewMatrix[0][1] *= myScale;
		viewMatrix[1][1] *= myScale;
		viewMatrix[1][0] *= myScale;

		return viewMatrix;
	}
	inline [[nodiscard]] SMatrix33f			GetInverseViewMatrix() const noexcept
	{
		auto viewMatrix = GetUnscaledViewMatrix();

		 // viewMatrix.FastInverse();

		viewMatrix[0][0] /= myScale;
		viewMatrix[0][1] /= myScale;
		viewMatrix[1][1] /= myScale;
		viewMatrix[1][0] /= myScale;

		return viewMatrix;
	}

private:
	std::string			myName;
	CameraUniformBuffer	myCameraBuffer;

	SVector2f			myPosition	= SVector2f::Zero();
	SRadianF32			myRotation	= 0.f;
	f32					myScale		= 1.0f;
};
