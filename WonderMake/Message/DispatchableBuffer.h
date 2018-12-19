#pragma once

#include "Typedefs.h"

#include "Threads/DoubleBuffer.h"
#include "Threads/RoutineIds.h"

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
	virtual const std::vector<const Dispatchable*>& GetList() const = 0;

	static bool UpdateBuffers(const ERoutineId aRoutineId);
	static const std::unordered_set<DispatchableBufferBase*>& GetUpdatedBuffers(const ERoutineId aRoutineId);

protected:
	DispatchableBufferBase(const size_t aTypeHash);

	void BufferUpdated(const ERoutineId aRoutineId);

	std::atomic_bool myIsUpdated = false;

private:
	static DoubleBuffer<std::unordered_set<DispatchableBufferBase*>> UpdatedBuffers[RoutineCount];
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
	static void Dispatch(const TDispatchType& aDispatchable, const ERoutineId aRoutineId);
	static void Dispatch(TDispatchType&& aDispatchable, const ERoutineId aRoutineId);

	virtual void UpdateList() override;
	virtual const std::vector<const Dispatchable*>& GetList() const override;

private:
	static DispatchableBuffer<TDispatchType> Instance[RoutineCount];
	DoubleBuffer<std::vector<TDispatchType>> myDoubleBuffer;
	std::vector<const Dispatchable*> myDispatchableList;
};

template<typename TDispatchType>
DispatchableBuffer<TDispatchType> DispatchableBuffer<TDispatchType>::Instance[RoutineCount];

template<typename TDispatchType>
DispatchableBuffer<TDispatchType>::DispatchableBuffer()
	: DispatchableBufferBase(TDispatchType::GetTypeHash())
{}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(const TDispatchType& aDispatchable)
{
	for (u32 i = 0; i < RoutineCount; ++i)
	{
		Dispatch(aDispatchable, static_cast<ERoutineId>(i));
	}
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(TDispatchType&& aDispatchable)
{
	for (u32 i = 0; i < RoutineCount - 1; ++i)
	{
		Dispatch(aDispatchable, static_cast<ERoutineId>(i));
	}
	Dispatch(std::move(aDispatchable), static_cast<ERoutineId>(RoutineCount - 1));
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(const TDispatchType& aDispatchable, const ERoutineId aRoutineId)
{
	Instance[static_cast<u32>(aRoutineId)].myDoubleBuffer.WriteContent([&aDispatchable](auto& aList) { aList.emplace_back(aDispatchable); });
	Instance[static_cast<u32>(aRoutineId)].BufferUpdated(aRoutineId);
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(TDispatchType&& aDispatchable, const ERoutineId aRoutineId)
{
	Instance[static_cast<u32>(aRoutineId)].myDoubleBuffer.WriteContent([&aDispatchable](auto& aList) { aList.emplace_back(std::move(aDispatchable)); });
	Instance[static_cast<u32>(aRoutineId)].BufferUpdated(aRoutineId);
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::UpdateList()
{
	myIsUpdated = false;
	myDispatchableList.clear();
	myDoubleBuffer.ClearContent([](auto& aList) { aList.clear(); });
	myDoubleBuffer.SwapContent();
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
}

template<typename TDispatchType>
const std::vector<const Dispatchable*>& DispatchableBuffer<TDispatchType>::GetList() const
{
	return myDispatchableList;
}
