#include "pch.h"
#include "EngineUniformBuffer.h"

REGISTER_SYSTEM(EngineUniformBuffer);

EngineUniformBuffer::EngineUniformBuffer()
	: UniformBuffer<SEngineUniformBufferData>(0)
{

}

CameraUniformBuffer::CameraUniformBuffer()
	: UniformBuffer<SCameraUniformBufferData>(1)
{

}

DisplayUniformBuffer::DisplayUniformBuffer()
	: UniformBuffer<SDisplayUniformBufferData>(2)
{

}
