#include "stdafx.h"
#include "DebugLineDrawer.h"
#include "Utilities/Utility.h"

DebugLineDrawer::DebugLineDrawer() noexcept
	: myRenderObject(10000)
	, mySubscriber(ERoutineId::Logic, BindHelper(&DebugLineDrawer::OnGotDebugLineMessage, this))
{

}

void DebugLineDrawer::Render()
{
	const float deltaTime = myTimeKeeperPtr->GetDeltaSeconds();

	for (i32 i = static_cast<i32>(myDebugLines.size()) - 1; i >= 0; i--)
	{
		SDebugLine& line = myDebugLines[i];

		if (line.Duration < 0.f)
		{
			myDebugLines.erase(myDebugLines.begin() + i);
			continue;
		}

		line.Duration -= deltaTime;
	}

	for (auto[i, line] : Utility::Enumerate(myDebugLines))
	{
		myRenderObject.SetLine(static_cast<u32>(i), line);
	}

	myRenderObject.SetLineCount(static_cast<u32>(myDebugLines.size()));
	myRenderObject.Render();
}

void DebugLineDrawer::OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage)
{
	myDebugLines.push_back(aDebugLineMessage.Line);
}
