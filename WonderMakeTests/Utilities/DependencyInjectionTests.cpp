#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Utilities/DependencyInjector.h"

struct Kalle {};

struct Hobbe
{
	Hobbe() = default;

	Hobbe(Kalle& aKalle)
		: myKalle(&aKalle)
	{}

	Kalle* myKalle = nullptr;
};

struct Gregg
{
	Gregg() = default;

	Gregg(Kalle& aKalle, Hobbe& aHobbe)
		: myKalle(&aKalle)
		, myHobbe(&aHobbe)
	{}

	Kalle* myKalle = nullptr;
	Hobbe* myHobbe = nullptr;
};

TEST_CASE("DependencyInjector are injecting the correct objects", "[DependencyInjector]")
{
	Kalle kalle;
	Hobbe hobbe;
	Gregg gregg;
	uint32_t kalleCallCount = 0;
	uint32_t hobbeCallCount = 0;
	uint32_t greggCallCount = 0;

	const auto createKalle = [&kalleCallCount, &kalle]() -> Kalle&
	{
		++kalleCallCount;

		return kalle;
	};
	const auto createHobbe = [&hobbeCallCount, &hobbe](Kalle& aKalle) -> Hobbe&
	{
		++hobbeCallCount;

		return hobbe = Hobbe(aKalle);
	};
	const auto createGregg = [&greggCallCount, &gregg](Kalle& aKalle, Hobbe& aHobbe) -> Gregg&
	{
		++greggCallCount;

		return gregg = Gregg(aKalle, aHobbe);
	};

	DependencyInjector di;
	Kalle* kallePtr = nullptr;
	Hobbe* hobbePtr = nullptr;
	Gregg* greggPtr = nullptr;

	SECTION("Added and fetched in order")
	{
		di.Add<Kalle>(createKalle);
		di.Add<Hobbe, decltype(createHobbe), Kalle>(createHobbe);
		di.Add<Gregg, decltype(createGregg), Kalle, Hobbe>(createGregg);

		SECTION("Create all dependencies")
		{
			di.CreateAll();

			REQUIRE(kalleCallCount == 1);
			REQUIRE(hobbeCallCount == 1);
			REQUIRE(greggCallCount == 1);
		}

		SECTION("Create dependencies individually")
		{
			REQUIRE(kalleCallCount == 0);
			REQUIRE(hobbeCallCount == 0);
			REQUIRE(greggCallCount == 0);
		}

		kallePtr = &di.Get<Kalle>();
		hobbePtr = &di.Get<Hobbe>();
		greggPtr = &di.Get<Gregg>();
	}

	SECTION("Added out of order and fetched in order")
	{
		di.Add<Hobbe, decltype(createHobbe), Kalle>(createHobbe);
		di.Add<Gregg, decltype(createGregg), Kalle, Hobbe>(createGregg);
		di.Add<Kalle>(createKalle);

		SECTION("Create all dependencies")
		{
			di.CreateAll();

			REQUIRE(kalleCallCount == 1);
			REQUIRE(hobbeCallCount == 1);
			REQUIRE(greggCallCount == 1);
		}

		SECTION("Create dependencies individually")
		{
			REQUIRE(kalleCallCount == 0);
			REQUIRE(hobbeCallCount == 0);
			REQUIRE(greggCallCount == 0);
		}

		kallePtr = &di.Get<Kalle>();
		hobbePtr = &di.Get<Hobbe>();
		greggPtr = &di.Get<Gregg>();
	}

	SECTION("Added in order and fetched out of order")
	{
		di.Add<Kalle>(createKalle);
		di.Add<Hobbe, decltype(createHobbe), Kalle>(createHobbe);
		di.Add<Gregg, decltype(createGregg), Kalle, Hobbe>(createGregg);

		SECTION("Create all dependencies")
		{
			di.CreateAll();

			REQUIRE(kalleCallCount == 1);
			REQUIRE(hobbeCallCount == 1);
			REQUIRE(greggCallCount == 1);
		}

		SECTION("Create dependencies individually")
		{
			REQUIRE(kalleCallCount == 0);
			REQUIRE(hobbeCallCount == 0);
			REQUIRE(greggCallCount == 0);
		}

		greggPtr = &di.Get<Gregg>();
		kallePtr = &di.Get<Kalle>();
		hobbePtr = &di.Get<Hobbe>();
	}

	SECTION("Added and fetched out of order ")
	{
		di.Add<Gregg, decltype(createGregg), Kalle, Hobbe>(createGregg);
		di.Add<Kalle>(createKalle);
		di.Add<Hobbe, decltype(createHobbe), Kalle>(createHobbe);

		SECTION("Create all dependencies")
		{
			di.CreateAll();

			REQUIRE(kalleCallCount == 1);
			REQUIRE(hobbeCallCount == 1);
			REQUIRE(greggCallCount == 1);
		}

		SECTION("Create dependencies individually")
		{
			REQUIRE(kalleCallCount == 0);
			REQUIRE(hobbeCallCount == 0);
			REQUIRE(greggCallCount == 0);
		}

		greggPtr = &di.Get<Gregg>();
		hobbePtr = &di.Get<Hobbe>();
		kallePtr = &di.Get<Kalle>();
	}

	REQUIRE(kalleCallCount == 1);
	REQUIRE(hobbeCallCount == 1);
	REQUIRE(greggCallCount == 1);
	REQUIRE(&kalle == kallePtr);
	REQUIRE(&hobbe == hobbePtr);
	REQUIRE(&gregg == greggPtr);
	REQUIRE(hobbePtr->myKalle == kallePtr);
	REQUIRE(greggPtr->myKalle == kallePtr);
	REQUIRE(greggPtr->myHobbe == hobbePtr);
}

TEST_CASE("DependencyInjector throws an exception when a dependency is missing.", "[DependencyInjector]")
{
	Kalle kalle;
	Hobbe hobbe;
	Gregg gregg;

	const auto createKalle = [&kalle]() -> Kalle&
	{
		return kalle;
	};
	const auto createHobbe = [&hobbe](Kalle& aKalle) -> Hobbe&
	{
		return hobbe = Hobbe(aKalle);
	};
	const auto createGregg = [&gregg](Kalle& aKalle, Hobbe& aHobbe) -> Gregg&
	{
		return gregg = Gregg(aKalle, aHobbe);
	};

	DependencyInjector di;

	di.Add<Kalle>(createKalle);
	di.Add<Gregg, decltype(createGregg), Kalle, Hobbe>(createGregg);

	SECTION("Create dependency without missing dependency")
	{
		REQUIRE(&di.Get<Kalle>() == &kalle);
	}

	SECTION("Create all dependencies")
	{
		REQUIRE_THROWS(di.CreateAll());
	}

	SECTION("Create missing dependency")
	{
		REQUIRE_THROWS(di.Get<Hobbe>());
	}

	SECTION("Create dependency with missing dependency")
	{
		REQUIRE_THROWS(di.Get<Gregg>());
	}
}
