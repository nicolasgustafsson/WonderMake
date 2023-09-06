#include <catch2/catch.hpp>

#include <algorithm>

#include "wondermake-base/SystemPolicy.h"
#include "wondermake-base/SystemTypeId.h"

#include "Policies/Scheduler.h"

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
	static_assert(!SetA::HasDependency_v<SystemA>);
	static_assert(!SetA::HasDependency_v<SystemB>);
	static_assert(!SetA::HasDependency_v<SystemC>);
	static_assert(!SetA::HasDependency_v<SystemD>);

	static_assert(SetB::HasDependency_v<SystemA>);
	static_assert(!SetB::HasDependency_v<SystemB>);
	static_assert(!SetB::HasDependency_v<SystemC>);
	static_assert(!SetB::HasDependency_v<SystemD>);

	static_assert(SetC::HasDependency_v<SystemA>);
	static_assert(!SetC::HasDependency_v<SystemC>);
	static_assert(!SetC::HasDependency_v<SystemC>);
	static_assert(!SetC::HasDependency_v<SystemD>);

	static_assert(SetD::HasDependency_v<SystemA>);
	static_assert(SetD::HasDependency_v<SystemB>);
	static_assert(SetD::HasDependency_v<SystemC>);
	static_assert(!SetD::HasDependency_v<SystemD>);

	static_assert(!SetA::HasPolicy_v<SystemA, PRead>);
	static_assert(!SetA::HasPolicy_v<SystemA, PWrite>);

	static_assert(SetB::HasPolicy_v<SystemA, PRead>);
	static_assert(!SetB::HasPolicy_v<SystemA, PWrite>);

	static_assert(!SetC::HasPolicy_v<SystemA, PRead>);
	static_assert(SetC::HasPolicy_v<SystemA, PWrite>);

	static_assert(SetD::HasPolicy_v<SystemA, PWrite>);
	static_assert(SetD::HasPolicy_v<SystemB, PRead>);
	static_assert(SetD::HasPolicy_v<SystemC, PWrite>);
}

void StaticTest_Policy_Concat()
{
	using ConcatSetA = Policy::Concat<SetA>;
	using ConcatSetB = Policy::Concat<SetB, SetC>;
	using ConcatSetC = Policy::Concat<SetB, SetC, SetD>;
	using ConcatSetD = Policy::Concat<SetA, SetB, SetC, SetD>;
	using ConcatSetE = Policy::Concat<SetB, SetA, SetC, SetD>;

	static_assert(!ConcatSetA::HasDependency_v<SystemA>);
	static_assert(!ConcatSetA::HasDependency_v<SystemB>);
	static_assert(!ConcatSetA::HasDependency_v<SystemC>);
	static_assert(!ConcatSetA::HasDependency_v<SystemD>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemA,	PWrite>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemA,	PRead>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemB,	PWrite>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemB,	PRead>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemC,	PWrite>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemC,	PRead>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemD,	PWrite>);
	static_assert(!ConcatSetA::HasPolicy_v<SystemD,	PRead>);
	
	static_assert(ConcatSetB::HasDependency_v<SystemA>);
	static_assert(!ConcatSetB::HasDependency_v<SystemB>);
	static_assert(!ConcatSetB::HasDependency_v<SystemC>);
	static_assert(!ConcatSetB::HasDependency_v<SystemD>);
	static_assert(ConcatSetB::HasPolicy_v<SystemA,	PWrite>);
	static_assert(ConcatSetB::HasPolicy_v<SystemA,	PRead>);
	static_assert(!ConcatSetB::HasPolicy_v<SystemB,	PWrite>);
	static_assert(!ConcatSetB::HasPolicy_v<SystemB,	PRead>);
	static_assert(!ConcatSetB::HasPolicy_v<SystemC,	PWrite>);
	static_assert(!ConcatSetB::HasPolicy_v<SystemC,	PRead>);
	static_assert(!ConcatSetB::HasPolicy_v<SystemD,	PWrite>);
	static_assert(!ConcatSetB::HasPolicy_v<SystemD,	PRead>);
	
	static_assert(ConcatSetC::HasDependency_v<SystemA>);
	static_assert(ConcatSetC::HasDependency_v<SystemB>);
	static_assert(ConcatSetC::HasDependency_v<SystemC>);
	static_assert(!ConcatSetC::HasDependency_v<SystemD>);
	static_assert(ConcatSetC::HasPolicy_v<SystemA,	PWrite>);
	static_assert(ConcatSetC::HasPolicy_v<SystemA,	PRead>);
	static_assert(!ConcatSetC::HasPolicy_v<SystemB,	PWrite>);
	static_assert(ConcatSetC::HasPolicy_v<SystemB,	PRead>);
	static_assert(ConcatSetC::HasPolicy_v<SystemC,	PWrite>);
	static_assert(!ConcatSetC::HasPolicy_v<SystemC,	PRead>);
	static_assert(!ConcatSetC::HasPolicy_v<SystemD,	PWrite>);
	static_assert(!ConcatSetC::HasPolicy_v<SystemD,	PRead>);

	static_assert(std::is_same_v<ConcatSetC, ConcatSetD>);
	static_assert(std::is_same_v<ConcatSetC, ConcatSetE>);
}

TEST_CASE("SystemTypeId can be created and copied", "[SystemTypeId]")
{
	SECTION("SystemTypeId can be created and is unique per class")
	{
		const SystemTypeId idSystemA1 = SystemTypeId::Create<SystemA>();
		const SystemTypeId idSystemA2 = SystemTypeId::Create<SystemA>();
		const SystemTypeId idSystemB = SystemTypeId::Create<SystemB>();

		REQUIRE(idSystemA1.Hash() != 0);
		REQUIRE(idSystemA2.Hash() != 0);
		REQUIRE(idSystemB.Hash() != 0);

		REQUIRE(idSystemA1.Hash() != idSystemB.Hash());
		REQUIRE(idSystemA2.Hash() != idSystemB.Hash());
		REQUIRE(idSystemA1.Hash() == idSystemA2.Hash());
	}

	SECTION("SystemTypeId can be copied")
	{
		const SystemTypeId idSystemA = SystemTypeId::Create<SystemA>();
		SystemTypeId idSystemB = SystemTypeId::Create<SystemB>();

		REQUIRE(idSystemA.Hash() != idSystemB.Hash());

		idSystemB = idSystemA;

		REQUIRE(idSystemA.Hash() == idSystemB.Hash());
	}
}

TEST_CASE("Policies can be created and can be tested for conflicts", "[Policy]")
{
	SECTION("Policy can be created")
	{
		const SystemTypeId idSystemA = SystemTypeId::Create<SystemA>();
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
		CHECK(listB[0].myDependencyId == SystemTypeId::Create<SystemA>());
		CHECK(listB[0].myPermission == PRead);

		REQUIRE(listC.size() == 1);
		CHECK(listC[0].myDependencyId == SystemTypeId::Create<SystemA>());
		CHECK(listC[0].myPermission == PWrite);

		REQUIRE(listD.size() == 3);
		CHECK(listD[0].myDependencyId == SystemTypeId::Create<SystemA>());
		CHECK(listD[0].myPermission == PWrite);
		CHECK(listD[1].myDependencyId == SystemTypeId::Create<SystemB>());
		CHECK(listD[1].myPermission == PRead);
		CHECK(listD[2].myDependencyId == SystemTypeId::Create<SystemC>());
		CHECK(listD[2].myPermission == PWrite);
	}

	SECTION("Policy can be copied")
	{
		const SystemTypeId idSystemA = SystemTypeId::Create<SystemA>();
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

	schedule.AddToSchedule(SystemTypeId::Create<SystemA>(), policyListA);
	schedule.AddToSchedule(SystemTypeId::Create<SystemB>(), policyListB);
	schedule.AddToSchedule(SystemTypeId::Create<SystemC>(), policyListC);
	schedule.AddToSchedule(SystemTypeId::Create<SystemD>(), policyListD);
	schedule.AddToSchedule(SystemTypeId::Create<SystemE>(), policyListE);

	REQUIRE_FALSE(schedule.Schedule());
}