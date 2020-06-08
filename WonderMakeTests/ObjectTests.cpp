#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Components/Component.h"

#include "Functionalities/Functionality.h"

#include "Object/Object.h"

struct SCatComponent
	: public SComponent
{
	u32 LegCount = 4;
};

class CatFunctionality
	: public Functionality<
		CatFunctionality,
		Policy::Set<
			Policy::Add<SCatComponent, Policy::EPermission::Write>>>
{
public:
	CatFunctionality(Object& aOwner) noexcept
		: Super(aOwner)
	{}

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
		BagFunctionality,
		Policy::Set<
			Policy::Add<SBagComponent, Policy::EPermission::Write>>>
{
public:
	BagFunctionality(Object& aOwner) noexcept
		: Super(aOwner)
	{}

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
	Object object;

	SECTION("Components can be added to and removed from Objects")
	{
		object.Add<SCatComponent>();
		object.Add<SBagComponent>();

		object.Remove<SCatComponent>();
		object.Remove<SBagComponent>();
	}
	
	SECTION("Functionalities can be added to and removed from Objects")
	{
		object.Add<CatFunctionality>();
		object.Add<BagFunctionality>();

		object.Remove<CatFunctionality>();
		object.Remove<BagFunctionality>();
	}
	
	SECTION("Added functionalities refer to the same instance")
	{
		auto& catA = object.Add<CatFunctionality>();
		auto& catB = object.Add<CatFunctionality>();

		auto& bagA = object.Add<BagFunctionality>();
		auto& bagB = object.Add<BagFunctionality>();

		REQUIRE(&catA == &catB);
		REQUIRE(&bagA == &bagB);
	}
}

TEST_CASE("Objects keep track of when a dependency no longer have any references", "[Object]")
{
	Object object;

	{
		auto& cat = object.Add<CatFunctionality>();

		REQUIRE(cat.GetLegCount() == 4);

		cat.Chop();

		REQUIRE(cat.GetLegCount() == 3);
	}

	// (Kevin): Adding an extra functionality so the object isn't completely reset whenever a dependency is removed.
	// This is to ensure that dependencies are still removed and properly work, even if there's other dependencies left.
	object.Add<BagFunctionality>();

	SECTION("Dependencies are not reset when there are still references to it")
	{
		object.Add<CatFunctionality>();

		object.Remove<CatFunctionality>();

		auto& cat = object.Add<CatFunctionality>();

		REQUIRE(cat.GetLegCount() == 3);
	}

	SECTION("Removed dependencies are reset when removed")
	{
		object.Remove<CatFunctionality>();

		auto& cat = object.Add<CatFunctionality>();

		REQUIRE(cat.GetLegCount() == 4);
	}

	SECTION("Dependencies are not reset when if a depending dependency is removed")
	{
		auto& catComponent = object.Add<SCatComponent>();

		object.Remove<CatFunctionality>();

		REQUIRE(catComponent.LegCount == 3);
	}
}

TEST_CASE("Objects can be visited", "[Object]")
{
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
		object.Add<SCatComponent>();
		object.Add<SBagComponent>();

		object.Visit(visitCallable);

		REQUIRE(callCount == 2);
	}

	SECTION("Objects with functionalities should invoke the callable once for every dependency")
	{
		object.Add<CatFunctionality>();
		object.Add<SBagComponent>();

		object.Visit(visitCallable);

		REQUIRE(callCount == 3);
	}

	SECTION("Removed dependencies should not invoke the callable")
	{
		object.Add<CatFunctionality>();
		object.Add<BagFunctionality>();
		object.Add<SBagComponent>();

		object.Visit(visitCallable);

		REQUIRE(callCount == 4);

		callCount = 0;

		object.Remove<BagFunctionality>();

		object.Visit(visitCallable);

		REQUIRE(callCount == 3);
	}

}
