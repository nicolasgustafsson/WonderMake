#pragma once

#include "wondermake-base/System.h"
#include "wondermake-base/SystemContainer.h"
#include "wondermake-base/SystemRegistry.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Result.h"
#include "wondermake-utility/UniqueFunction.h"

#include <functional>
#include <memory>

class GuidGeneratorSystem;
class ScheduleSystemSingleton;

class SystemFactory
	: public System<
		Policy::Set<
			PAdd<GuidGeneratorSystem, PWrite>,
			PAdd<ScheduleSystemSingleton, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	Future<Result<std::shared_ptr<SystemContainer>, SystemRegistry::SError>> CreateSystems(
		SystemRegistry::Filter																	aFilter,
		std::function<Future<void>(AnyExecutor aExecutor, Closure&& aTask)>						aScheduleFunc			= nullptr,
		std::function<EventSubscriber(AnyExecutor aExecutor, std::function<void()>&& aTask)>	aScheduleRepeatingFunc	= nullptr);
};
