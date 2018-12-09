#pragma once

#include <atomic>
#include <mutex>
#include <vector>

template<typename T>
class DoubleBuffer
{
public:
	DoubleBuffer();

	void Add(const T& Object);
	void Add(T&& Object);

	const std::vector<T>& GetReadBuffer() const;

	bool ClearAndSwap();

private:
	std::mutex myLock;
	std::atomic_bool myHasContent;

	std::vector<T> myBufferA;
	std::vector<T> myBufferB;

	std::vector<T>* myWriteBuffer;
	std::vector<T>* myReadBuffer;
};

template<typename T>
DoubleBuffer<T>::DoubleBuffer()
	: myWriteBuffer(&myBufferA)
	, myReadBuffer(&myBufferB)
{}

template<typename T>
void DoubleBuffer<T>::Add(const T& Object)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	if (myWriteBuffer->empty())
	{
		myHasContent = true;
	}
	myWriteBuffer->emplace_back(Object);
}

template<typename T>
void DoubleBuffer<T>::Add(T&& Object)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	if (myWriteBuffer->empty())
	{
		myHasContent = true;
	}
	myWriteBuffer->emplace_back(std::move(Object));
}

template<typename T>
const std::vector<T>& DoubleBuffer<T>::GetReadBuffer() const
{
	return *myReadBuffer;
}

template<typename T>
bool DoubleBuffer<T>::ClearAndSwap()
{
	myReadBuffer->clear();
	if (!myHasContent)
	{
		return false;
	}
	std::lock_guard<decltype(myLock)> lock(myLock);
	std::swap(myWriteBuffer, myReadBuffer);
	myHasContent = false;
	return true;
}
