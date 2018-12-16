#pragma once

#include <atomic>
#include <mutex>

template<typename TType>
class DoubleBuffer
{
public:
	DoubleBuffer() = default;

	template<typename TFunction>
	void WriteContent(const TFunction& aWriteFunction);

	const TType& ReadContent() const;

	bool SwapContent();
	template<typename TFunction>
	void ClearContent(const TFunction& aClearFunction);

private:
	std::mutex myLock;
	std::atomic_bool myHasContent;

	TType myContentA;
	TType myContentB;

	TType* myContentWrite = &myContentA;
	TType* myContentRead = &myContentB;
};

template<typename TType>
template<typename TFunction>
void DoubleBuffer<TType>::WriteContent(const TFunction& aWriteFunction)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	aWriteFunction(*myContentWrite);
	myHasContent = true;
}

template<typename TType>
const TType& DoubleBuffer<TType>::ReadContent() const
{
	return *myContentRead;
}

template<typename TType>
bool DoubleBuffer<TType>::SwapContent()
{
	if (!myHasContent)
	{
		return false;
	}
	std::lock_guard<decltype(myLock)> lock(myLock);
	std::swap(myContentWrite, myContentRead);
	myHasContent = false;
	return true;
}

template<typename TType>
template<typename TFunction>
void DoubleBuffer<TType>::ClearContent(const TFunction& aClearFunction)
{
	aClearFunction(*myContentRead);
}
