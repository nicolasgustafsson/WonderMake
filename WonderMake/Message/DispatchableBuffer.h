#pragma once

#include "Typedefs.h"

#include "Threads/DoubleBuffer.h"
#include "Threads/ThreadIds.h"

#include "Utilities/Singleton.h"
#include "Utilities/RestrictTypes.h"
#include "Utilities/Utility.h"

#include <atomic>
#include <unordered_set>
#include <vector>

class Dispatchable;

class DispatchableBufferBase
	: private NonCopyable
{
public:
	virtual ~DispatchableBufferBase() = default;
	
	inline size_t GetTypeHash() const;

	virtual void UpdateList() = 0;
	virtual const std::vector<const Dispatchable*>& GetList() = 0;

	static bool UpdateBuffers(const EThreadId aThreadId);
	static const std::unordered_set<DispatchableBufferBase*>& GetUpdatedBuffers(const EThreadId aThreadId);

protected:
	DispatchableBufferBase(const size_t aTypeHash);

	void BufferUpdated(const EThreadId aThreadId);

	std::atomic_bool myIsUpdated = false;

private:
	static DoubleBuffer<std::unordered_set<DispatchableBufferBase*>> UpdatedBuffers[ThreadCount];
	const size_t myTypeHash;
};

size_t DispatchableBufferBase::GetTypeHash() const
{
	return myTypeHash;
}

template<typename TDispatchType>
class DispatchableBuffer final
	: public DispatchableBufferBase
{
public:
	inline DispatchableBuffer();

	static void Dispatch(const TDispatchType& aDispatchable);
	static void Dispatch(TDispatchType&& aDispatchable);
	static void Dispatch(const TDispatchType& aDispatchable, const EThreadId aThreadId);
	static void Dispatch(TDispatchType&& aDispatchable, const EThreadId aThreadId);

	virtual void UpdateList() override;
	virtual const std::vector<const Dispatchable*>& GetList() override;

private:
	static DispatchableBuffer<TDispatchType> Instance[ThreadCount];
	DoubleBuffer<std::vector<TDispatchType>> myDoubleBuffer;
	std::vector<const Dispatchable*> myDispatchableList;
};

template<typename TDispatchType>
DispatchableBuffer<TDispatchType> DispatchableBuffer<TDispatchType>::Instance[ThreadCount];

template<typename TDispatchType>
DispatchableBuffer<TDispatchType>::DispatchableBuffer()
	: DispatchableBufferBase(TDispatchType::GetTypeHash())
{}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(const TDispatchType& aDispatchable)
{
	for (u32 i = 0; i < ThreadCount; ++i)
	{
		Dispatch(aDispatchable, static_cast<EThreadId>(i));
	}
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(TDispatchType&& aDispatchable)
{
	for (u32 i = 0; i < ThreadCount - 1; ++i)
	{
		Dispatch(aDispatchable, static_cast<EThreadId>(i));
	}
	Dispatch(std::move(aDispatchable), static_cast<EThreadId>(ThreadCount - 1));
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(const TDispatchType& aDispatchable, const EThreadId aThreadId)
{
	Instance[static_cast<u32>(aThreadId)].myDoubleBuffer.WriteContent([&aDispatchable](auto& aList) { aList.emplace_back(aDispatchable); });
	Instance[static_cast<u32>(aThreadId)].BufferUpdated(aThreadId);
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(TDispatchType&& aDispatchable, const EThreadId aThreadId)
{
	Instance[static_cast<u32>(aThreadId)].myDoubleBuffer.WriteContent([&aDispatchable](auto& aList) { aList.emplace_back(std::move(aDispatchable)); });
	Instance[static_cast<u32>(aThreadId)].BufferUpdated(aThreadId);
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::UpdateList()
{
	myIsUpdated = false;
	myDispatchableList.clear();
	myDoubleBuffer.ClearContent([](auto& aList) { aList.clear(); });
	myDoubleBuffer.SwapContent();
}

template<typename TDispatchType>
const std::vector<const Dispatchable*>& DispatchableBuffer<TDispatchType>::GetList()
{
	const auto& list = myDoubleBuffer.ReadContent();
	if (myDispatchableList.capacity() < list.size())
	{
		myDispatchableList.reserve(list.size());
	}
	myDispatchableList.resize(list.size());
	for (auto[i, dispatchable] : Utility::Enumerate(list))
	{
		myDispatchableList[i] = &dispatchable;
	}
	return myDispatchableList;
}
