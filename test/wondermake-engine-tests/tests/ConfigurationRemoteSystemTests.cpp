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

std::vector<u8> SerializeProto(const auto& aProto)
{
	const u32 size = static_cast<u32>(aProto.ByteSizeLong());

	std::vector<u8> serializedData(sizeof(size) + size, 0);

	*reinterpret_cast<u32*>(serializedData.data()) = ToEndianNetwork(size);

	aProto.SerializeToArray(serializedData.data() + sizeof(size), static_cast<int>(serializedData.size()));

	return serializedData;
}
MATCHER_P(ProtobufMessageMatcher, aExpectedProto, "")
{
	const u32 size = ToEndianHost(*reinterpret_cast<const u32*>(arg.data()));

	if (arg.size() < sizeof(size) + size)
		return false;

	std::decay_t<decltype(aExpectedProto)> argProto;

	const bool result = argProto.ParseFromArray(arg.data() + sizeof(size), static_cast<int>(size));

	if (!result)
		return false;

	const size_t readBytes = sizeof(size) + static_cast<size_t>(size);

	return argProto.SerializeAsString() == aExpectedProto.SerializeAsString();
}

using ProtoConfigurationRemote::EDataType;
using ProtoConfigurationRemote::Upstream;
using ProtoConfigurationRemote::Downstream;

template<typename TType>
auto MakeSetOverride(auto& aProtoSetOverride, const Guid aInstanceId, const std::string& aId, const std::optional<TType>& aOverrideValue)
{
	const auto instanceGuidString = aInstanceId.ToFixedSizeString();

	aProtoSetOverride.set_instanceid(instanceGuidString.ToCString());
	aProtoSetOverride.set_id(aId);
	if (aOverrideValue)
	{
		if constexpr (std::is_same_v<TType, bool>)
			aProtoSetOverride.set_config_bool(*aOverrideValue);
		else if constexpr (std::is_same_v<TType, bool>)
			aProtoSetOverride.set_config_bool(*aOverrideValue);
		else if constexpr (std::is_floating_point_v<TType>)
			aProtoSetOverride.set_config_f64(*aOverrideValue);
		else if constexpr (std::is_unsigned_v<TType>)
			aProtoSetOverride.set_config_u64(*aOverrideValue);
		else if constexpr (std::is_signed_v<TType>)
			aProtoSetOverride.set_config_i64(*aOverrideValue);
		else if constexpr (std::is_same_v<TType, std::string>)
			aProtoSetOverride.set_config_string(*aOverrideValue);
		else if constexpr (std::is_same_v<TType, FilePath>)
		{
			static constexpr auto convertFolderLocation = [](FilePath::EFolder aLocation)
			{
				switch (aLocation)
				{
				case FilePath::EFolder::Unset:		return ProtoConfigurationRemote::FilePathT_EFolder_Unset;
				case FilePath::EFolder::Bin:		return ProtoConfigurationRemote::FilePathT_EFolder_Bin;
				case FilePath::EFolder::Data:		return ProtoConfigurationRemote::FilePathT_EFolder_Data;
				case FilePath::EFolder::User:		return ProtoConfigurationRemote::FilePathT_EFolder_User;
				case FilePath::EFolder::UserData:	return ProtoConfigurationRemote::FilePathT_EFolder_UserData;
				}

				return ProtoConfigurationRemote::FilePathT_EFolder_Unset;
			};


			auto& filePath = *aProtoSetOverride.mutable_config_filepath();

			filePath.set_path(aOverrideValue->Path.string());
			filePath.set_location(convertFolderLocation(aOverrideValue->Location));
		}
	}
}

template<typename TType>
auto MakePushUpSetOverride(const Guid aInstanceId, const std::string& aId, const std::optional<TType>& aOverrideValue = std::nullopt)
{
	Upstream upStream;

	MakeSetOverride(*upStream.mutable_push()->mutable_set_override(), aInstanceId, aId, aOverrideValue);

	return upStream;
}
template<typename TType>
auto MakePushDownSetOverride(const Guid aInstanceId, const std::string& aId, const std::optional<TType>& aOverrideValue = std::nullopt)
{
	Downstream downStream;

	MakeSetOverride(*downStream.mutable_push()->mutable_set_override(), aInstanceId, aId, aOverrideValue);

	return downStream;
}

struct SInstanceData
{
	Guid					Id;
	std::string				Name;
	const Configuration& Configuration;
};

auto MakeInstancesAdded(const std::vector<SInstanceData>& aInstances)
{
	Upstream upStream;

	auto& push = *upStream.mutable_push()->mutable_instance_added();

	auto& reqInstances = *push.mutable_instances();

	for (const auto& instance : aInstances)
	{
		const auto instanceGuidString = instance.Id.ToFixedSizeString();

		auto& reqInstance = reqInstances[instanceGuidString.ToStringView()];

		reqInstance.set_name(instance.Name);
		SerializeConfigurationToProto(instance.Configuration, reqInstance);
	}

	return upStream;
}
auto MakeInstancesRemoved(const std::vector<Guid>& aInstanceIds)
{
	Upstream upStream;

	auto& push = *upStream.mutable_push()->mutable_instance_removed();

	auto& instanceIds = *push.mutable_instanceids();

	for (const auto& instanceId : aInstanceIds)
	{
		const auto instanceGuidString = instanceId.ToFixedSizeString();

		instanceIds.Add(instanceGuidString.ToString());
	}

	return upStream;
}

TEST(ConfigurationRemoteSystemTests, list_iterators_begin_iterator_is_same_as_end_iterator_when_constructed)
{
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	EXPECT_EQ(begin, end);
}

TEST(ConfigurationRemoteSystemTests, listed_config_have_all_different_values_from_instancesadded_push)
{
	static constexpr auto instanceName	= "instance";
	static constexpr Guid instanceGuid	= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");

	static constexpr auto instanceGuidString = instanceGuid.ToFixedSizeString();

	static constexpr auto findConfig = [](const Guid& aGuid)
	{
		return [guid = aGuid](const auto& aTuple)
		{
			const auto& [id, _] = aTuple;

			return guid == id;
		};
	};

	Configuration expectedConfiguration;

	expectedConfiguration.Set<bool>(		"test_id_bool",		false,				EConfigGroup::Application);
	expectedConfiguration.Set<f32>(			"test_id_f32",		1.1f,				EConfigGroup::Application);
	expectedConfiguration.Set<f64>(			"test_id_f64",		1.2,				EConfigGroup::Application);
	expectedConfiguration.Set<u8>(			"test_id_u8",		1,					EConfigGroup::Application);
	expectedConfiguration.Set<u16>(			"test_id_u16",		2,					EConfigGroup::Application);
	expectedConfiguration.Set<u32>(			"test_id_u32",		3,					EConfigGroup::Application);
	expectedConfiguration.Set<u64>(			"test_id_u64",		4,					EConfigGroup::Application);
	expectedConfiguration.Set<i8>(			"test_id_i8",		-1,					EConfigGroup::Application);
	expectedConfiguration.Set<i16>(			"test_id_i16",		-2,					EConfigGroup::Application);
	expectedConfiguration.Set<i32>(			"test_id_i32",		-3,					EConfigGroup::Application);
	expectedConfiguration.Set<i64>(			"test_id_i64",		-4,					EConfigGroup::Application);
	expectedConfiguration.Set<std::string>(	"test_id_string",	"value",			EConfigGroup::Application);
	expectedConfiguration.Set<FilePath>(	"test_id_filepath",	FilePath("value"),	EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ instanceGuid, instanceName, expectedConfiguration } });

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	const auto it = std::find_if(begin, end, findConfig(instanceGuid));

	EXPECT_EQ(std::distance(begin, end), 1);
	ASSERT_NE(it, end);

	const auto&[guid, result] = *it;

	EXPECT_EQ(guid, instanceGuid);
	EXPECT_EQ(result.InstanceName, instanceName);
	EXPECT_EQ(result.Configuration, expectedConfiguration);
}

TEST(ConfigurationRemoteSystemTests, list_iterators_does_not_list_instances_from_removedinstances_push)
{
	static constexpr auto instanceId = *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ instanceId, "INSTANCE_NAME", Configuration() } });
	const auto instancesRemoved = MakeInstancesRemoved({ instanceId });

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));
	socketMock->CompleteNextRead(Ok(SerializeProto(instancesRemoved)));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	EXPECT_EQ(begin, end);
}

TEST(ConfigurationRemoteSystemTests, list_iterators_does_not_list_instances_from_disconnected_socket)
{
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	auto [promise, future] = MakeAsync<Socket::ResultTypeClose>();

	ON_CALL(*socketMock, OnClose)
		.WillByDefault(Return(ByMove(std::move(future))));

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d"), "INSTANCE_NAME", Configuration() } });

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));

	promise.Complete(Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedRemotely }));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	EXPECT_EQ(begin, end);
}

TEST(ConfigurationRemoteSystemTests, previous_acceptor_is_removed_when_acceptconnectionfrom_is_called_with_nullptr)
{
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = std::make_shared<NiceMock<SocketAcceptorMock>>();

	socketAcceptorMock->DelegateToFake();
	auto [promise, future] = MakeAsync<SocketAcceptor::ResultTypeConnection>();
	std::weak_ptr<NiceMock<SocketAcceptorMock>> weakAcceptor = socketAcceptorMock;

	EXPECT_CALL(*socketAcceptorMock, OnConnection)
		.WillOnce(Return(ByMove(std::move(future))));

	configRemoteSys->AcceptConnectionFrom(std::move(socketAcceptorMock));

	EXPECT_TRUE(promise.IsValid());
	EXPECT_FALSE(weakAcceptor.expired());

	configRemoteSys->AcceptConnectionFrom(nullptr);

	EXPECT_FALSE(promise.IsValid());
	EXPECT_TRUE(weakAcceptor.expired());
}

TEST(ConfigurationRemoteSystemTests, previous_acceptor_is_removed_when_acceptconnectionfrom_is_called_with_new_acceptor)
{
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = std::make_shared<NiceMock<SocketAcceptorMock>>();

	socketAcceptorMock->DelegateToFake();
	auto [promise, future] = MakeAsync<SocketAcceptor::ResultTypeConnection>();
	std::weak_ptr<NiceMock<SocketAcceptorMock>> weakAcceptor = socketAcceptorMock;

	EXPECT_CALL(*socketAcceptorMock, OnConnection)
		.WillOnce(Return(ByMove(std::move(future))));

	configRemoteSys->AcceptConnectionFrom(std::move(socketAcceptorMock));

	EXPECT_TRUE(promise.IsValid());
	EXPECT_FALSE(weakAcceptor.expired());

	configRemoteSys->AcceptConnectionFrom(MakeSharedReference<NiceMock<SocketAcceptorMock>>());

	EXPECT_FALSE(promise.IsValid());
	EXPECT_TRUE(weakAcceptor.expired());
}

template<typename TType>
void TestPushUpSetOverridesSetsOverrides(const TType& aOverrideValue)
{
	static constexpr auto	instanceName		= "instance";
	static constexpr Guid	instanceGuid		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	configId			= "test_id";

	static constexpr auto instanceGuidString = instanceGuid.ToFixedSizeString();

	static constexpr auto findConfig = [](const Guid& aGuid)
	{
		return [guid = aGuid](const auto& aTuple)
		{
			const auto& [id, _] = aTuple;

			return guid == id;
		};
	};

	Configuration expectedConfiguration;

	expectedConfiguration.Set<TType>(configId, TType(), EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	expectedConfiguration.SetOverride<TType>(configId, aOverrideValue);

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ instanceGuid, instanceName, expectedConfiguration } });
	const auto setOverridePush = MakePushUpSetOverride<TType>(instanceGuid, configId, aOverrideValue);

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));

	socketMock->CompleteNextRead(Ok(SerializeProto(setOverridePush)));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	const auto it = std::find_if(begin, end, findConfig(instanceGuid));

	EXPECT_EQ(std::distance(begin, end), 1);
	ASSERT_NE(it, end);

	const auto& [id, result] = *it;

	EXPECT_EQ(result.Configuration, expectedConfiguration);
}

TEST(ConfigurationRemoteSystemTests, push_up_set_overrides_sets_overrides)
{
	TestPushUpSetOverridesSetsOverrides<bool>			(true);
	TestPushUpSetOverridesSetsOverrides<f32>			(1.f);
	TestPushUpSetOverridesSetsOverrides<f64>			(1.);
	TestPushUpSetOverridesSetsOverrides<u8>				(1);
	TestPushUpSetOverridesSetsOverrides<u16>			(1);
	TestPushUpSetOverridesSetsOverrides<u32>			(1);
	TestPushUpSetOverridesSetsOverrides<u64>			(1);
	TestPushUpSetOverridesSetsOverrides<i8>				(1);
	TestPushUpSetOverridesSetsOverrides<i16>			(1);
	TestPushUpSetOverridesSetsOverrides<i32>			(1);
	TestPushUpSetOverridesSetsOverrides<i64>			(1);
	TestPushUpSetOverridesSetsOverrides<std::string>	("override");
	TestPushUpSetOverridesSetsOverrides<FilePath>		(FilePath(FilePath::EFolder::Bin, "override"));
}

TEST(ConfigurationRemoteSystemTests, push_up_set_overrides_resets_override)
{
	static constexpr auto	instanceName		= "instance";
	static constexpr Guid	instanceGuid		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	configId			= "test_id";

	static constexpr auto	instanceGuidString	= instanceGuid.ToFixedSizeString();

	static constexpr auto findConfig = [](const Guid& aGuid)
	{
		return [guid = aGuid](const auto& aTuple)
		{
			const auto& [id, _] = aTuple;

			return guid == id;
		};
	};

	Configuration expectedConfiguration;

	expectedConfiguration.Set<bool>(configId, false, EConfigGroup::Application);

	expectedConfiguration.SetOverride<bool>(configId, true);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ instanceGuid, instanceName, expectedConfiguration } });
	const auto setOverridePush = MakePushUpSetOverride<std::string>(instanceGuid, configId);

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));

	socketMock->CompleteNextRead(Ok(SerializeProto(setOverridePush)));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	const auto it = std::find_if(begin, end, findConfig(instanceGuid));

	EXPECT_EQ(std::distance(begin, end), 1);
	ASSERT_NE(it, end);

	const auto& [id, result] = *it;
	
	expectedConfiguration.ResetOverride(configId);

	EXPECT_EQ(result.Configuration, expectedConfiguration);
}

template<typename TType>
void TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage(const TType& aOverrideValue)
{
	static constexpr auto	instanceName		= "instance";
	static constexpr Guid	instanceGuid		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	configId			= "test_id";

	static constexpr auto	instanceGuidString	= instanceGuid.ToFixedSizeString();

	Configuration configuration;

	configuration.Set<TType>(configId, TType(), EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ instanceGuid, instanceName, configuration } });

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));

	const auto setOverridePush = MakePushDownSetOverride<TType>(instanceGuid, configId, aOverrideValue);
	
	EXPECT_CALL(*socketMock, Write(ProtobufMessageMatcher(setOverridePush)));

	configRemoteSys->SetOverride<TType>(instanceGuid, configId, aOverrideValue);
}

TEST(ConfigurationRemoteSystemTests, setting_a_process_override_sends_a_push_down_set_override_message)
{
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<bool>			(true);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<f32>			(1.f);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<f64>			(1.);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<u8>				(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<u16>			(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<u32>			(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<u64>			(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<i8>				(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<i16>			(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<i32>			(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<i64>			(1);
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<std::string>	("override");
	TestSettingAProcessOverrideSendsAPushDownSetOverrideMessage<FilePath>		(FilePath(FilePath::EFolder::Bin, "override"));
}

TEST(ConfigurationRemoteSystemTests, resetting_a_process_override_sends_a_push_down_set_override_message)
{
	static constexpr auto instanceName			= "instance";
	static constexpr Guid instanceGuid			= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto configId				= "test_id";

	static constexpr auto instanceGuidString	= instanceGuid.ToFixedSizeString();

	Configuration configuration;

	configuration.Set<std::string>(configId, "value", EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock = MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMock->DelegateToFake();

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMock));

	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ instanceGuid, instanceName, configuration } });

	socketMock->CompleteNextRead(Ok(SerializeProto(instancesAdded)));

	const auto setOverridePush = MakePushDownSetOverride<std::string>(instanceGuid, configId);

	EXPECT_CALL(*socketMock, Write(ProtobufMessageMatcher(setOverridePush)));

	configRemoteSys->ResetOverride(instanceGuid, configId);
}

template<typename TType>
void TestSettingAnOverrideSendsAPushUpSetOverrideMessage(const TType& aOverrideValue)
{
	static constexpr Guid instanceGuid	= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto configId		= "test_id";

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem(instanceGuid);

	configSys->Set<TType>(configId, TType(), EConfigGroup::Application);
	
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	configRemoteSys->Initialize();
	socketMock->DelegateToFake();

	configRemoteSys->ConnectTo(socketMock);

	const auto setOverridePush = MakePushUpSetOverride<TType>(instanceGuid, configId, aOverrideValue);
	
	EXPECT_CALL(*socketMock, Write(ProtobufMessageMatcher(setOverridePush)));

	configSys->SetOverride<TType>(configId, aOverrideValue);
}

TEST(ConfigurationRemoteSystemTests, setting_an_override_sends_a_push_up_set_override_message)
{
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<bool>			(true);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<f32>			(1.f);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<f64>			(1.);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<u8>				(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<u16>			(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<u32>			(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<u64>			(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<i8>				(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<i16>			(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<i32>			(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<i64>			(1);
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<std::string>	("override");
	TestSettingAnOverrideSendsAPushUpSetOverrideMessage<FilePath>		(FilePath(FilePath::EFolder::Bin, "override"));
}

TEST(ConfigurationRemoteSystemTests, resetting_an_override_sends_a_push_up_set_override_message)
{
	static constexpr Guid instanceGuid	= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto configId		= "test_id";
	
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem(instanceGuid);

	configSys->Set<std::string>(configId, "value", EConfigGroup::Application);
	
	configSys->SetOverride<std::string>(configId, "override");

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	configRemoteSys->Initialize();
	socketMock->DelegateToFake();

	configRemoteSys->ConnectTo(socketMock);

	const auto setOverridePush = MakePushUpSetOverride<std::string>(instanceGuid, configId);

	EXPECT_CALL(*socketMock, Write(ProtobufMessageMatcher(setOverridePush)));

	configSys->ResetOverride(configId);
}

template<typename TType>
void TestPushDownSetOverrideSetsOverride(const TType& aOverrideValue)
{
	static constexpr Guid	instanceGuid	= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	configId		= "test_id";

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem(instanceGuid);

	configSys->Set<TType>(configId, TType(), EConfigGroup::Application);
	
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	configRemoteSys->ConnectTo(socketMock);

	const auto setOverridePush = MakePushDownSetOverride<TType>(instanceGuid, configId, aOverrideValue);

	socketMock->CompleteNextRead(Ok(SerializeProto(setOverridePush)));

	EXPECT_EQ(configSys->Get<TType>(configId, TType()), aOverrideValue);
}

TEST(ConfigurationRemoteSystemTests, push_down_set_override_sets_override)
{
	TestPushDownSetOverrideSetsOverride<bool>			(true);
	TestPushDownSetOverrideSetsOverride<f32>			(1.f);
	TestPushDownSetOverrideSetsOverride<f64>			(1.);
	TestPushDownSetOverrideSetsOverride<u8>				(1);
	TestPushDownSetOverrideSetsOverride<u16>			(1);
	TestPushDownSetOverrideSetsOverride<u32>			(1);
	TestPushDownSetOverrideSetsOverride<u64>			(1);
	TestPushDownSetOverrideSetsOverride<i8>				(1);
	TestPushDownSetOverrideSetsOverride<i16>			(1);
	TestPushDownSetOverrideSetsOverride<i32>			(1);
	TestPushDownSetOverrideSetsOverride<i64>			(1);
	TestPushDownSetOverrideSetsOverride<std::string>	("override");
	TestPushDownSetOverrideSetsOverride<FilePath>		(FilePath(FilePath::EFolder::Bin, "override"));
}

TEST(ConfigurationRemoteSystemTests, push_down_set_override_resets_override)
{
	static constexpr Guid instanceGuid			= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto configId				= "test_id";
	static constexpr auto configValue			= "value";
	
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem(instanceGuid);

	configSys->Set<std::string>(configId, configValue, EConfigGroup::Application);

	configSys->SetOverride<std::string>(configId, "override");
	
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	configRemoteSys->ConnectTo(socketMock);

	const auto setOverridePush = MakePushDownSetOverride<std::string>(instanceGuid, configId);

	socketMock->CompleteNextRead(Ok(SerializeProto(setOverridePush)));

	EXPECT_EQ(configSys->Get<std::string>(configId, "default"), configValue);
}

TEST(ConfigurationRemoteSystemTests, previous_socket_is_removed_when_connectto_is_called_with_nullptr)
{
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();
	auto [promise, future] = MakeAsync<Socket::ResultTypeRead>();
	std::weak_ptr<NiceMock<SocketMock>> weakSocket = socketMock;

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(ByMove(std::move(future))));

	configRemoteSys->ConnectTo(std::move(socketMock));

	EXPECT_TRUE(promise.IsValid());
	EXPECT_FALSE(weakSocket.expired());

	configRemoteSys->ConnectTo(nullptr);

	EXPECT_FALSE(promise.IsValid());
	EXPECT_TRUE(weakSocket.expired());
}

TEST(ConfigurationRemoteSystemTests, previous_socket_is_removed_when_connectto_is_called_with_new_socket)
{
	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();
	auto [promise, future] = MakeAsync<Socket::ResultTypeRead>();
	std::weak_ptr<NiceMock<SocketMock>> weakSocket = socketMock;

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(ByMove(std::move(future))));

	configRemoteSys->ConnectTo(std::move(socketMock));

	EXPECT_TRUE(promise.IsValid());
	EXPECT_FALSE(weakSocket.expired());

	configRemoteSys->ConnectTo(MakeSharedReference<NiceMock<SocketMock>>());

	EXPECT_FALSE(promise.IsValid());
	EXPECT_TRUE(weakSocket.expired());
}

TEST(ConfigurationRemoteSystemTests, all_process_configurations_are_passed_up_between_processes)
{
	static constexpr auto	expectedInstanceId		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	expectedInstanceName	= "instance_name";
	Configuration			expectedConfiguration;
	static constexpr auto	configId				= "test_id";
	
	expectedConfiguration.Set<std::string>(configId, "value", EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock	= MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMockUp		= MakeSharedReference<NiceMock<SocketMock>>();
	auto socketMockDown		= MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMockUp->DelegateToFake();
	socketMockDown->DelegateToFake();
	
	const auto instancesAdded = MakeInstancesAdded({ SInstanceData{ expectedInstanceId, expectedInstanceName, expectedConfiguration } });

	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMockDown));

	configRemoteSys->ConnectTo(socketMockUp);

	EXPECT_CALL(*socketMockUp, Write(ProtobufMessageMatcher(instancesAdded)));

	socketMockDown->CompleteNextRead(Ok(SerializeProto(instancesAdded)));
}

TEST(ConfigurationRemoteSystemTests, all_process_configurations_from_newly_added_config_instances_are_passed_up_between_processes)
{
	static constexpr auto	expectedInstanceId		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	expectedInstanceName	= "instance_name";
	Configuration			expectedConfiguration;
	static constexpr auto	configId				= "test_id";
	
	expectedConfiguration.Set<std::string>(configId, "value", EConfigGroup::Application);

	static constexpr auto findConfig = [](const Guid& aGuid)
	{
		return [guid = aGuid](const auto& aTuple)
		{
			const auto& [id, _] = aTuple;

			return guid == id;
		};
	};

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock	= MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMockUp		= MakeSharedReference<NiceMock<SocketMock>>();
	auto socketMockDown		= MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMockUp->DelegateToFake();
	socketMockDown->DelegateToFake();
	
	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);
	configRemoteSys->ConnectTo(socketMockUp);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMockDown));

	const auto instanceAdded = MakeInstancesAdded({ SInstanceData{ expectedInstanceId, expectedInstanceName, expectedConfiguration } });

	EXPECT_CALL(*socketMockUp, Write(ProtobufMessageMatcher(instanceAdded)));

	socketMockDown->CompleteNextRead(Ok(SerializeProto(instanceAdded)));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();

	const auto it = std::find_if(begin, end, findConfig(expectedInstanceId));

	EXPECT_EQ(std::distance(begin, end), 1);
	ASSERT_NE(it, end);

	const auto& [id, result] = *it;

	EXPECT_EQ(result.InstanceName,	expectedInstanceName);
	EXPECT_EQ(result.Configuration,	expectedConfiguration);
}

TEST(ConfigurationRemoteSystemTests, all_process_configurations_from_newly_removed_config_instances_are_passed_up_between_processes)
{
	static constexpr auto	expectedInstanceId		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	expectedInstanceName	= "instance_name";
	Configuration			expectedConfiguration;
	static constexpr auto	configId				= "test_id";
	
	expectedConfiguration.Set<std::string>(configId, "value", EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock	= MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMockUp		= MakeSharedReference<NiceMock<SocketMock>>();
	auto socketMockDown		= MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMockUp->DelegateToFake();
	socketMockDown->DelegateToFake();
	
	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);
	configRemoteSys->ConnectTo(socketMockUp);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMockDown));

	const auto instanceAdded = MakeInstancesAdded({ SInstanceData{ expectedInstanceId, expectedInstanceName, expectedConfiguration } });
	const auto instanceRemoved = MakeInstancesRemoved({ expectedInstanceId });

	socketMockDown->CompleteNextRead(Ok(SerializeProto(instanceAdded)));

	EXPECT_CALL(*socketMockUp, Write(ProtobufMessageMatcher(instanceRemoved)));

	socketMockDown->CompleteNextRead(Ok(SerializeProto(instanceRemoved)));

	auto begin = configRemoteSys->begin();
	auto end = configRemoteSys->end();
	
	ASSERT_EQ(begin, end);
}

template<typename TType>
void TestPushUpSetOverridePassedBetweenProcesses(const TType& aOverrideValue)
{
	static constexpr auto	expectedInstanceId		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	configId				= "test_id";
	Configuration			expectedConfiguration;
	
	expectedConfiguration.Set<TType>(configId, TType(), EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock	= MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMockUp		= MakeSharedReference<NiceMock<SocketMock>>();
	auto socketMockDown		= MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMockUp->DelegateToFake();
	socketMockDown->DelegateToFake();
	
	configRemoteSys->ConnectTo(socketMockUp);
	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMockDown));

	const auto instanceAdded = MakeInstancesAdded({ SInstanceData{ expectedInstanceId, "INSTANCE_NAME", expectedConfiguration } });
	const auto setOverridePush = MakePushUpSetOverride<TType>(expectedInstanceId, configId, aOverrideValue);

	socketMockDown->CompleteNextRead(Ok(SerializeProto(instanceAdded)));

	EXPECT_CALL(*socketMockUp, Write(ProtobufMessageMatcher(setOverridePush)));

	socketMockDown->CompleteNextRead(Ok(SerializeProto(setOverridePush)));
}

TEST(ConfigurationRemoteSystemTests, push_up_set_override_is_passed_between_processes)
{
	TestPushUpSetOverridePassedBetweenProcesses<bool>			(true);
	TestPushUpSetOverridePassedBetweenProcesses<f32>			(1.f);
	TestPushUpSetOverridePassedBetweenProcesses<f64>			(1.);
	TestPushUpSetOverridePassedBetweenProcesses<u8>				(1);
	TestPushUpSetOverridePassedBetweenProcesses<u16>			(1);
	TestPushUpSetOverridePassedBetweenProcesses<u32>			(1);
	TestPushUpSetOverridePassedBetweenProcesses<u64>			(1);
	TestPushUpSetOverridePassedBetweenProcesses<i8>				(1);
	TestPushUpSetOverridePassedBetweenProcesses<i16>			(1);
	TestPushUpSetOverridePassedBetweenProcesses<i32>			(1);
	TestPushUpSetOverridePassedBetweenProcesses<i64>			(1);
	TestPushUpSetOverridePassedBetweenProcesses<std::string>	("override");
	TestPushUpSetOverridePassedBetweenProcesses<FilePath>		(FilePath(FilePath::EFolder::Bin, "override"));
}

template<typename TType>
void TestPushDownSetOverridePassedBetweenProcesses(const TType& aOverrideValue)
{
	static constexpr auto	expectedInstanceId		= *Guid::Parse("38d1d3df-82ba-4f8a-a30e-510110a94a0d");
	static constexpr auto	configId				= "test_id";
	Configuration			expectedConfiguration;
	
	expectedConfiguration.Set<TType>(configId, TType(), EConfigGroup::Application);

	auto [configSys, processSys, configRemoteSys] = MakeConfigurationRemoteSystem();
	auto socketAcceptorMock	= MakeSharedReference<NiceMock<SocketAcceptorMock>>();
	auto socketMockUp		= MakeSharedReference<NiceMock<SocketMock>>();
	auto socketMockDown		= MakeSharedReference<NiceMock<SocketMock>>();

	socketAcceptorMock->DelegateToFake();
	socketMockUp->DelegateToFake();
	socketMockDown->DelegateToFake();
	
	configRemoteSys->AcceptConnectionFrom(socketAcceptorMock);
	configRemoteSys->ConnectTo(socketMockUp);

	socketAcceptorMock->CompleteNextConnection(Ok(socketMockDown));

	const auto instanceAdded = MakeInstancesAdded({ SInstanceData{ expectedInstanceId, "INSTANCE_NAME", expectedConfiguration } });
	const auto setOverridePush = MakePushDownSetOverride<TType>(expectedInstanceId, configId, aOverrideValue);

	socketMockDown->CompleteNextRead(Ok(SerializeProto(instanceAdded)));

	EXPECT_CALL(*socketMockDown, Write(ProtobufMessageMatcher(setOverridePush)));

	socketMockUp->CompleteNextRead(Ok(SerializeProto(setOverridePush)));
}

TEST(ConfigurationRemoteSystemTests, push_down_set_override_is_passed_between_processes)
{
	TestPushDownSetOverridePassedBetweenProcesses<bool>			(true);
	TestPushDownSetOverridePassedBetweenProcesses<f32>			(1.f);
	TestPushDownSetOverridePassedBetweenProcesses<f64>			(1.);
	TestPushDownSetOverridePassedBetweenProcesses<u8>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<u16>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<u32>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<u64>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<i8>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<i16>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<i32>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<i64>			(1);
	TestPushDownSetOverridePassedBetweenProcesses<std::string>	("override");
	TestPushDownSetOverridePassedBetweenProcesses<FilePath>		(FilePath(FilePath::EFolder::Bin, "override"));
}
