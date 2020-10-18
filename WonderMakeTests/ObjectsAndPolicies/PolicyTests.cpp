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
		const Policy policy = Policy::Create<SystemA>(Policy::EPermission::Read);

		REQUIRE(policy.myDependencyId == idSystemA);
		REQUIRE(policy.myPermission == Policy::EPermission::Read);
	}

	SECTION("Policy::Set can converted to a list")
	{
		using setA = Policy::Set<>;
		using setB = Policy::Set<
			Policy::Add<SystemA, Policy::EPermission::Read>>;
		using setC = Policy::Set<
			Policy::Add<SystemA, Policy::EPermission::Write>>;
		using setD = Policy::Set<
			Policy::Add<SystemA, Policy::EPermission::Unrestricted>,
			Policy::Add<SystemB, Policy::EPermission::Read>,
			Policy::Add<SystemC, Policy::EPermission::Write>>;

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

		static_assert(!setA::HasPolicy_v<SystemA, Policy::EPermission::Read>,			"setA should not have read permisson for SystemA.");
		static_assert(!setA::HasPolicy_v<SystemA, Policy::EPermission::Write>,			"setA should not have write permisson for SystemA.");
		static_assert(!setA::HasPolicy_v<SystemA, Policy::EPermission::Unrestricted>,	"setA should not have unrestricted permisson for SystemA.");

		static_assert(setB::HasPolicy_v<SystemA, Policy::EPermission::Read>,			"setB should have read permisson for SystemA.");
		static_assert(!setB::HasPolicy_v<SystemA, Policy::EPermission::Write>,			"setB should not have write permisson for SystemA.");
		static_assert(!setB::HasPolicy_v<SystemA, Policy::EPermission::Unrestricted>,	"setB should not have unrestricted permisson for SystemA.");

		static_assert(!setC::HasPolicy_v<SystemA, Policy::EPermission::Read>,			"setC should not have read permisson for SystemA."); 
		static_assert(setC::HasPolicy_v<SystemA, Policy::EPermission::Write>,			"setC should have write permisson for SystemA.");
		static_assert(!setC::HasPolicy_v<SystemA, Policy::EPermission::Unrestricted>,	"setC should not have unrestricted permisson for SystemA.");

		static_assert(setD::HasPolicy_v<SystemA, Policy::EPermission::Unrestricted>,	"setD should have unrestricted permisson for SystemA.");
		static_assert(setD::HasPolicy_v<SystemB, Policy::EPermission::Read>,			"setD should have read permisson for SystemB.");
		static_assert(setD::HasPolicy_v<SystemC, Policy::EPermission::Write>,			"setD should have write permisson for SystemC.");

		const auto listA = setA::GetPolicies();
		const auto listB = setB::GetPolicies();
		const auto listC = setC::GetPolicies();
		const auto listD = setD::GetPolicies();

		REQUIRE(listA.size() == 0);

		REQUIRE(listB.size() == 1);
		REQUIRE(listB[0].myDependencyId == SystemId::Create<SystemA>());
		REQUIRE(listB[0].myPermission == Policy::EPermission::Read);

		REQUIRE(listC.size() == 1);
		REQUIRE(listC[0].myDependencyId == SystemId::Create<SystemA>());
		REQUIRE(listC[0].myPermission == Policy::EPermission::Write);

		REQUIRE(listD.size() == 3);
		REQUIRE(listD[0].myDependencyId == SystemId::Create<SystemA>());
		REQUIRE(listD[0].myPermission == Policy::EPermission::Unrestricted);
		REQUIRE(listD[1].myDependencyId == SystemId::Create<SystemB>());
		REQUIRE(listD[1].myPermission == Policy::EPermission::Read);
		REQUIRE(listD[2].myDependencyId == SystemId::Create<SystemC>());
		REQUIRE(listD[2].myPermission == Policy::EPermission::Write);
	}

	SECTION("Policy can be copied")
	{
		const SystemId idSystemA = SystemId::Create<SystemA>();
		const Policy policy1 = Policy::Create<SystemA>(Policy::EPermission::Read);
		const Policy policy2 = policy1;

		REQUIRE(policy2.myDependencyId == idSystemA);
		REQUIRE(policy2.myPermission == Policy::EPermission::Read);
	}

	SECTION("Policy conflicts correctly")
	{
		const Policy policy1 = Policy::Create<SystemA>(Policy::EPermission::Read);
		const Policy policy2 = Policy::Create<SystemA>(Policy::EPermission::Read);
		const Policy policy3 = Policy::Create<SystemB>(Policy::EPermission::Read);
		const Policy policy4 = Policy::Create<SystemA>(Policy::EPermission::Write);
		const Policy policy5 = Policy::Create<SystemA>(Policy::EPermission::Unrestricted);
		const Policy policy6 = Policy::Create<SystemA>(Policy::EPermission::Unrestricted);

		REQUIRE_FALSE(policy1.Conflicts(policy2));
		REQUIRE_FALSE(policy2.Conflicts(policy1));
		REQUIRE_FALSE(policy1.Conflicts(policy3));
		REQUIRE_FALSE(policy3.Conflicts(policy1));

		REQUIRE(policy1.Conflicts(policy4));
		REQUIRE(policy4.Conflicts(policy1));

		REQUIRE(policy1.Conflicts(policy6));
		REQUIRE(policy2.Conflicts(policy6));
		REQUIRE(policy3.Conflicts(policy6));
		REQUIRE(policy4.Conflicts(policy6));
		REQUIRE_FALSE(policy5.Conflicts(policy6));
	}
}

TEST_CASE("Scheduler detects circular dependencies", "[Scheduler]")
{
	class SystemA;
	class SystemB;
	class SystemC;
	class SystemD;
	class SystemE;

	const std::vector<Policy> policyListA = { Policy::Create<SystemB>(Policy::EPermission::Read), Policy::Create<SystemC>(Policy::EPermission::Read) };
	const std::vector<Policy> policyListB = {};
	const std::vector<Policy> policyListC = { Policy::Create<SystemD>(Policy::EPermission::Read) };
	const std::vector<Policy> policyListD = { Policy::Create<SystemA>(Policy::EPermission::Read) };
	const std::vector<Policy> policyListE = { Policy::Create<SystemA>(Policy::EPermission::Unrestricted) };

	Scheduler schedule;

	schedule.AddToSchedule(SystemId::Create<SystemA>(), policyListA);
	schedule.AddToSchedule(SystemId::Create<SystemB>(), policyListB);
	schedule.AddToSchedule(SystemId::Create<SystemC>(), policyListC);
	schedule.AddToSchedule(SystemId::Create<SystemD>(), policyListD);
	schedule.AddToSchedule(SystemId::Create<SystemE>(), policyListE);

	REQUIRE_FALSE(schedule.Schedule());
}