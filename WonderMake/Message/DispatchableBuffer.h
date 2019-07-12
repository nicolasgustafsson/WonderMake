#pragma once
#include "Typedefs.h"

#include "Threads/DoubleBuffer.h"
#include "Threads/RoutineIds.h"

#include "Utilities/RestrictTypes.h"
#include "Utilities/Utility.h"

#include <mutex>
#include <unordered_set>
#include <vector>

class Dispatchable;

class DispatchableBufferBase
	: private NonCopyable
{
public:
	virtual ~DispatchableBufferBase() = default;
	
	inline size_t GetTypeHash() const noexcept;

	static bool UpdateBuffers(const ERoutineId aRoutineId) noexcept;
	static void GetDispatchableList(const ERoutineId aRoutineId, std::vector<const Dispatchable*>& aOutList) noexcept;

protected:
	DispatchableBufferBase(const size_t aTypeHash) noexcept;

	void AddDispatchable(const ERoutineId aRoutineId, const size_t aIndex) noexcept;

	virtual const Dispatchable& GetDispatchableAt(const size_t aIndex) const noexcept = 0;
	virtual void Clear() noexcept = 0;
	virtual void Swap() noexcept = 0;

	static std::mutex myTemporaryLock;

private:
	struct DispatchableLocation
	{
		DispatchableBufferBase* myBuffer = nullptr;
		size_t myIndex = 0;
	};

	struct DispatchableContainer
	{
		std::unordered_set<DispatchableBufferBase*> myUpdatedBuffers;
		std::vector<DispatchableLocation> myMessages;
	};

	static DoubleBuffer<DispatchableContainer> UpdatedBuffers[RoutineCount];
	static std::mutex myLock;
	const size_t myTypeHash;
};

size_t DispatchableBufferBase::GetTypeHash() const noexcept
{
	return myTypeHash;
}

template<typename TDispatchType>
class DispatchableBuffer final
	: public DispatchableBufferBase
{
public:
	inline DispatchableBuffer() noexcept;

	static void Dispatch(const TDispatchType& aDispatchable) noexcept;
	static void Dispatch(TDispatchType&& aDispatchable) noexcept;
	static void Dispatch(const TDispatchType& aDispatchable, const ERoutineId aRoutineId) noexcept;
	static void Dispatch(TDispatchType&& aDispatchable, const ERoutineId aRoutineId) noexcept;

private:
	virtual const Dispatchable& GetDispatchableAt(const size_t aIndex) const noexcept override;
	virtual void Clear() noexcept override;
	virtual void Swap() noexcept override;

	static DispatchableBuffer<TDispatchType> Instance[RoutineCount];
	DoubleBuffer<std::vector<TDispatchType>> myDoubleBuffer;
};

template<typename TDispatchType>
DispatchableBuffer<TDispatchType> DispatchableBuffer<TDispatchType>::Instance[RoutineCount];

template<typename TDispatchType>
DispatchableBuffer<TDispatchType>::DispatchableBuffer() noexcept
	: DispatchableBufferBase(TDispatchType::GetTypeHash())
{}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(const TDispatchType& aDispatchable) noexcept
{
	for (u32 i = 0; i < RoutineCount; ++i)
	{
		Dispatch(aDispatchable, static_cast<ERoutineId>(i));
	}
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(TDispatchType&& aDispatchable) noexcept
{
	for (u32 i = 0; i < RoutineCount - 1; ++i)
	{
		Dispatch(aDispatchable, static_cast<ERoutineId>(i));
	}
	Dispatch(std::move(aDispatchable), static_cast<ERoutineId>(RoutineCount - 1));
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(const TDispatchType& aDispatchable, const ERoutineId aRoutineId) noexcept
{
	std::lock_guard<decltype(myTemporaryLock)> _lock(myTemporaryLock);
	size_t index = 0;
	Instance[static_cast<u32>(aRoutineId)].myDoubleBuffer.WriteContent([&aDispatchable, &index](auto& aList)
	{
		index = aList.size();
		aList.emplace_back(aDispatchable);
	});
	Instance[static_cast<u32>(aRoutineId)].AddDispatchable(aRoutineId, index);
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Dispatch(TDispatchType&& aDispatchable, const ERoutineId aRoutineId) noexcept
{
	std::lock_guard<decltype(myTemporaryLock)> _lock(myTemporaryLock);
	size_t index = 0;
	Instance[static_cast<u32>(aRoutineId)].myDoubleBuffer.WriteContent([&aDispatchable, &index](auto& aList)
	{
		index = aList.size();
		aList.emplace_back(std::move(aDispatchable));
	});
	Instance[static_cast<u32>(aRoutineId)].AddDispatchable(aRoutineId, index);
}

template<typename TDispatchType>
const Dispatchable& DispatchableBuffer<TDispatchType>::GetDispatchableAt(const size_t aIndex) const noexcept
{
	return myDoubleBuffer.ReadContent()[aIndex];
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Clear() noexcept
{
	myDoubleBuffer.ClearContent([](auto& aList) { aList.clear(); });
}

template<typename TDispatchType>
void DispatchableBuffer<TDispatchType>::Swap() noexcept
{
	myDoubleBuffer.SwapContent();
}
