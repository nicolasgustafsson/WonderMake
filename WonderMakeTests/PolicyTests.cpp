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

	SECTION("Policy can be created")
	{
		const SystemId idSystemA = SystemId::Create<SystemA>();
		const Policy policy = Policy::Create<SystemA>(Policy::EPermission::Read);

		REQUIRE(policy.myDependencyId == idSystemA);
		REQUIRE(policy.myPermission == Policy::EPermission::Read);
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

		REQUIRE_FALSE(policy1.Conflicts(policy2));
		REQUIRE_FALSE(policy2.Conflicts(policy1));
		REQUIRE_FALSE(policy1.Conflicts(policy3));
		REQUIRE_FALSE(policy3.Conflicts(policy1));

		REQUIRE(policy1.Conflicts(policy4));
		REQUIRE(policy4.Conflicts(policy1));
	}

}

TEST_CASE("Scheduler can dequeue the correct SystemIds in the correct order", "[Scheduler]")
{
	class SystemReadA;
	class SystemReadB;
	class SystemWriteA;
	class SystemWriteB;
	class SystemContent;

	const Policy policyReadContent = Policy::Create<SystemContent>(Policy::EPermission::Read);
	const Policy policyWriteContent = Policy::Create<SystemContent>(Policy::EPermission::Write);

	const std::vector<Policy> policyListReadContent = { policyReadContent };
	const std::vector<Policy> policyListWriteContent = { policyWriteContent };
	const std::vector<Policy> policyListEmpty;

	Scheduler schedule;

	schedule.AddToSchedule(SystemId::Create<SystemReadA>(), policyListReadContent);
	schedule.AddToSchedule(SystemId::Create<SystemReadB>(), policyListReadContent);
	schedule.AddToSchedule(SystemId::Create<SystemWriteA>(), policyListWriteContent);
	schedule.AddToSchedule(SystemId::Create<SystemWriteB>(), policyListWriteContent);
	schedule.AddToSchedule(SystemId::Create<SystemContent>(), policyListEmpty);

	REQUIRE(schedule.Schedule());

	REQUIRE(schedule.IsDone());

	schedule.ResetQueue();

	REQUIRE_FALSE(schedule.IsDone());

	const std::vector<SystemId> validReadList = { SystemId::Create<SystemReadA>(), SystemId::Create<SystemReadB>() };

	const auto dequeue = [&]()
	{
		const auto compareLists = [](const auto& aListA, const auto& aListB) -> bool
		{
			for (const auto& element : aListA)
			{
				if (std::find(aListB.cbegin(), aListB.cend(), element) == aListB.cend())
				{
					return false;
				}
			}
			return true;
		};

		std::vector<SystemId> dequeuedIds;

		{
			const auto id = schedule.DequeueSystem();
			REQUIRE(id);
			dequeuedIds.emplace_back(*id);
		}
		{
			const auto id = schedule.DequeueSystem();
			if (id)
			{
				dequeuedIds.emplace_back(*id);
			}
		}

		REQUIRE_FALSE(schedule.DequeueSystem());
		REQUIRE_FALSE(schedule.IsDone());
		REQUIRE_FALSE(dequeuedIds.empty());
		REQUIRE(dequeuedIds.size() <= 2);

		if (dequeuedIds.size() == 2)
		{
			REQUIRE(compareLists(dequeuedIds, validReadList));

			schedule.MarkSystemAsDone(dequeuedIds.back());

			REQUIRE_FALSE(schedule.DequeueSystem());

			schedule.MarkSystemAsDone(dequeuedIds.front());
		}
		else if (dequeuedIds.size() == 1)
		{
			const auto id = dequeuedIds.front();
			REQUIRE((id == SystemId::Create<SystemWriteA>() || id == SystemId::Create<SystemWriteB>()));

			schedule.MarkSystemAsDone(id);
		}

		REQUIRE_FALSE(schedule.IsDone());
	};
	
	dequeue();
	dequeue();
	dequeue();

	const auto id = schedule.DequeueSystem();
	REQUIRE(id);
	REQUIRE(*id == SystemId::Create<SystemContent>());

	REQUIRE_FALSE(schedule.DequeueSystem());

	REQUIRE_FALSE(schedule.IsDone());

	schedule.MarkSystemAsDone(SystemId::Create<SystemContent>());

	REQUIRE_FALSE(schedule.DequeueSystem());
	REQUIRE(schedule.IsDone());
}

TEST_CASE("Scheduler detects circular dependencies", "[Scheduler]")
{
	class SystemA;
	class SystemB;
	class SystemC;
	class SystemD;

	const std::vector<Policy> policyListA = { Policy::Create<SystemB>(Policy::EPermission::Read), Policy::Create<SystemC>(Policy::EPermission::Read) };
	const std::vector<Policy> policyListB = {};
	const std::vector<Policy> policyListC = { Policy::Create<SystemD>(Policy::EPermission::Read) };
	const std::vector<Policy> policyListD = { Policy::Create<SystemA>(Policy::EPermission::Read) };

	Scheduler schedule;

	schedule.AddToSchedule(SystemId::Create<SystemA>(), policyListA);
	schedule.AddToSchedule(SystemId::Create<SystemB>(), policyListB);
	schedule.AddToSchedule(SystemId::Create<SystemC>(), policyListC);
	schedule.AddToSchedule(SystemId::Create<SystemD>(), policyListD);

	REQUIRE_FALSE(schedule.Schedule());
}