#pragma once

#include "Graphics/OpenGLFacade.h"

#include "wondermake-utility/Typedefs.h"

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
		myOpenGLFacade.BindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		myOpenGLFacade.UpdateBufferData(GL_UNIFORM_BUFFER, 0, sizeof(TBuffer), &myBuffer);
		myOpenGLFacade.BindBufferToSlot(GL_UNIFORM_BUFFER, myBufferBindIndex, myUniformBufferObject);
	}

protected:
	UniformBuffer(OpenGLFacade& aOpenGlFacade, const u32 aBufferBindIndex)
		: myOpenGLFacade(aOpenGlFacade)
	{
		myBufferBindIndex = aBufferBindIndex;
		
		myUniformBufferObject = myOpenGLFacade.GenerateBuffer();
		myOpenGLFacade.BindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		myOpenGLFacade.AllocateBufferData(GL_UNIFORM_BUFFER, sizeof(TBuffer), &myBuffer, GL_STATIC_DRAW);

		myOpenGLFacade.BindBufferToSlot(GL_UNIFORM_BUFFER, myBufferBindIndex, myUniformBufferObject);
	}

	~UniformBuffer() noexcept
	{
		myOpenGLFacade.DeleteBuffer(myUniformBufferObject);
	}

private:
	OpenGLFacade& myOpenGLFacade;

	TBuffer myBuffer;

	u32 myUniformBufferObject;

	u32 myBufferBindIndex;
};