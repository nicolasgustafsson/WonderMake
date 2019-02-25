#pragma once
#include "System/System.h"
#include "DebugLine.h"
#include "DebugLineRenderObject.h"
#include "TimeKeeper.h"

class DebugLineDrawer
	: public System
{
public:
	DebugLineDrawer();
	~DebugLineDrawer() = default;

	void Render();

private:
	void OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage);

	MessageSubscriber mySubscriber;
	DebugLineRenderObject myRenderObject;
	std::vector<SDebugLine> myDebugLines;
	SystemPtr<TimeKeeper> myTimeKeeperPtr;
};
