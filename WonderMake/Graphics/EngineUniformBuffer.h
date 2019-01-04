#pragma once
#include "UniformBuffer.h"
#include "Utilities/Singleton.h"

struct SEngineUniformBufferData
{
	//MAKE SURE THESE ARE PROPERLY ALIGNED

	float Time = 0;
};

class EngineUniformBuffer : public UniformBuffer<0, SEngineUniformBufferData>, public Singleton<EngineUniformBuffer>
{
public:
};

