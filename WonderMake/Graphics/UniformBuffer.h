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

	void Update()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TBuffer), &myBuffer);
	}

protected:
	UniformBuffer(const u32 aBufferBindIndex)
	{
		glGenBuffers(1, &myUniformBufferObject);
		glBindBuffer(GL_UNIFORM_BUFFER, myUniformBufferObject);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(TBuffer), NULL, GL_STATIC_DRAW);

		glBindBufferBase(GL_UNIFORM_BUFFER, aBufferBindIndex, myUniformBufferObject);
	}

	~UniformBuffer()
	{
		glDeleteBuffers(1, &myUniformBufferObject);
	}

private:
	TBuffer myBuffer;

	u32 myUniformBufferObject;
};