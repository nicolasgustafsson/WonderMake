#include "stdafx.h"
#include "DispatchableBuffer.h"

DoubleBuffer<std::unordered_set<DispatchableBufferBase*>> DispatchableBufferBase::UpdatedBuffers[RoutineCount];

DispatchableBufferBase::DispatchableBufferBase(const size_t aTypeHash)
	: myTypeHash(aTypeHash)
{}

bool DispatchableBufferBase::UpdateBuffers(const ERoutineId aRoutineId)
{
	auto& buffers = UpdatedBuffers[static_cast<u32>(aRoutineId)];
	buffers.ClearContent([](auto& aSet) { aSet.clear(); });
	return buffers.SwapContent();
}

const std::unordered_set<DispatchableBufferBase*>& DispatchableBufferBase::GetUpdatedBuffers(const ERoutineId aRoutineId)
{
	return UpdatedBuffers[static_cast<u32>(aRoutineId)].ReadContent();
}

void DispatchableBufferBase::BufferUpdated(const ERoutineId aRoutineId)
{
	if (myIsUpdated)
	{
		return;
	}
	UpdatedBuffers[static_cast<u32>(aRoutineId)].WriteContent([this](auto& aSet) { aSet.insert(this); });
	myIsUpdated = true;
}
