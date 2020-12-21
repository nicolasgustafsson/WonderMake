#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>
#include "Utilities/Container/Container.h"
#include <Typedefs.h>
#include <iostream>

TEST_CASE("General Container Tests", "[Container]")
{
	Container<int, Iterable, Indexable> intContainer;
	//Container<int, Key<int>, Indexable, RandomAccess> dictionary;
	std::unordered_set<int> bla;

	auto it = bla.begin();

	std::cout << typeid(it).name();

	SECTION("The containers are of the correct type")
	{
		Container<int, Iterable, Indexable> intContainerColony;
		Container<int, Iterable, Indexable, RandomAccess> intContainerVector;
		Container<int, Iterable, ConstantInsertion, ConstantDeletion> intContainerLinkedList;

		REQUIRE(intContainerColony.HasBackend<ColonyBackend<int>>());
		REQUIRE(intContainerVector.HasBackend<VectorBackend<int>>());
		REQUIRE(intContainerLinkedList.HasBackend<LinkedListBackend<int>>());
		REQUIRE(intContainerVector.HasTrait<Sortable>);
		REQUIRE(!intContainerColony.HasTrait<Sortable>);
	}

	SECTION("You can add to a container")
	{
		REQUIRE(intContainer.Count() == 0);
		intContainer.Add(10);
		intContainer.Add(13);
		intContainer.Add(50);
		intContainer.Add(22);
		REQUIRE(intContainer.Count() == 4);

		intContainer.Clear();
	}

	SECTION("You can erase from a container")
	{
		REQUIRE(intContainer.Count() == 0);
		intContainer.Add(10);
		intContainer.Add(13);
		REQUIRE(intContainer.Count() == 2);
		intContainer.Erase(intContainer.begin());
		REQUIRE(intContainer.Count() == 1);
		intContainer.Clear();
		REQUIRE(intContainer.Count() == 0);
	}

	SECTION("You can add to a container 100000 times")
	{
		REQUIRE(intContainer.Count() == 0);

		for (i32 i = 0; i < 100000; i++)
		{
			intContainer.Add(10);
		}

		REQUIRE(intContainer.Count() == 100000);

		intContainer.Clear();
	}

	SECTION("You can erase elements by predicates")
	{
		REQUIRE(intContainer.Count() == 0);

		intContainer.Add(15);
		intContainer.Add(-45);
		intContainer.Add(-45);
		intContainer.Add(12);
		intContainer.Add(1002);
		intContainer.Add(102);
		intContainer.Add(-45);
		intContainer.Add(53);
		intContainer.Add(99998);
		intContainer.Add(1111);

		REQUIRE(intContainer[1] == -45);

		intContainer.Erase(-45);

		REQUIRE(intContainer[1] == 12);

		//[Nicos]: remove all even numbers
		intContainer.EraseIf([](int aNum) {return aNum % 2 == 0; });

		REQUIRE(intContainer.Count() == 3);
		
		REQUIRE(intContainer[0] == 15);
		REQUIRE(intContainer[1] == 53);
		REQUIRE(intContainer[2] == 1111);
		
		intContainer.Clear();
	}

	std::cin.get();
}
