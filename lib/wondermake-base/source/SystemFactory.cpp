#include "wondermake-base/SystemFactory.h"

#include "wondermake-base/DependencySystem.h"
#include "wondermake-base/GuidGeneratorSystem.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WmLogTags.h"

WM_REGISTER_SYSTEM(SystemFactory);

Future<Result<std::shared_ptr<SystemContainer>, SystemRegistry::SError>> SystemFactory::CreateSystems(
	SystemRegistry::Filter																	aFilter,
	std::function<Future<void>(AnyExecutor aExecutor, Closure&& aTask)>						aScheduleFunc,
	std::function<EventSubscriber(AnyExecutor aExecutor, std::function<void()>&& aTask)>	aScheduleRepeatingFunc)
{
	auto [promise, future] = MakeAsync<Result<std::shared_ptr<SystemContainer>, SystemRegistry::SError>>();
	auto& guidGenSys = Get<GuidGeneratorSystem>();
	auto& scheduleSys = Get<ScheduleSystemSingleton>();

	auto sheduledFuture = scheduleSys
		.Schedule(GetExecutor(), [this,
			filter = std::move(aFilter),
			passedScheduleFunc = std::move(aScheduleFunc),
			passedScheduleRepeatingFunc = std::move(aScheduleRepeatingFunc),
			promise = std::move(promise),
			&guidGenSys,
			&scheduleSys]() mutable
	{
		auto getScheduleFunc = [&scheduleSys, &passedScheduleFunc]()
			-> std::function<Future<void>(AnyExecutor aExecutor, Closure&& aTask)>
		{
			if (passedScheduleFunc)
				return std::move(passedScheduleFunc);

			return [&scheduleSys](AnyExecutor aExecutor, Closure&& aTask)
				-> Future<void>
			{
				return scheduleSys.Schedule(std::move(aExecutor), std::move(aTask));
			};
		};
		auto getScheduleRepeatingFunc = [&scheduleSys, &passedScheduleRepeatingFunc]()
			-> std::function<EventSubscriber(AnyExecutor aExecutor, std::function<void()>&& aTask)>
		{
			if (passedScheduleRepeatingFunc)
				return std::move(passedScheduleRepeatingFunc);

			return [&scheduleSys](AnyExecutor aExecutor, std::function<void()>&& aTask)
				-> EventSubscriber
			{
				return scheduleSys.ScheduleRepeating(std::move(aExecutor), std::move(aTask));
			};
		};

		auto registry = Global::GetSystemRegistry();
		auto scheduleFunc = getScheduleFunc();
		auto scheduleRepeatingFunc = getScheduleRepeatingFunc();
		const auto generateSystemId = [&guidGenSys]() -> SystemId
		{
			auto guid = guidGenSys.GenerateNew();

			if (!guid)
				WmLogError(TagWonderMake << "Failed to generate system id.");

			return *guid;
		};

		registry.AddSystem<ScheduleSystem>([&scheduleFunc, &scheduleRepeatingFunc](auto aOnCreate, auto aOnDestroy) -> SharedReference<ScheduleSystem>
			{
				auto destroy = [onDestroy = std::move(aOnDestroy)](ScheduleSystem* aPtr) mutable
				{
					if (!aPtr)
						return;

					std::move(onDestroy)(*aPtr);

					delete aPtr;
				};

				std::move(aOnCreate)();

				auto ptr = std::shared_ptr<ScheduleSystem>(new ScheduleSystem(std::move(scheduleFunc), std::move(scheduleRepeatingFunc)), destroy);

				return SharedReference<ScheduleSystem>::FromPointer(std::move(ptr))
					.Unwrap();
			});

		auto result = registry
			.CreateSystems(filter, generateSystemId);

		if (!result)
		{
			promise.Complete(std::move(result));

			return;
		}

		auto& systems = result.Unwrap();

		if (auto* depSys = systems.TryGet<DependencySystem>(); depSys)
			depSys->SetSystems(systems);

		auto ptr = std::shared_ptr<SystemContainer>(new SystemContainer(std::move(systems)), [](SystemContainer* const aPtr)
			{
				if (!aPtr)
					return;

				if (auto* schSys = aPtr->TryGet<ScheduleSystem>(); schSys)
					schSys->CancelAll();

				if (auto* depSys = aPtr->TryGet<DependencySystem>(); depSys)
					depSys->ResetSystems();

				delete aPtr;
			});

		promise.Complete(Ok(std::move(ptr)));
	});
	
	future.AddOwnership(std::move(sheduledFuture));

	return std::move(future);
}
