#pragma once
#include "System/System.h"
#include "DebugLine.h"
#include "DebugLineRenderObject.h"
#include "Utilities/TimeKeeper.h"

class DebugLineDrawer
	: public System<
		Policy::Set<
			PAdd<TimeKeeper, PRead>>>
{
public:
	DebugLineDrawer() noexcept;

	void Render();
	void Update();

private:

	void OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage);

	MessageSubscriber mySubscriber;
	DebugLineRenderObject myRenderObject;
	std::vector<SDebugLine> myDebugLines;
};
