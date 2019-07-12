#include "stdafx.h"
#include "DispatchableBuffer.h"

DoubleBuffer<DispatchableBufferBase::DispatchableContainer> DispatchableBufferBase::UpdatedBuffers[RoutineCount];

std::mutex DispatchableBufferBase::myLock;

std::mutex DispatchableBufferBase::myTemporaryLock;

DispatchableBufferBase::DispatchableBufferBase(const size_t aTypeHash) noexcept
	: myTypeHash(aTypeHash)
{}

bool DispatchableBufferBase::UpdateBuffers(const ERoutineId aRoutineId) noexcept
{
	auto& buffers = UpdatedBuffers[static_cast<u32>(aRoutineId)];
	buffers.ClearContent([](auto& aContainer)
	{
		for (auto buffer : aContainer.myUpdatedBuffers)
		{
			buffer->Clear();
		}
		aContainer.myUpdatedBuffers.clear();
		aContainer.myMessages.clear();
	});
	std::lock_guard<decltype(myTemporaryLock)> _lock(myTemporaryLock);
	std::lock_guard<decltype(myLock)> lock(myLock);
	buffers.SwapContent();
	for (auto buffer : buffers.ReadContent().myUpdatedBuffers)
	{
		buffer->Swap();
	}
	return !buffers.ReadContent().myUpdatedBuffers.empty();
}

void DispatchableBufferBase::GetDispatchableList(const ERoutineId aRoutineId, std::vector<const Dispatchable*>& aOutList) noexcept
{
	const auto& buffers = UpdatedBuffers[static_cast<u32>(aRoutineId)].ReadContent();
	for (const auto& dispatchable : buffers.myMessages)
	{
		aOutList.emplace_back(&dispatchable.myBuffer->GetDispatchableAt(dispatchable.myIndex));
	}
}

void DispatchableBufferBase::AddDispatchable(const ERoutineId aRoutineId, const size_t aIndex) noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	UpdatedBuffers[static_cast<u32>(aRoutineId)].WriteContent([this, aIndex](auto& aContainer)
	{
		aContainer.myUpdatedBuffers.insert(this);
		DispatchableLocation location;
		location.myBuffer = this;
		location.myIndex = aIndex;
		aContainer.myMessages.emplace_back(location);
	});
}
