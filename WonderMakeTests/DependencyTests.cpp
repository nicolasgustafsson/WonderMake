#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Components/Component.h"

#include "Functionalities/Functionality.h"

#include "Object/Dependency.h"
#include "Object/Object.h"

struct SHamsterTerrariumComponent
	: public SComponent
{
	u32 HamsterCount = 100;
};

class HamsterTerrariumFunctionality
	: public Functionality<HamsterTerrariumFunctionality
	, Policy::Set<
		Policy::Add<SHamsterTerrariumComponent, Policy::EPermission::Write>>>
		
{
public:
	HamsterTerrariumFunctionality(Object& aOwner) noexcept
		: Super(aOwner)
	{}

	u32 GetHamsterCount() const
	{
		return Get<SHamsterTerrariumComponent>().HamsterCount;
	}

	void ThrowInHamster()
	{
		++Get<SHamsterTerrariumComponent>().HamsterCount;
	}

	void Flood()
	{
		Get<SHamsterTerrariumComponent>().HamsterCount = 0;
	}
};

TEST_CASE("Dependency can be created, dereferenced and destroyed", "[Dependency]")
{
	Object object;
	Dependency<HamsterTerrariumFunctionality> dependency;

	SECTION("Dependency can be created and dereferenced")
	{
		dependency.Create(object);

		REQUIRE(dependency->GetHamsterCount() == 100);

		(*dependency).ThrowInHamster();

		REQUIRE(dependency->GetHamsterCount() == 101);

		dependency.Destroy(object, *dependency);
	}

	SECTION("Dependency are reset after being destroyed")
	{
		dependency.Create(object);

		REQUIRE(dependency->GetHamsterCount() == 100);

		dependency->Flood();

		REQUIRE(dependency->GetHamsterCount() == 0);

		dependency.Destroy(object, *dependency);

		dependency.Create(object);

		REQUIRE(dependency->GetHamsterCount() == 100);
	}
}

TEST_CASE("Dependencies can be created, dereferenced and destroyed", "[Dependencies]")
{
	using HamsterTerrariumPolicySet = Policy::Set<
		Policy::Add<HamsterTerrariumFunctionality, Policy::EPermission::Write>,
		Policy::Add<SHamsterTerrariumComponent, Policy::EPermission::Write>>;

	Object object;

	SECTION("Dependencies can be created and dereferenced")
	{
		Dependencies<HamsterTerrariumPolicySet> dependencies(object);

		auto& terrariumFunctionality = dependencies.Get<HamsterTerrariumFunctionality>();
		auto& terrariumComponent = dependencies.Get<SHamsterTerrariumComponent>();

		REQUIRE(terrariumFunctionality.GetHamsterCount() == 100);
		REQUIRE(terrariumComponent.HamsterCount == 100);

		terrariumFunctionality.ThrowInHamster();

		REQUIRE(terrariumFunctionality.GetHamsterCount() == 101);
		REQUIRE(terrariumComponent.HamsterCount == 101);
	}

	SECTION("Dependencies are reset after being destroyed")
	{
		Dependencies<HamsterTerrariumPolicySet> dependencies(object);

		{
			auto& terrariumFunctionality = dependencies.Get<HamsterTerrariumFunctionality>();
			auto& terrariumComponent = dependencies.Get<SHamsterTerrariumComponent>();

			REQUIRE(terrariumFunctionality.GetHamsterCount() == 100);
			REQUIRE(terrariumComponent.HamsterCount == 100);

			terrariumFunctionality.Flood();

			REQUIRE(terrariumFunctionality.GetHamsterCount() == 0);
			REQUIRE(terrariumComponent.HamsterCount == 0);

			dependencies.Destroy(object, terrariumFunctionality);
		}

		dependencies.Create(object);

		auto& terrariumFunctionality = dependencies.Get<HamsterTerrariumFunctionality>();

		REQUIRE(terrariumFunctionality.GetHamsterCount() == 100);
	}
}
