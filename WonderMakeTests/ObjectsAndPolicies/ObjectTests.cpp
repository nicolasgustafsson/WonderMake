#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Components/Component.h"

#include "Functionalities/Functionality.h"

#include "Object/Object.h"

#include "WonderMakeBase/ScheduleSystem.h"

struct SCatComponent
	: public SComponent
{
	u32 LegCount = 4;
};

class CatFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<SCatComponent, PWrite>>>
{
public:
	u32 GetLegCount() const
	{
		return Get<SCatComponent>().LegCount;
	}

	void Chop()
	{
		--Get<SCatComponent>().LegCount;
	}
};

struct SBagComponent
	: public SComponent
{
	u32 CatCount = 0;
};

class BagFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<SBagComponent, PWrite>>>
{
public:
	u32 GetCatCount() const
	{
		return Get<SBagComponent>().CatCount;
	}

	void AddCat()
	{
		++Get<SBagComponent>().CatCount;
	}

	void RemoveCat()
	{
		--Get<SBagComponent>().CatCount;
	}
};

TEST_CASE("Objects can be created and have dependencies added and removed", "[Object]")
{
	ScheduleSystem::InjectDependencies(std::tie());
	const auto mockScheduleProc = [](auto&&) {};

	ScheduleSystem scheduleSystem(mockScheduleProc, mockScheduleProc);

	ComponentSystem<SCatComponent>::InjectDependencies(std::tie());

	ComponentSystem<SCatComponent> sysCompCat;

	ComponentSystem<SBagComponent>::InjectDependencies(std::tie());

	ComponentSystem<SBagComponent> sysCompBag;

	FunctionalitySystem<CatFunctionality>::InjectDependencies(std::tie(scheduleSystem, sysCompCat));

	FunctionalitySystem<CatFunctionality> sysFuncCat;

	FunctionalitySystem<BagFunctionality>::InjectDependencies(std::tie(scheduleSystem, sysCompBag));

	FunctionalitySystem<BagFunctionality> sysFuncBag;

	Object object;

	SECTION("Components can be added to and removed from Objects")
	{
		sysCompCat.AddComponent(object);
		sysCompBag.AddComponent(object);

		object.Remove<SCatComponent>();
		object.Remove<SBagComponent>();
	}
	
	SECTION("Functionalities can be added to and removed from Objects")
	{
		sysFuncCat.AddFunctionality(object);
		sysFuncBag.AddFunctionality(object);

		object.Remove<CatFunctionality>();
		object.Remove<BagFunctionality>();
	}
	
	SECTION("Added functionalities refer to the same instance")
	{
		auto& catA = sysFuncCat.AddFunctionality(object);
		auto& catB = sysFuncCat.AddFunctionality(object);

		auto& bagA = sysFuncBag.AddFunctionality(object);
		auto& bagB = sysFuncBag.AddFunctionality(object);

		REQUIRE(&catA == &catB);
		REQUIRE(&bagA == &bagB);
	}
}

TEST_CASE("Objects keep track of when a dependency no longer have any references", "[Object]")
{
	ScheduleSystem::InjectDependencies(std::tie());
	const auto mockScheduleProc = [](auto&&) {};

	ScheduleSystem scheduleSystem(mockScheduleProc, mockScheduleProc);

	ComponentSystem<SCatComponent>::InjectDependencies(std::tie());

	ComponentSystem<SCatComponent> sysCompCat;

	ComponentSystem<SBagComponent>::InjectDependencies(std::tie());

	ComponentSystem<SBagComponent> sysCompBag;

	FunctionalitySystem<CatFunctionality>::InjectDependencies(std::tie(scheduleSystem, sysCompCat));

	FunctionalitySystem<CatFunctionality> sysFuncCat;

	FunctionalitySystem<BagFunctionality>::InjectDependencies(std::tie(scheduleSystem, sysCompBag));

	FunctionalitySystem<BagFunctionality> sysFuncBag;

	Object object;

	{
		auto& cat = sysFuncCat.AddFunctionality(object);;

		REQUIRE(cat.GetLegCount() == 4);

		cat.Chop();

		REQUIRE(cat.GetLegCount() == 3);
	}

	// (Kevin): Adding an extra functionality so the object isn't completely reset whenever a dependency is removed.
	// This is to ensure that dependencies are still removed and properly work, even if there's other dependencies left.
	sysFuncBag.AddFunctionality(object);

	SECTION("Dependencies are not reset when there are still references to it")
	{
		sysFuncCat.AddFunctionality(object, false);

		object.Remove<CatFunctionality>(false);

		auto& cat = sysFuncCat.AddFunctionality(object);

		REQUIRE(cat.GetLegCount() == 3);
	}

	SECTION("Removed dependencies are reset when removed")
	{
		object.Remove<CatFunctionality>();

		auto& cat = sysFuncCat.AddFunctionality(object);;

		REQUIRE(cat.GetLegCount() == 4);
	}

	SECTION("Dependencies are not reset when if a depending dependency is removed")
	{
		auto& catComponent = sysCompCat.AddComponent(object);

		object.Remove<CatFunctionality>();

		REQUIRE(catComponent.LegCount == 3);
	}
}

TEST_CASE("Objects can be visited", "[Object]")
{
	ScheduleSystem::InjectDependencies(std::tie());
	const auto mockScheduleProc = [](auto&&) {};

	ScheduleSystem scheduleSystem(mockScheduleProc, mockScheduleProc);

	ComponentSystem<SCatComponent>::InjectDependencies(std::tie());

	ComponentSystem<SCatComponent> sysCompCat;

	ComponentSystem<SBagComponent>::InjectDependencies(std::tie());

	ComponentSystem<SBagComponent> sysCompBag;

	FunctionalitySystem<CatFunctionality>::InjectDependencies(std::tie(scheduleSystem, sysCompCat));

	FunctionalitySystem<CatFunctionality> sysFuncCat;

	FunctionalitySystem<BagFunctionality>::InjectDependencies(std::tie(scheduleSystem, sysCompBag));

	FunctionalitySystem<BagFunctionality> sysFuncBag;

	Object object;
	u32 callCount = 0;

	const auto visitCallable = [&callCount](auto&, auto&)
	{
		++callCount;
	};

	SECTION("Objects with no dependencies should not invoke the callable")
	{
		object.Visit(visitCallable);

		REQUIRE(callCount == 0);
	}

	SECTION("Objects with only components should invoke the callable once for every component")
	{
		sysCompCat.AddComponent(object);
		sysCompBag.AddComponent(object);

		object.Visit(visitCallable);

		REQUIRE(callCount == 2);
	}

	SECTION("Objects with functionalities should invoke the callable once for every dependency")
	{
		sysFuncCat.AddFunctionality(object);
		sysCompBag.AddComponent(object);

		object.Visit(visitCallable);

		REQUIRE(callCount == 3);
	}

	SECTION("Removed dependencies should not invoke the callable")
	{
		sysFuncCat.AddFunctionality(object);
		sysFuncBag.AddFunctionality(object);
		sysCompBag.AddComponent(object);

		object.Visit(visitCallable);

		REQUIRE(callCount == 4);

		callCount = 0;

		object.Remove<BagFunctionality>();

		object.Visit(visitCallable);

		REQUIRE(callCount == 3);
	}

}
