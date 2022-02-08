#include "pch.h"
#include "EngineUniformBuffer.h"

REGISTER_SYSTEM(EngineUniformBuffer);

EngineUniformBuffer::EngineUniformBuffer()
	: UniformBuffer<SEngineUniformBufferData>(Get<OpenGLFacade>(), 0)
{

}

CameraUniformBuffer::CameraUniformBuffer(OpenGLFacade& aOpenGlFacade)
	: UniformBuffer<SCameraUniformBufferData>(aOpenGlFacade, 1)
{

}

DisplayUniformBuffer::DisplayUniformBuffer(OpenGLFacade& aOpenGlFacade)
	: UniformBuffer<SDisplayUniformBufferData>(aOpenGlFacade, 2)
{

}
