#pragma once

#include "DebugLine.h"
#include "DebugLineRenderObject.h"

#include "Message/MessageSubscriber.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/Future.h"

class TimeKeeperSingleton;

class ShaderResourceSystem;

class DebugLineDrawer
	: public System<
		Policy::Set<
			PAdd<ShaderResourceSystem, PWrite>,
			PAdd<TimeKeeperSingleton, PRead>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	DebugLineDrawer() noexcept;

	void Render();
	void Update();

private:

	void OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage);

	Future<void> myOnShaderProgram;
	MessageSubscriber mySubscriber;
	std::optional<DebugLineRenderObject> myRenderObject;
	std::vector<SDebugLine> myDebugLines;
};
