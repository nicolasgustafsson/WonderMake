#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Typedefs.h"

#include "System/System.h"
#include "System/SystemContainer_v2.h"

class ContainerTestSystemA
	: public System<>
{};

class ContainerTestSystemB
	: public System<>
{};

TEST_CASE("Empty system container", "[SystemContainer]")
{
	SystemContainer_v2 container;

	auto optA = container.TryGet<ContainerTestSystemA>();
	auto optB = container.TryGet<ContainerTestSystemB>();
	
	REQUIRE(!optA);
	REQUIRE(!optB);
}

TEST_CASE("Construct container with Systems", "[SystemContainer]")
{
	auto testSystemA = std::make_shared<ContainerTestSystemA>();
	auto testSystemB = std::make_shared<ContainerTestSystemB>();

	SystemContainer_v2 container(
		{
			std::make_pair<std::type_index, std::shared_ptr<SystemAbstracted>>(typeid(ContainerTestSystemA), testSystemA),
			std::make_pair<std::type_index, std::shared_ptr<SystemAbstracted>>(typeid(ContainerTestSystemB), testSystemB),
		});

	auto optA = container.TryGet<ContainerTestSystemA>();
	auto optB = container.TryGet<ContainerTestSystemB>();

	CHECK(&container.Get<ContainerTestSystemA>() == testSystemA.get());
	CHECK(&container.Get<ContainerTestSystemB>() == testSystemB.get());
	REQUIRE(optA);
	REQUIRE(optB);

	REQUIRE(&*optA == testSystemA.get());
	REQUIRE(&*optB == testSystemB.get());
}

TEST_CASE("Add System to container", "[SystemContainer]")
{
	SystemContainer_v2 container;

	auto testSystem = std::make_shared<ContainerTestSystemA>();

	container.Add(testSystem);

	auto opt = container.TryGet<ContainerTestSystemA>();
	
	CHECK(&container.Get<ContainerTestSystemA>() == testSystem.get());
	REQUIRE(opt);

	REQUIRE(&*opt == testSystem.get());
}
