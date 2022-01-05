#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "WonderMakeBase/Typedefs.h"

#include "System/SystemRegistry.h"

class RegistryTestSystem
	: public System<>
{};

class RegistryTestSystemBase
	: public SystemAbstracted
{};

class RegistryTestSystemChild
	: public RegistryTestSystemBase
	, public SystemSub<>
{};

TEST_CASE("Add System to registry", "[SystemRegistry]")
{
	SystemRegistry registry;
	auto testSystem = std::make_shared<RegistryTestSystem>();

	u32 callCounter = 0;

	registry.AddSystem<RegistryTestSystem>([&callCounter, &testSystem]() -> std::shared_ptr<RegistryTestSystem>
		{
			++callCounter;

			return testSystem;
		});

	REQUIRE(callCounter == 0);
}

TEST_CASE("Add derived System to registry", "[SystemRegistry]")
{
	SystemRegistry registry;
	auto testSystem = std::make_shared<RegistryTestSystemChild>();

	u32 callCounter = 0;

	registry.AddSystem<RegistryTestSystemChild, RegistryTestSystemBase>([&callCounter, &testSystem]() ->std::shared_ptr<RegistryTestSystemBase>
		{
			++callCounter;

			return testSystem;
		});

	REQUIRE(callCounter == 0);
}

TEST_CASE("Create System in registry", "[SystemRegistry]")
{
	SystemRegistry registry;
	auto testSystem = std::make_shared<RegistryTestSystem>();

	u32 callCounter = 0;

	registry.AddSystem<RegistryTestSystem>([&callCounter, &testSystem]() -> std::shared_ptr<RegistryTestSystem>
		{
			++callCounter;

			return testSystem;
		});

	registry.CreateSystems({});

	REQUIRE(callCounter == 1);
}

TEST_CASE("Create System in registry twice", "[SystemRegistry]")
{
	SystemRegistry registry;
	auto testSystem = std::make_shared<RegistryTestSystem>();

	u32 callCounter = 0;

	registry.AddSystem<RegistryTestSystem>([&callCounter, &testSystem]() -> std::shared_ptr<RegistryTestSystem>
		{
			++callCounter;

			return testSystem;
		});

	registry.CreateSystems({});
	registry.CreateSystems({});

	REQUIRE(callCounter == 2);
}
