#include <catch2/catch.hpp>

#include "wondermake-utility/Typedefs.h"

#include "WonderMakeBase/System.h"
#include "WonderMakeBase/SystemContainer.h"

class ContainerTestSystemA
	: public System<>
{};

class ContainerTestSystemB
	: public System<>
{};

TEST_CASE("Empty system container", "[SystemContainer]")
{
	SystemContainer container;

	auto optA = container.TryGet<ContainerTestSystemA>();
	auto optB = container.TryGet<ContainerTestSystemB>();
	
	REQUIRE(!optA);
	REQUIRE(!optB);
}

TEST_CASE("Construct container with Systems", "[SystemContainer]")
{
	ContainerTestSystemA::InjectDependencies(std::tie());

	auto testSystemA = std::make_shared<ContainerTestSystemA>();

	ContainerTestSystemB::InjectDependencies(std::tie());

	auto testSystemB = std::make_shared<ContainerTestSystemB>();

	SystemContainer container(
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
	SystemContainer container;

	ContainerTestSystemA::InjectDependencies(std::tie());

	auto testSystem = std::make_shared<ContainerTestSystemA>();

	container.Add(testSystem);

	auto opt = container.TryGet<ContainerTestSystemA>();
	
	CHECK(&container.Get<ContainerTestSystemA>() == testSystem.get());
	REQUIRE(opt);

	REQUIRE(&*opt == testSystem.get());
}
