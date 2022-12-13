#include "pch.h"

#include "DebugLineDrawer.h"

#include "Utilities/TimeKeeper.h"

#include "wondermake-utility/Utility.h"

WM_REGISTER_SYSTEM(DebugLineDrawer);

DebugLineDrawer::DebugLineDrawer() noexcept
	: mySubscriber(BindHelper(&DebugLineDrawer::OnGotDebugLineMessage, this))
{}

void DebugLineDrawer::Render()
{
	if (!myRenderObject)
		myRenderObject.emplace(Get<ResourceSystem<Shader<EShaderType::Vertex>>>(), Get<ResourceSystem<Shader<EShaderType::Fragment>>>(), Get<ResourceSystem<Shader<EShaderType::Geometry>>>(), 10000);

	for (auto [i, line] : Utility::Enumerate(myDebugLines))
		myRenderObject->SetLine(static_cast<u32>(i), line);

	myRenderObject->SetLineCount(static_cast<u32>(myDebugLines.size()));

	myRenderObject->RenderImmediate();
}

void DebugLineDrawer::Update()
{
	const float deltaTime = Get<TimeKeeperSingleton>().GetDeltaTime<WmChrono::fSeconds>().count();

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
