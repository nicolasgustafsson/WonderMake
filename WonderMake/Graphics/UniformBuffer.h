#pragma once
#include "WonderMakeUtility/Typedefs.h"

//this assumes that the buffer uses an optimal memory alignment
template< typename TBuffer>
class UniformBuffer
{
public:
	TBuffer& GetBuffer()
	{
		return myBuffer;
	}

	void Update() noexcept
	{
		SystemPtr<OpenGLFacade> openGL;
		openGL->BindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		openGL->UpdateBufferData(GL_UNIFORM_BUFFER, 0, sizeof(TBuffer), &myBuffer);
		openGL->BindBufferToSlot(GL_UNIFORM_BUFFER, myBufferBindIndex, myUniformBufferObject);
	}

protected:
	UniformBuffer(OpenGLFacade& aOpenGlFacade, const u32 aBufferBindIndex)
	{
		myBufferBindIndex = aBufferBindIndex;
		
		myUniformBufferObject = aOpenGlFacade.GenerateBuffer();
		aOpenGlFacade.BindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		aOpenGlFacade.AllocateBufferData(GL_UNIFORM_BUFFER, sizeof(TBuffer), &myBuffer, GL_STATIC_DRAW);

		aOpenGlFacade.BindBufferToSlot(GL_UNIFORM_BUFFER, myBufferBindIndex, myUniformBufferObject);
	}

	~UniformBuffer() noexcept
	{
		SystemPtr<OpenGLFacade> openGL;
		openGL->DeleteBuffer(myUniformBufferObject);
	}

private:
	TBuffer myBuffer;

	u32 myUniformBufferObject;

	u32 myBufferBindIndex;
};