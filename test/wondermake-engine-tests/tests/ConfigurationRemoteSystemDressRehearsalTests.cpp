#include "wondermake-tests-common/GTestInclude.h"

#include "ConfigurationRemoteSystemTests.h"

#include "WonderMakeIoTests/SocketAcceptorMock.h"
#include "WonderMakeIoTests/SocketMock.h"

#include "ConfigurationProtoSerialization.h"

#include "wondermake-engine/ConfigurationRemoteSystem.h"

#include "WonderMakeBaseTests/GuidGeneratorSystemMock.h"

#include "wondermake-base/Configuration.h"
#include "wondermake-base/ConfigurationSystem.h"

#include "wondermake-utility/Endian.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/ConfigurationRemote.pb.h"
#pragma warning(pop)

TEST(ConfigurationRemoteSystemTests, dress_rehearsal)
{
	static constexpr auto instanceIdA	= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto instanceIdB	= *Guid::Parse("e8992896-f718-4d1a-97ff-06daab3558a4");
	static constexpr auto instanceIdC	= *Guid::Parse("162d0323-9fe5-46ed-9598-849f2d1998f3");

	static constexpr auto dummyId			= "dummy_id";
	static constexpr auto dummyValue		= "value";
	static constexpr auto dummyOverrideA	= "override_a";
	static constexpr auto dummyOverrideB	= "override_b";

	static constexpr auto findConfig = [](const Guid& aGuid)
	{
		return [guid = aGuid](const auto& aTuple)
		{
			const auto& [id, _] = aTuple;

			return guid == id;
		};
	};

	auto [configSysA, guidSysA, configRemoteSysA] = MakeConfigurationRemoteSystem(instanceIdA);
	auto [configSysB, guidSysB, configRemoteSysB] = MakeConfigurationRemoteSystem(instanceIdB);
	auto socketAcceptorMockA = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto [socketADn, socketBUp] = MakeLinkedSockets();

	const auto testConfiguration = [](const auto& aConfigRemote, const auto& aInstanceIdDn, const auto& aConfig)
	{
		auto begin	= aConfigRemote->begin();
		auto end	= aConfigRemote->end();

		const auto it = std::find_if(begin, end, findConfig(aInstanceIdDn));

		ASSERT_NE(it, end);

		const auto& [_, result] = *it;

		EXPECT_EQ(result.Configuration, aConfig->GetConfiguration());
	};

	configSysB->Set<std::string>(dummyId, dummyValue, EConfigGroup::Application);

	socketAcceptorMockA->DelegateToFake();

	configRemoteSysA->AcceptConnectionFrom(socketAcceptorMockA);
	configRemoteSysB->ConnectTo(std::move(socketBUp));

	socketAcceptorMockA->CompleteNextConnection(Ok(std::move(socketADn)));

	testConfiguration(configRemoteSysA, instanceIdB, configSysB);

	configRemoteSysA->SetOverride<std::string>(instanceIdB, dummyId, dummyOverrideA);

	EXPECT_EQ(configSysB->Get<std::string>(dummyId, "default"), dummyOverrideA);

	configSysB->SetOverride<std::string>(dummyId, dummyOverrideB);

	testConfiguration(configRemoteSysA, instanceIdB, configSysB);

	{
		auto [configSysC, guidSysC, configRemoteSysC] = MakeConfigurationRemoteSystem(instanceIdC);
		auto socketAcceptorMockB = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
		auto [socketBDn, socketCUp] = MakeLinkedSockets();

		socketAcceptorMockB->DelegateToFake();

		configRemoteSysB->AcceptConnectionFrom(socketAcceptorMockB);
		configRemoteSysC->ConnectTo(std::move(socketCUp));

		socketAcceptorMockB->CompleteNextConnection(Ok(std::move(socketBDn)));

		testConfiguration(configRemoteSysA, instanceIdB, configSysB);
		testConfiguration(configRemoteSysA, instanceIdC, configSysC);
		testConfiguration(configRemoteSysB, instanceIdC, configSysC);
	}

	auto begin	= configRemoteSysB->begin();
	auto end	= configRemoteSysB->end();

	EXPECT_EQ(begin, end);
	testConfiguration(configRemoteSysA, instanceIdB, configSysB);
}
