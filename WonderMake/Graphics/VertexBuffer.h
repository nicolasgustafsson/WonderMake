#pragma once
#include <glad/glad.h>

template <typename TVertexData, u32 Count = 1>
class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(const std::array<TVertexData, Count>& Data);

	void SetData(const std::array<TVertexData, Count>& Data);

	void Bind();

private:
	u32 myBufferHandle;
};

template <typename TVertexData, u32 Count>
VertexBuffer<TVertexData, Count>::VertexBuffer(const std::array<TVertexData, Count>& Data)
	: VertexBuffer()
{
	SetData(Data);
}

template <typename TVertexData, u32 Count>
void VertexBuffer<TVertexData, Count>::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glVertexAttribPointer(0, Count, GL_FLOAT, GL_FALSE, sizeof(TVertexData), (void*)0);
	auto error = glGetError();
	glEnableVertexAttribArray(0);
}

template <typename TVertexData, u32 Count>
void VertexBuffer<TVertexData, Count>::SetData(const std::array<TVertexData, Count>& Data)
{
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TVertexData) * Count, Data.data());
}

template <typename TVertexData, u32 Count>
VertexBuffer<TVertexData, Count>::VertexBuffer()
{
	glGenBuffers(1, &myBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TVertexData) * Count, nullptr, GL_STATIC_DRAW);
}
