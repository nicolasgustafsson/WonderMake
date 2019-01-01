#pragma once

#include <atomic>
#include <mutex>

template<typename TType>
class DoubleBuffer
{
public:
	template<typename TFunction>
	void WriteContent(const TFunction& aWriteFunction);

	const TType& ReadContent() const;

	void SwapContent();
	template<typename TFunction>
	void ClearContent(const TFunction& aClearFunction);

private:
	TType myContentA;
	TType myContentB;

protected:
	TType* myContentWrite = &myContentA;
	TType* myContentRead = &myContentB;
};

template<typename TType>
class ThreadSafeDoubleBuffer
	: protected DoubleBuffer<TType>
{
public:
	template<typename TFunction>
	void WriteContent(const TFunction& aWriteFunction);

	const TType& ReadContent() const;

	bool SwapContent();
	template<typename TFunction>
	void ClearContent(const TFunction& aClearFunction);

private:
	std::mutex myLock;
	std::atomic_bool myHasContent;
};

template<typename TType>
template<typename TFunction>
void DoubleBuffer<TType>::WriteContent(const TFunction& aWriteFunction)
{
	aWriteFunction(*myContentWrite);
}

template<typename TType>
const TType& DoubleBuffer<TType>::ReadContent() const
{
	return *myContentRead;
}

template<typename TType>
void DoubleBuffer<TType>::SwapContent()
{
	std::swap(myContentWrite, myContentRead);
}

template<typename TType>
template<typename TFunction>
void DoubleBuffer<TType>::ClearContent(const TFunction& aClearFunction)
{
	aClearFunction(*myContentRead);
}

template<typename TType>
template<typename TFunction>
void ThreadSafeDoubleBuffer<TType>::WriteContent(const TFunction& aWriteFunction)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	DoubleBuffer<TType>::WriteContent(aWriteFunction);
	myHasContent = true;
}

template<typename TType>
const TType& ThreadSafeDoubleBuffer<TType>::ReadContent() const
{
	return DoubleBuffer<TType>::ReadContent();
}

template<typename TType>
bool ThreadSafeDoubleBuffer<TType>::SwapContent()
{
	if (!myHasContent)
	{
		return false;
	}
	std::lock_guard<decltype(myLock)> lock(myLock);
	DoubleBuffer<TType>::SwapContent();
	myHasContent = false;
	return true;
}

template<typename TType>
template<typename TFunction>
void ThreadSafeDoubleBuffer<TType>::ClearContent(const TFunction& aClearFunction)
{
	DoubleBuffer<TType>::ClearContent(aClearFunction);
}