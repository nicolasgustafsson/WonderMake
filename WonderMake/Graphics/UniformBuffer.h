#pragma once
#include "Typedefs.h"

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
	}

protected:
	UniformBuffer(const u32 aBufferBindIndex)
	{
		SystemPtr<OpenGLFacade> openGL;
		
		myUniformBufferObject = openGL->GenerateBuffer();
		openGL->BindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		openGL->AllocateBufferData(GL_UNIFORM_BUFFER, sizeof(TBuffer), &myBuffer, GL_STATIC_DRAW);

		openGL->BindBufferToSlot(GL_UNIFORM_BUFFER, aBufferBindIndex, myUniformBufferObject);
	}

	~UniformBuffer() noexcept
	{
		SystemPtr<OpenGLFacade> openGL;
		openGL->DeleteBuffer(myUniformBufferObject);
	}

private:
	TBuffer myBuffer;

	u32 myUniformBufferObject;
};