#pragma once
#include "VertexAttributes.h"
#include "VertexBuffer.h"

template<EVertexAttribute TAttribute>
class SVertexAttributeContainer final
{
private:
	using ValueType = decltype(GetValueFromAttribute<TAttribute>());

public:
	void Resize(const u32 aSize)
	{
		myContainer.resize(aSize);
		myVertexBuffer.ResizeBuffer(aSize);
	}

	void Set(const u32 aIndex, const ValueType aValue)
	{
		myContainer[aIndex] = aValue;
		myIsDirty = true;
	}

	void Update()
	{
		myVertexBuffer.Bind();
		if (!myIsDirty)
			return;

		myVertexBuffer.SetData(myContainer);
		myIsDirty = false;
	}

	void Bind()
	{
		myVertexBuffer.Bind();
	}

private:
	VertexBuffer<ValueType> myVertexBuffer;
	std::vector<ValueType> myContainer;
	bool myIsDirty = false;
};

template<EVertexAttribute... TAttributes>
class VertexBufferArray final : NonCopyable
{
public:
	VertexBufferArray();
	VertexBufferArray(const u32 aSize);

	void Render();

	template<EVertexAttribute TAttribute>
	void Set(u32 VertexIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttributeValue)
	{
		std::get<SVertexAttributeContainer<TAttribute>>(myAttributeData).Set(VertexIndex, aAttributeValue);
	}

private:
	std::tuple<SVertexAttributeContainer<TAttributes>...> myAttributeData;

	u32 myVAO;
};

template<EVertexAttribute... TAttributes>
VertexBufferArray<TAttributes...>::VertexBufferArray()
{
	glGenVertexArrays(1, &myVAO);
}

template<EVertexAttribute... TAttributes>
VertexBufferArray<TAttributes...>::VertexBufferArray(const u32 aSize)
	:VertexBufferArray<TAttributes...>()
{
	(std::get<SVertexAttributeContainer<TAttributes>>(myAttributeData).Resize(aSize), ...);
}

template<EVertexAttribute... TAttributes>
void VertexBufferArray<TAttributes...>::Render()
{
	glBindVertexArray(myVAO);

	(std::get<SVertexAttributeContainer<TAttributes>>(myAttributeData).Update(), ...);
}

