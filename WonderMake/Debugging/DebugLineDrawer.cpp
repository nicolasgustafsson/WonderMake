#include "pch.h"
#include "DebugLineDrawer.h"
#include "Utilities/Utility.h"

REGISTER_SYSTEM(DebugLineDrawer);

DebugLineDrawer::DebugLineDrawer() noexcept
	: myRenderObject(10000)
	, mySubscriber(ERoutineId::Logic, BindHelper(&DebugLineDrawer::OnGotDebugLineMessage, this))
{

}

void DebugLineDrawer::Render()
{
	for (auto [i, line] : Utility::Enumerate(myDebugLines))
	{
		myRenderObject.SetLine(static_cast<u32>(i), line);
	}

	myRenderObject.SetLineCount(static_cast<u32>(myDebugLines.size()));

	myRenderObject.RenderImmediate();
}

void DebugLineDrawer::Update()
{
	const float deltaTime = Get<TimeKeeper>().GetDeltaSeconds();

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
}

void DebugLineDrawer::OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage)
{
	myDebugLines.push_back(aDebugLineMessage.Line);
}
