#include "stdafx.h"
#include "DispatchableBuffer.h"

DoubleBuffer<std::unordered_set<DispatchableBufferBase*>> DispatchableBufferBase::UpdatedBuffers[ThreadCount];

DispatchableBufferBase::DispatchableBufferBase(const size_t aTypeHash)
	: myTypeHash(aTypeHash)
{}

bool DispatchableBufferBase::UpdateBuffers(const EThreadId aThreadId)
{
	auto& buffers = UpdatedBuffers[static_cast<u32>(aThreadId)];
	buffers.ClearContent([](auto& aSet) { aSet.clear(); });
	return buffers.SwapContent();
}

const std::unordered_set<DispatchableBufferBase*>& DispatchableBufferBase::GetUpdatedBuffers(const EThreadId aThreadId)
{
	return UpdatedBuffers[static_cast<u32>(aThreadId)].ReadContent();
}

void DispatchableBufferBase::BufferUpdated(const EThreadId aThreadId)
{
	if (myIsUpdated)
	{
		return;
	}
	UpdatedBuffers[static_cast<u32>(aThreadId)].WriteContent([this](auto& aSet) { aSet.insert(this); });
	myIsUpdated = true;
}
