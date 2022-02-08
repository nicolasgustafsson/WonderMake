#pragma once
#include <glad/glad.h>
#include "OpenGLFacade.h"

template <typename TVertexData>
class VertexBuffer : NonCopyable
{
public:
	VertexBuffer();
	VertexBuffer(const std::vector<TVertexData>& aData);

	void ResizeBuffer(const u32 aCount);

	template<class TContainer>
	void SetData(const TContainer& aContainer);
	
	void Bind(const u32 aIndex);

private:
	u32 myBufferHandle;
	u32 myVertexCount = 0;
};

template <typename TVertexData>
VertexBuffer<TVertexData>::VertexBuffer()
{
	SystemPtr<OpenGLFacade> openGL;
	myBufferHandle = openGL->GenerateBuffer();
}

template <typename TVertexData>
VertexBuffer<TVertexData>::VertexBuffer(const std::vector<TVertexData>& aData)
	: VertexBuffer()
{
	SetData(aData);
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::ResizeBuffer(const u32 aCount)
{
	SystemPtr<OpenGLFacade> openGL;
	myVertexCount = aCount;
	openGL->BindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	openGL->AllocateBufferData(GL_ARRAY_BUFFER, sizeof(TVertexData) * myVertexCount, nullptr, GL_DYNAMIC_DRAW);
}

template <typename TVertexData>
template<class TContainer>
void VertexBuffer<TVertexData>::SetData(const TContainer& aData)
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->BindBuffer(GL_ARRAY_BUFFER, myBufferHandle);

	if (myVertexCount != static_cast<u32>(aData.Count()))
		ResizeBuffer(static_cast<u32>(aData.Count()));

	openGL->UpdateBufferData(GL_ARRAY_BUFFER, 0, sizeof(TVertexData) * myVertexCount, &aData[0]);
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::Bind(const u32 aIndex)
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->BindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	
	openGL->DefineVertexAttributeData(aIndex, sizeof(TVertexData) / 4, GL_FLOAT, false, 0, nullptr);

	openGL->EnableVertexAttributeArray(aIndex);
}

