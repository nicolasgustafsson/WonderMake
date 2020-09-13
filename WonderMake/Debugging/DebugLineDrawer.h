#pragma once
#include "System/System.h"
#include "DebugLine.h"
#include "DebugLineRenderObject.h"
#include "Utilities/TimeKeeper.h"

class DebugLineDrawer
	: public System<
		Policy::Set<
			Policy::Add<TimeKeeper, Policy::EPermission::Read>>>
{
public:
	DebugLineDrawer(Dependencies&& aDependencies) noexcept;

	void Render();

private:
	void OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage);

	MessageSubscriber mySubscriber;
	DebugLineRenderObject myRenderObject;
	std::vector<SDebugLine> myDebugLines;
};
