#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include <algorithm>

#include "Policies/Policy.h"
#include "Policies/Scheduler.h"
#include "Policies/SystemId.h"

TEST_CASE("SystemId can be created and copied", "[SystemId]")
{
	class SystemA;
	class SystemB;

	SECTION("SystemId can be created and is unique per class")
	{
		const SystemId idSystemA1 = SystemId::Create<SystemA>();
		const SystemId idSystemA2 = SystemId::Create<SystemA>();
		const SystemId idSystemB = SystemId::Create<SystemB>();

		REQUIRE(idSystemA1.Hash() != 0);
		REQUIRE(idSystemA2.Hash() != 0);
		REQUIRE(idSystemB.Hash() != 0);

		REQUIRE(idSystemA1.Hash() != idSystemB.Hash());
		REQUIRE(idSystemA2.Hash() != idSystemB.Hash());
		REQUIRE(idSystemA1.Hash() == idSystemA2.Hash());
	}

	SECTION("SystemId can be copied")
	{
		const SystemId idSystemA = SystemId::Create<SystemA>();
		SystemId idSystemB = SystemId::Create<SystemB>();

		REQUIRE(idSystemA.Hash() != idSystemB.Hash());

		idSystemB = idSystemA;

		REQUIRE(idSystemA.Hash() == idSystemB.Hash());
	}
}

TEST_CASE("Policies can be created and can be tested for conflicts", "[Policy]")
{
	class SystemA;
	class SystemB;
	class SystemC;
	class SystemD;

	SECTION("Policy can be created")
	{
		const SystemId idSystemA = SystemId::Create<SystemA>();
		const Policy policy = Policy::Create<SystemA>(PRead);

		REQUIRE(policy.myDependencyId == idSystemA);
		REQUIRE(policy.myPermission == PRead);
	}

	SECTION("Policy::Set can converted to a list")
	{
		using setA = Policy::Set<>;
		using setB = Policy::Set<
			PAdd<SystemA, PRead>>;
		using setC = Policy::Set<
			PAdd<SystemA, PWrite>>;
		using setD = Policy::Set<
			PAdd<SystemA, PWrite>,
			PAdd<SystemB, PRead>,
			PAdd<SystemC, PWrite>>;

		static_assert(!setA::HasDependency_v<SystemA>,	"setA should not depend on SystemA.");
		static_assert(!setA::HasDependency_v<SystemB>,	"setA should not depend on SystemB.");
		static_assert(!setA::HasDependency_v<SystemC>,	"setA should not depend on SystemC.");
		static_assert(!setA::HasDependency_v<SystemD>,	"setA should not depend on SystemD.");

		static_assert(setB::HasDependency_v<SystemA>,	"setB should depend on SystemA.");
		static_assert(!setB::HasDependency_v<SystemB>,	"setB should not depend on SystemB.");
		static_assert(!setB::HasDependency_v<SystemC>,	"setB should not depend on SystemC.");
		static_assert(!setB::HasDependency_v<SystemD>,	"setB should not depend on SystemD.");
		
		static_assert(setC::HasDependency_v<SystemA>,	"setC should depend on SystemA.");
		static_assert(!setC::HasDependency_v<SystemC>,	"setC should not depend on SystemB.");
		static_assert(!setC::HasDependency_v<SystemC>, "setC should not depend on SystemC.");
		static_assert(!setC::HasDependency_v<SystemD>, "setC should not depend on SystemD.");

		static_assert(setD::HasDependency_v<SystemA>,	"setD should depend on SystemA.");
		static_assert(setD::HasDependency_v<SystemB>,	"setD should depend on SystemB.");
		static_assert(setD::HasDependency_v<SystemC>,	"setD should depend on SystemC.");
		static_assert(!setD::HasDependency_v<SystemD>,	"setD should not depend on SystemD.");

		static_assert(!setA::HasPolicy_v<SystemA, PRead>,	"setA should not have read permisson for SystemA.");
		static_assert(!setA::HasPolicy_v<SystemA, PWrite>,	"setA should not have write permisson for SystemA.");

		static_assert(setB::HasPolicy_v<SystemA, PRead>,	"setB should have read permisson for SystemA.");
		static_assert(!setB::HasPolicy_v<SystemA, PWrite>,	"setB should not have write permisson for SystemA.");

		static_assert(!setC::HasPolicy_v<SystemA, PRead>,	"setC should not have read permisson for SystemA."); 
		static_assert(setC::HasPolicy_v<SystemA, PWrite>,	"setC should have write permisson for SystemA.");

		static_assert(setD::HasPolicy_v<SystemA, PWrite>,	"setD should have write permisson for SystemA.");
		static_assert(setD::HasPolicy_v<SystemB, PRead>,	"setD should have read permisson for SystemB.");
		static_assert(setD::HasPolicy_v<SystemC, PWrite>,	"setD should have write permisson for SystemC.");

		const auto listA = setA::GetPolicies();
		const auto listB = setB::GetPolicies();
		const auto listC = setC::GetPolicies();
		const auto listD = setD::GetPolicies();

		CHECK(listA.size() == 0);

		REQUIRE(listB.size() == 1);
		CHECK(listB[0].myDependencyId == SystemId::Create<SystemA>());
		CHECK(listB[0].myPermission == PRead);

		REQUIRE(listC.size() == 1);
		CHECK(listC[0].myDependencyId == SystemId::Create<SystemA>());
		CHECK(listC[0].myPermission == PWrite);

		REQUIRE(listD.size() == 3);
		CHECK(listD[0].myDependencyId == SystemId::Create<SystemA>());
		CHECK(listD[0].myPermission == PWrite);
		CHECK(listD[1].myDependencyId == SystemId::Create<SystemB>());
		CHECK(listD[1].myPermission == PRead);
		CHECK(listD[2].myDependencyId == SystemId::Create<SystemC>());
		CHECK(listD[2].myPermission == PWrite);
	}

	SECTION("Policy can be copied")
	{
		const SystemId idSystemA = SystemId::Create<SystemA>();
		const Policy policy1 = Policy::Create<SystemA>(PRead);
		const Policy policy2 = policy1;

		CHECK(policy2.myDependencyId == idSystemA);
		CHECK(policy2.myPermission == PRead);
	}

	SECTION("Policy conflicts correctly")
	{
		const Policy policy1 = Policy::Create<SystemA>(PRead);
		const Policy policy2 = Policy::Create<SystemA>(PRead);
		const Policy policy3 = Policy::Create<SystemB>(PRead);
		const Policy policy4 = Policy::Create<SystemA>(PWrite);
		const Policy policy5 = Policy::Create<SystemA>(PWrite);
		const Policy policy6 = Policy::Create<SystemA>(PWrite);

		CHECK_FALSE(policy1.Conflicts(policy2));
		CHECK_FALSE(policy2.Conflicts(policy1));
		CHECK_FALSE(policy1.Conflicts(policy3));
		CHECK_FALSE(policy3.Conflicts(policy1));

		CHECK(policy1.Conflicts(policy4));
		CHECK(policy4.Conflicts(policy1));

		CHECK(policy1.Conflicts(policy6));
		CHECK(policy2.Conflicts(policy6));
		CHECK_FALSE(policy3.Conflicts(policy6));
		CHECK(policy4.Conflicts(policy6));
		CHECK(policy5.Conflicts(policy6));
	}
}

TEST_CASE("Scheduler detects circular dependencies", "[Scheduler]")
{
	class SystemA;
	class SystemB;
	class SystemC;
	class SystemD;
	class SystemE;

	const std::vector<Policy> policyListA = { Policy::Create<SystemB>(PRead), Policy::Create<SystemC>(PRead) };
	const std::vector<Policy> policyListB = {};
	const std::vector<Policy> policyListC = { Policy::Create<SystemD>(PRead) };
	const std::vector<Policy> policyListD = { Policy::Create<SystemA>(PRead) };
	const std::vector<Policy> policyListE = { Policy::Create<SystemA>(PWrite) };

	Scheduler schedule;

	schedule.AddToSchedule(SystemId::Create<SystemA>(), policyListA);
	schedule.AddToSchedule(SystemId::Create<SystemB>(), policyListB);
	schedule.AddToSchedule(SystemId::Create<SystemC>(), policyListC);
	schedule.AddToSchedule(SystemId::Create<SystemD>(), policyListD);
	schedule.AddToSchedule(SystemId::Create<SystemE>(), policyListE);

	REQUIRE_FALSE(schedule.Schedule());
}