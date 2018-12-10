#pragma once
#include <glad/glad.h>

template <typename TVertexData>
class VertexBuffer : NonCopyable
{
public:
	VertexBuffer();
	VertexBuffer(const std::vector<TVertexData>& Data);

	void ResizeBuffer(const u32 Count);
	void SetData(const std::vector<TVertexData>& Data);

	void Bind();

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
VertexBuffer<TVertexData>::VertexBuffer(const std::vector<TVertexData>& Data)
	: VertexBuffer()
{
	SetData(Data);
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::ResizeBuffer(const u32 Count)
{
	myVertexCount = Count;
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TVertexData) * myVertexCount, nullptr, GL_STATIC_DRAW);
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::SetData(const std::vector<TVertexData>& Data)
{
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);

	if (myVertexCount != Data.size())
		ResizeBuffer(Data.size());

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TVertexData) * myVertexCount, Data.data());
}

template <typename TVertexData>
void VertexBuffer<TVertexData>::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glVertexAttribPointer(0, sizeof(TVertexData) / 4, GL_FLOAT, GL_FALSE, sizeof(TVertexData), (void*)0);
	glEnableVertexAttribArray(0);
}

