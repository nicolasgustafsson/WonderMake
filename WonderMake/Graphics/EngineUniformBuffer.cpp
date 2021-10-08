#include "pch.h"
#include "EngineUniformBuffer.h"

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
