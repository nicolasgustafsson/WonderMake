#pragma once
#include "UniformBuffer.h"
#include "Utilities/Matrix.h"
#include "System/System.h"
#include "Message/MessageSubscriber.h"
#include "Utilities/Debugging/Debugged.h"

struct SPaddedMatrix33
{
	SPaddedMatrix33& operator=(const SMatrix33f& aMatrix) noexcept
	{
		Data[0] = aMatrix[0][0];
		Data[1] = aMatrix[1][0];
		Data[2] = aMatrix[2][0];
		Data[3] = 0.f;

		Data[4] = aMatrix[0][1];
		Data[5] = aMatrix[1][1];
		Data[6] = aMatrix[2][1];
		Data[7] = 0.f;

		Data[8] = aMatrix[0][2];
		Data[9] = aMatrix[1][2];
		Data[10] = aMatrix[2][2];
		Data[11] = 0.f;

		return *this;
	}

	std::string ToString() const
	{
		std::string DebugString;
		for (u32 y = 0; y < 3; y++)
		{
			DebugString += "[";
			for (u32 x = 0; x < 4; x++)
			{
				DebugString += std::to_string(Data[y * 4 + x]);
				DebugString += ",";
			}
			DebugString += "]\n";
		}

		return DebugString;
	}

	float Data[4 * 3];
};

struct SEngineUniformBufferData
{
	//MAKE SURE THESE ARE PROPERLY ALIGNED

	float Time = 0;
};

struct SCameraUniformBufferData
{
	SPaddedMatrix33 ViewMatrix;

	SVector2f CameraPosition;
};

struct SDisplayUniformBufferData
{
	SPaddedMatrix33 ProjectionMatrix;
	SPaddedMatrix33 ViewProjectionMatrix;
	SVector2f Resolution;
	SVector2f InverseResolution;
};

class Window;

class EngineUniformBuffer 
	: public UniformBuffer<SEngineUniformBufferData>
	, public System<
		Policy::Set<
			PAdd<Window, PWrite>,
			PAdd<OpenGLFacade, PWrite>>>
	
{
public:
	EngineUniformBuffer();

};


class CameraUniformBuffer
	: public UniformBuffer<SCameraUniformBufferData>

{
public:
	CameraUniformBuffer(OpenGLFacade& aOpenGlFacade);

private:
};

class DisplayUniformBuffer
	: public UniformBuffer<SDisplayUniformBufferData>

{
public:
	DisplayUniformBuffer(OpenGLFacade& aOpenGlFacade);

private:
};
