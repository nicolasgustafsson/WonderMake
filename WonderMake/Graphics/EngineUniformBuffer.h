#pragma once
#include "UniformBuffer.h"
#include "Utilities/Matrix33.h"
#include "System/System.h"

struct SPaddedMatrix33
{
	SPaddedMatrix33& operator=(const SMatrix33f& aMatrix)
	{
		Data[0] = aMatrix.m11;
		Data[1] = aMatrix.m12;
		Data[2] = aMatrix.m13;
		Data[3] = 0.f;// aMatrix.m11;

		Data[4] = aMatrix.m21;
		Data[5] = aMatrix.m22;
		Data[6] = aMatrix.m23;
		Data[7] = 0.f;

		Data[8] = aMatrix.m31;
		Data[9] = aMatrix.m32;
		Data[10] = aMatrix.m33;
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

	SPaddedMatrix33 ProjectionMatrix;
	SPaddedMatrix33 ViewMatrix;
	SPaddedMatrix33 ViewProjectionMatrix;

	float Time = 0;
};

class EngineUniformBuffer 
	: public UniformBuffer<SEngineUniformBufferData>
	, public System
	
{
public:
	EngineUniformBuffer();

	void Debug();
};

