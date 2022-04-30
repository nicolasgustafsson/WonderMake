#pragma once
#include "System/System.h"
#include "DebugLine.h"
#include "DebugLineRenderObject.h"
#include "Utilities/TimeKeeper.h"
#include "Universe/UniverseSystem.h"

class DebugLineDrawer
: public UniverseSystem<DebugLineDrawer, Wm::TimeKeeper>
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
