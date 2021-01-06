#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include <algorithm>

#include "Policies/Policy.h"
#include "Policies/Scheduler.h"
#include "Policies/SystemId.h"

class SystemA;
class SystemB;
class SystemC;
class SystemD;
class SystemE;

using SetA = Policy::Set<>;
using SetB = Policy::Set<
	PAdd<SystemA, PRead>>;
using SetC = Policy::Set<
	PAdd<SystemA, PWrite>>;
using SetD = Policy::Set<
	PAdd<SystemA, PWrite>,
	PAdd<SystemB, PRead>,
	PAdd<SystemC, PWrite>>;

void StaticTest_Policy_HasDependencyAndPolicy()
{
	static_assert(!SetA::HasDependency_v<SystemA>,	"SetA should not depend on SystemA.");
	static_assert(!SetA::HasDependency_v<SystemB>,	"SetA should not depend on SystemB.");
	static_assert(!SetA::HasDependency_v<SystemC>,	"SetA should not depend on SystemC.");
	static_assert(!SetA::HasDependency_v<SystemD>,	"SetA should not depend on SystemD.");

	static_assert(SetB::HasDependency_v<SystemA>,	"SetB should depend on SystemA.");
	static_assert(!SetB::HasDependency_v<SystemB>,	"SetB should not depend on SystemB.");
	static_assert(!SetB::HasDependency_v<SystemC>,	"SetB should not depend on SystemC.");
	static_assert(!SetB::HasDependency_v<SystemD>,	"SetB should not depend on SystemD.");

	static_assert(SetC::HasDependency_v<SystemA>,	"SetC should depend on SystemA.");
	static_assert(!SetC::HasDependency_v<SystemC>,	"SetC should not depend on SystemB.");
	static_assert(!SetC::HasDependency_v<SystemC>,	"SetC should not depend on SystemC.");
	static_assert(!SetC::HasDependency_v<SystemD>,	"SetC should not depend on SystemD.");

	static_assert(SetD::HasDependency_v<SystemA>,	"SetD should depend on SystemA.");
	static_assert(SetD::HasDependency_v<SystemB>,	"SetD should depend on SystemB.");
	static_assert(SetD::HasDependency_v<SystemC>,	"SetD should depend on SystemC.");
	static_assert(!SetD::HasDependency_v<SystemD>,	"SetD should not depend on SystemD.");

	static_assert(!SetA::HasPolicy_v<SystemA, PRead>,	"SetA should not have read permisson for SystemA.");
	static_assert(!SetA::HasPolicy_v<SystemA, PWrite>,	"SetA should not have write permisson for SystemA.");

	static_assert(SetB::HasPolicy_v<SystemA, PRead>,	"SetB should have read permisson for SystemA.");
	static_assert(!SetB::HasPolicy_v<SystemA, PWrite>,	"SetB should not have write permisson for SystemA.");

	static_assert(!SetC::HasPolicy_v<SystemA, PRead>,	"SetC should not have read permisson for SystemA.");
	static_assert(SetC::HasPolicy_v<SystemA, PWrite>,	"SetC should have write permisson for SystemA.");

	static_assert(SetD::HasPolicy_v<SystemA, PWrite>,	"SetD should have write permisson for SystemA.");
	static_assert(SetD::HasPolicy_v<SystemB, PRead>,	"SetD should have read permisson for SystemB.");
	static_assert(SetD::HasPolicy_v<SystemC, PWrite>,	"SetD should have write permisson for SystemC.");
}

TEST_CASE("SystemId can be created and copied", "[SystemId]")
{
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
	SECTION("Policy can be created")
	{
		const SystemId idSystemA = SystemId::Create<SystemA>();
		const Policy policy = Policy::Create<SystemA>(PRead);

		REQUIRE(policy.myDependencyId == idSystemA);
		REQUIRE(policy.myPermission == PRead);
	}

	SECTION("Policy::Set can converted to a list")
	{
		const auto listA = SetA::GetPolicies();
		const auto listB = SetB::GetPolicies();
		const auto listC = SetC::GetPolicies();
		const auto listD = SetD::GetPolicies();

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