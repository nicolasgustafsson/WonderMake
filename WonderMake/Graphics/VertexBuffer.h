#pragma once
#include <glad/glad.h>

template <typename TVertexData>
class VertexBuffer : NonCopyable
{
public:
	VertexBuffer();
	VertexBuffer(const std::vector<TVertexData>& aData);

	void ResizeBuffer(const u32 aCount);
	void SetData(const std::vector<TVertexData>& aData);

	void Bind(const u32 aIndex);

private:
	u32 myBufferHandle;
	u32 myVertexCount = 0;
};

template <typename TVertexData>
VertexBuffer<TVertexData>::VertexBuffer()
{
	glGenBuffers(1, &myBufferHandle);
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
	myVertexCount = aCount;
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TVertexData) * myVertexCount, nullptr, GL_DYNAMIC_DRAW);
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::SetData(const std::vector<TVertexData>& aData)
{
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);

	if (myVertexCount != static_cast<u32>(aData.size()))
		ResizeBuffer(static_cast<u32>(aData.size()));

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TVertexData) * myVertexCount, aData.data());
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::Bind(const u32 aIndex)
{
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glVertexAttribPointer(aIndex, sizeof(TVertexData) / 4, GL_FLOAT, GL_FALSE, sizeof(TVertexData), (void*)0);
	glEnableVertexAttribArray(aIndex);
}

