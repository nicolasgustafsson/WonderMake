#include "wondermake-ui/EngineUniformBuffer.h"

#include "wondermake-ui/OpenGLFacade.h"
#include "wondermake-ui/Window.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(EngineUniformBuffer);

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
