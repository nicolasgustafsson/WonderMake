#include "wondermake-engine/ConfigurationRemoteSystem.h"

#include "ConfigurationProtoSerialization.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-io/SocketProtobufImpl.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/GuidGeneratorSystem.h"
#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/MemoryUnit.h"

REGISTER_SYSTEM(ConfigurationRemoteSystem);

using ProtoConfigurationRemote::Upstream;
using ProtoConfigurationRemote::Downstream;

using namespace MemoryUnitLiterals;

inline constexpr auto locDefaultBufferSize = 4_KiB;

ConfigurationRemoteSystem::ConfigurationRemoteSystem()
	: myInstanceGuid(*Get<GuidGeneratorSystem>().GenerateNew())
{
	myInstanceName = ("Instance:" + myInstanceGuid.ToFixedSizeString())
		.ToString();
}

void ConfigurationRemoteSystem::Initialize()
{
	myOverrideSubscriber = Get<ConfigurationSystem>()
		.OnOverrideChanged(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnOverrideChanged, weak_from_this()));
}

void ConfigurationRemoteSystem::AcceptConnectionFrom(std::shared_ptr<SocketAcceptor> aAcceptor)
{
	auto resultAcceptor =  SharedReference<SocketAcceptor>::FromPointer(std::move(aAcceptor));

	if (!resultAcceptor)
	{
		myConnectionFuture.Reset();

		myAcceptor.reset();

		return;
	}

	auto acceptor = std::move(resultAcceptor).Unwrap();

	if (acceptor->GetState() != SocketAcceptor::EState::Open)
	{
		myConnectionFuture.Reset();

		myAcceptor.reset();

		return;
	}

	myAcceptor = std::move(acceptor);

	AcceptConnection();
}

void ConfigurationRemoteSystem::ConnectTo(std::shared_ptr<Socket> aSocket)
{
	auto resultSocket = SharedReference<Socket>::FromPointer(std::move(aSocket));

	if (!resultSocket)
	{
		myUpReadFuture.Reset();

		myUpSocket.reset();

		return;
	}

	auto socket = std::move(resultSocket).Unwrap();

	if (socket->GetState() != Socket::EState::Open)
	{
		myUpReadFuture.Reset();

		myUpSocket.reset();

		return;
	}

	using socketType = SocketProtobufImpl<Upstream, Downstream>;

	const auto& configuration = Get<ConfigurationSystem>();

	const auto bufferSize = configuration.Get<ConfigurationEngine::MemoryType>(ConfigurationEngine::ConfigurationRemoteBufferSize, locDefaultBufferSize);

	myUpSocket = std::make_shared<socketType>(GetExecutor(), bufferSize, std::move(socket));

	Upstream us;

	auto& instances = *us.mutable_push()->mutable_instance_added()->mutable_instances();

	const auto addInstance = [&instances](Guid aInstanceId, const std::string& aInstanceName, const Configuration& aConfiguration)
	{
		auto& instance = instances[aInstanceId.ToFixedSizeString().ToStringView()];

		instance.set_name(aInstanceName);
		SerializeConfigurationToProto(aConfiguration, instance);
	};

	addInstance(myInstanceGuid, myInstanceName, Get<ConfigurationSystem>().GetConfiguration());

	for (const auto& connectionData : myDownConnectionList)
		for (const auto& [id, data] : connectionData.Configurations)
			addInstance(id, data.InstanceName, data.Configuration);

	myUpSocket->WriteMessage(us)
		.ThenRun(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnWriteUpMessage, this->weak_from_this()))
		.Detach();

	UpReadMessage();
}

ConfigurationRemoteSystem::Iterator ConfigurationRemoteSystem::NextIterator(const std::pair<CIteratorDataType, CIteratorConfigType>& aIterators) const noexcept
{
	auto [itData, itConfig] = aIterators;

	if (itData == myDownConnectionList.end())
		return end();

	if (itConfig != itData->Configurations.end())
		++itConfig;

	while (itConfig == itData->Configurations.end())
	{
		++itData;

		if (itData == myDownConnectionList.end())
			return end();

		itConfig = itData->Configurations.begin();
	}

	return Iterator(*this, std::make_pair(std::move(itData), std::move(itConfig)));
}

std::optional<std::pair<ConfigurationRemoteSystem::IteratorDataType, ConfigurationRemoteSystem::IteratorConfigType>> ConfigurationRemoteSystem::FindInstance(Guid aInstanceId) noexcept
{
	for (auto itData = myDownConnectionList.begin(); itData != myDownConnectionList.end(); ++itData)
	{
		auto itConfig = itData->Configurations.find(aInstanceId);

		if (itConfig != itData->Configurations.end())
			return std::make_pair(std::move(itData), std::move(itConfig));
	}

	return std::nullopt;
}

void ConfigurationRemoteSystem::OnOverrideChanged(const std::string& aId)
{
	SendUp([this, &aId]() -> std::optional<Upstream>
		{
			auto& configSystem = Get<ConfigurationSystem>();

			auto configs = configSystem.GetConfigs();

			const auto it = configs.find(aId);

			if (it == configs.end())
			{
				WmLogError(TagWonderMake << TagWmConfigurationRemote << "Missing configuration, id: " << aId << '.');

				return std::nullopt;
			}

			Upstream us;

			auto& setOverride = *us.mutable_push()->mutable_set_override();

			setOverride.set_instanceid(myInstanceGuid.ToFixedSizeString().ToCString());
			setOverride.set_id(aId);

			std::visit([&setOverride](const auto& aConfigData)
				{
					if (!aConfigData.Override)
						return;

					const auto& over = *aConfigData.Override;
					using Type = std::decay_t<decltype(aConfigData.Value)>;

					if constexpr (std::is_same_v<Type, bool>)
						setOverride.set_config_bool(over);
					else if constexpr (
						std::is_same_v<Type, f32> ||
						std::is_same_v<Type, f64>)
						setOverride.set_config_f64(over);
					else if constexpr (
						std::is_same_v<Type, u8> ||
						std::is_same_v<Type, u16> ||
						std::is_same_v<Type, u32> ||
						std::is_same_v<Type, u64>)
						setOverride.set_config_u64(over);
					else if constexpr (
						std::is_same_v<Type, i8> ||
						std::is_same_v<Type, i16> ||
						std::is_same_v<Type, i32> ||
						std::is_same_v<Type, i64>)
						setOverride.set_config_i64(over);
					else if constexpr (std::is_same_v<Type, std::string>)
						setOverride.set_config_string(over);

				}, it->second);

			return us;
		});
}

SocketAcceptor::FutureTypeConnection ConfigurationRemoteSystem::AcceptConnection()
{
	myConnectionFuture = myAcceptor->OnConnection()
		.ThenApply(GetExecutor(), FutureApplyResult(Bind(&ConfigurationRemoteSystem::OnConnection, weak_from_this())));

	return myConnectionFuture;
}

SocketAcceptor::FutureTypeConnection ConfigurationRemoteSystem::OnConnection(SocketAcceptor::ResultTypeConnection&& aResult)
{
	aResult
		.AndThen([this](auto& aConnection)
			{
				using socketType = SocketProtobufImpl<Downstream, Upstream>;

				const auto& configuration = Get<ConfigurationSystem>();

				const auto bufferSize = configuration.Get<ConfigurationEngine::MemoryType>(ConfigurationEngine::ConfigurationRemoteBufferSize, locDefaultBufferSize);

				auto& connectionData = myDownConnectionList.emplace_back(MakeSharedReference<socketType>(GetExecutor(), bufferSize, std::move(aConnection)));

				connectionData.Connection->OnClose()
					.ThenRun(GetExecutor(), FutureRunResult(Bind(&ConfigurationRemoteSystem::OnDownClosed, weak_from_this(), connectionData.Connection.Weak())))
					.Detach();

				DownReadMessage(connectionData);
			})
		.OrElse([](auto& aError)
			{
				WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to accepto connection, error: " << aError << '.');
			});

	return AcceptConnection();
}

ConfigurationRemoteSystem::SocketUpType::FutureTypeRead ConfigurationRemoteSystem::UpReadMessage()
{
	myUpReadFuture = myUpSocket->ReadMessage()
		.ThenApply(GetExecutor(), FutureApplyResult(Bind(&ConfigurationRemoteSystem::OnUpReadMessage, weak_from_this())));

	return myUpReadFuture;
}

ConfigurationRemoteSystem::SocketUpType::FutureTypeRead ConfigurationRemoteSystem::OnUpReadMessage(SocketUpType::ResultTypeRead&& aResult)
{
	return aResult
		.AndThen([this](const auto& aMessage)
			{
				ParseMessage(aMessage);

				return UpReadMessage();
			})
		.OrElse([this](const auto& aError)
			{
				WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to read from up connection, error: " << aError << '.');

				myUpReadFuture.Reset();

				myUpSocket.reset();

				return MakeCanceledFuture<SocketUpType::ResultTypeRead>();
			});
}

void ConfigurationRemoteSystem::ParseMessage(const Downstream& aMessage)
{
	if (aMessage.has_push())
	{
		const auto& push = aMessage.push();

		if (push.has_set_override())
			HandleMessage(push.set_override());
	}
}

void ConfigurationRemoteSystem::HandleMessage(const ProtoConfigurationRemote::PushDownSetOverride& aMessage)
{
	auto instanceIdOpt = Guid::Parse(aMessage.instanceid());

	if (!instanceIdOpt)
	{
		WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to parse instance id, id: " << aMessage.instanceid() << '.');

		return;
	}

	const auto instanceId = *instanceIdOpt;

	if (instanceId == myInstanceGuid)
	{
		auto& configSystem = Get<ConfigurationSystem>();

		if (aMessage.has_config_bool())
			configSystem.SetOverride<bool, true>(aMessage.id(), aMessage.config_bool());
		else if (aMessage.has_config_f64())
		{
			if (configSystem.GetConfiguration().Has<f32, true>(aMessage.id()))
				configSystem.SetOverride<f32, true>(aMessage.id(), static_cast<f32>(aMessage.config_f64()));
			else if (configSystem.GetConfiguration().Has<f64, true>(aMessage.id()))
				configSystem.SetOverride<f64, true>(aMessage.id(), static_cast<f64>(aMessage.config_f64()));
		}
		else if (aMessage.has_config_u64())
		{
			if (configSystem.GetConfiguration().Has<u8, true>(aMessage.id()))
				configSystem.SetOverride<u8, true>(aMessage.id(), static_cast<u8>(aMessage.config_u64()));
			else if (configSystem.GetConfiguration().Has<u16, true>(aMessage.id()))
				configSystem.SetOverride<u16, true>(aMessage.id(), static_cast<u16>(aMessage.config_u64()));
			else if (configSystem.GetConfiguration().Has<u32, true>(aMessage.id()))
				configSystem.SetOverride<u32, true>(aMessage.id(), static_cast<u32>(aMessage.config_u64()));
			else if (configSystem.GetConfiguration().Has<u64, true>(aMessage.id()))
				configSystem.SetOverride<u64, true>(aMessage.id(), static_cast<u64>(aMessage.config_u64()));
		}
		else if (aMessage.has_config_i64())
		{
			if (configSystem.GetConfiguration().Has<i8, true>(aMessage.id()))
				configSystem.SetOverride<i8, true>(aMessage.id(), static_cast<i8>(aMessage.config_i64()));
			else if (configSystem.GetConfiguration().Has<i16, true>(aMessage.id()))
				configSystem.SetOverride<i16, true>(aMessage.id(), static_cast<i16>(aMessage.config_i64()));
			else if (configSystem.GetConfiguration().Has<i32, true>(aMessage.id()))
				configSystem.SetOverride<i32, true>(aMessage.id(), static_cast<i32>(aMessage.config_i64()));
			else if (configSystem.GetConfiguration().Has<i64, true>(aMessage.id()))
				configSystem.SetOverride<i64, true>(aMessage.id(), static_cast<i64>(aMessage.config_i64()));
		}
		else if (aMessage.has_config_string())
			configSystem.SetOverride<std::string, true>(aMessage.id(), aMessage.config_string());
		else
			configSystem.ResetOverride(aMessage.id());

		return;
	}

	const auto itOpt = FindInstance(instanceId);

	if (!itOpt)
	{
		WmLogError(TagWonderMake << TagWmConfigurationRemote << "Missing configuration instance, id: " << instanceId << '.');

		return;
	}

	const auto& [itData, itConfig] = *itOpt;

	auto& downSocket = itData->Connection;

	Downstream ds;

	*ds.mutable_push()->mutable_set_override() = aMessage;

	downSocket->WriteMessage(ds)
		.ThenRun(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnWriteDownMessage, this->weak_from_this(), itConfig->second.InstanceName))
		.Detach();
}

ConfigurationRemoteSystem::SocketDownType::FutureTypeRead ConfigurationRemoteSystem::DownReadMessage(SConfigurationData& aConnectionData)
{
	aConnectionData.ReadFuture = aConnectionData.Connection->ReadMessage()
		.ThenApply(GetExecutor(), FutureApplyResult(Bind(&ConfigurationRemoteSystem::OnDownReadMessage, weak_from_this(), aConnectionData.Connection.Weak())));

	return aConnectionData.ReadFuture;
}

ConfigurationRemoteSystem::SocketDownType::FutureTypeRead ConfigurationRemoteSystem::OnDownReadMessage(std::weak_ptr<SocketDownType> aConnection, SocketDownType::ResultTypeRead&& aResult)
{
	auto resultConnection = SharedReference<SocketDownType>::FromPointer(aConnection.lock());

	if (!resultConnection)
		return MakeCanceledFuture<SocketDownType::ResultTypeRead>();

	auto& connection = resultConnection.Unwrap();

	const auto it = std::find_if(myDownConnectionList.begin(), myDownConnectionList.end(), [&connection](const auto& aData)
		{
			return aData.Connection == connection;
		});

	return aResult
		.AndThen([this, it](const auto& aMessage)
			{
				if (it == myDownConnectionList.end())
				{
					WmLogError(TagWonderMake << TagWmConfigurationRemote << "Missing connection data. Was the connection already closed?");

					return MakeCanceledFuture<SocketDownType::ResultTypeRead>();
				}

				ParseMessage(*it, aMessage);

				return DownReadMessage(*it);
			})
		.OrElse([this, it](const auto& aError)
			{
				if (it == myDownConnectionList.end())
				{
					WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to read from down connection, error: " << aError << ". Connection data is missing.");

					return MakeCanceledFuture<SocketDownType::ResultTypeRead>();
				}

				std::stringstream ss;

				for (const auto& [id, _] : it->Configurations)
					ss << id << ", ";

				auto idList = std::move(ss).str();

				if (!idList.empty())
				{
					idList.pop_back();
					idList.pop_back();
				}

				WmLogError(TagWonderMake << TagWmConfigurationRemote << "Missing configuration instance, error: " << aError << ", instance ids: [" << std::move(idList) << "].");

				it->ReadFuture.Reset();

				myDownConnectionList.erase(it);

				return MakeCanceledFuture<SocketDownType::ResultTypeRead>();
			});
}

void ConfigurationRemoteSystem::OnDownClosed(std::weak_ptr<SocketDownType> aConnection, SocketDownType::ResultTypeClose aResult)
{
	aResult
		.AndThen([](const auto& aLocation)
			{
				WmLogInfo(TagWonderMake << TagWmConfigurationRemote << "Connection closed, location: " << aLocation << '.');
			})
		.OrElse([](const auto& aError)
			{
				WmLogInfo(TagWonderMake << TagWmConfigurationRemote << "Connection closed, error: " << aError << '.');
			});

	auto resultConnection = SharedReference<SocketDownType>::FromPointer(aConnection.lock());

	if (!resultConnection)
		return;

	const auto& connection = resultConnection.Unwrap();

	const auto it = std::find_if(myDownConnectionList.begin(), myDownConnectionList.end(), [&connection](const auto& aData)
		{
			return aData.Connection == connection;
		});

	if (it == myDownConnectionList.end())
		return;

	SendUp([&data = *it]()
	{
		Upstream us;

		auto& instancesRemoved = *us.mutable_push()->mutable_instance_removed();

		for (const auto& [instanceId, configs] : data.Configurations)
			instancesRemoved.add_instanceids(instanceId.ToFixedSizeString().ToCString());

		return us;
	});

	myDownConnectionList.erase(it);
}

void ConfigurationRemoteSystem::ParseMessage(SConfigurationData& aConnectionData, const Upstream& aMessage)
{
	if (aMessage.has_push())
	{
		const auto& push = aMessage.push();

		if (push.has_set_override())
			HandleMessage(aConnectionData, push.set_override());
		else if (push.has_instance_added())
			HandleMessage(aConnectionData, push.instance_added());
		else if (push.has_instance_removed())
			HandleMessage(aConnectionData, push.instance_removed());
	}
}

void ConfigurationRemoteSystem::HandleMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::PushUpSetOverride& aMessage)
{
	auto instanceIdOpt = Guid::Parse(aMessage.instanceid());

	if (!instanceIdOpt)
	{
		WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to parse instance id, id: " << aMessage.instanceid() << '.');

		return;
	}

	const auto instanceId = *instanceIdOpt;

	const auto it = aConnectionData.Configurations.find(instanceId);

	if (it == aConnectionData.Configurations.cend())
	{
		WmLogError(TagWonderMake << TagWmConfigurationRemote << "Connection missing instance, id: " << instanceId << '.');

		return;
	}

	auto& config = it->second.Configuration;
	static constexpr bool raw = true;

	if (aMessage.has_config_bool())
		config.SetOverride<bool, raw>(aMessage.id(), aMessage.config_bool());
	else if (aMessage.has_config_f64())
	{
		if (config.Has<f32, raw>(aMessage.id()))
			config.SetOverride<f32, raw>(aMessage.id(), static_cast<f32>(aMessage.config_f64()));
		else if (config.Has<f64, raw>(aMessage.id()))
			config.SetOverride<f64, raw>(aMessage.id(), static_cast<f64>(aMessage.config_f64()));
	}
	else if (aMessage.has_config_u64())
	{
		if (config.Has<u8, raw>(aMessage.id()))
			config.SetOverride<u8, raw>(aMessage.id(), static_cast<u8>(aMessage.config_u64()));
		else if (config.Has<u16, raw>(aMessage.id()))
			config.SetOverride<u16, raw>(aMessage.id(), static_cast<u16>(aMessage.config_u64()));
		else if (config.Has<u32, raw>(aMessage.id()))
			config.SetOverride<u32, raw>(aMessage.id(), static_cast<u32>(aMessage.config_u64()));
		else if (config.Has<u64, raw>(aMessage.id()))
			config.SetOverride<u64, raw>(aMessage.id(), static_cast<u64>(aMessage.config_u64()));
	}
	else if (aMessage.has_config_i64())
	{
		if (config.Has<i8, raw>(aMessage.id()))
			config.SetOverride<i8, raw>(aMessage.id(), static_cast<i8>(aMessage.config_i64()));
		else if (config.Has<i16, raw>(aMessage.id()))
			config.SetOverride<i16, raw>(aMessage.id(), static_cast<i16>(aMessage.config_i64()));
		else if (config.Has<i32, raw>(aMessage.id()))
			config.SetOverride<i32, raw>(aMessage.id(), static_cast<i32>(aMessage.config_i64()));
		else if (config.Has<i64, raw>(aMessage.id()))
			config.SetOverride<i64, raw>(aMessage.id(), static_cast<i64>(aMessage.config_i64()));
	}
	else if (aMessage.has_config_string())
		config.SetOverride<std::string, raw>(aMessage.id(), aMessage.config_string());
	else
		config.ResetOverride(aMessage.id());

	SendUp([&aMessage]()
		{
			Upstream us;

			*us.mutable_push()->mutable_set_override() = aMessage;

			return us;
		});
}

void ConfigurationRemoteSystem::HandleMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::PushUpInstancesAdded& aMessage)
{
	for (const auto& [instanceIdString, instance] : aMessage.instances())
	{
		const auto instanceIdOpt = Guid::Parse(instanceIdString);

		if (!instanceIdOpt)
		{
			WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to parse instance id, id: " << instanceIdString << '.');

			continue;
		}

		const auto instanceId = *instanceIdOpt;

		const auto hasInstance = [this, &instanceId, &instance]()
		{
			for (auto& connectionData : myDownConnectionList)
			{
				const auto it = connectionData.Configurations.find(instanceId);

				if (it != connectionData.Configurations.cend())
				{
					WmLogError(TagWonderMake << TagWmConfigurationRemote << "Attempted to add already existing instance, id: " << instanceId << ", existing name: " << it->second.InstanceName << ", incoming name: " << instance.name() << '.');

					return true;
				}
			}

			return false;
		};

		if (hasInstance())
			continue;

		auto& data = aConnectionData.Configurations[instanceId];

		data.InstanceName = instance.name();
		DeserializeConfigurationFromProto(instance, data.Configuration);
	}

	SendUp([&aMessage]()
		{
			Upstream us;

			*us.mutable_push()->mutable_instance_added() = aMessage;

			return us;
		});
}

void ConfigurationRemoteSystem::HandleMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::PushUpInstancesRemoved& aMessage)
{
	for (const auto& instanceIdString : aMessage.instanceids())
	{
		const auto instanceIdOpt = Guid::Parse(instanceIdString);

		if (!instanceIdOpt)
		{
			WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to parse instance id, id: " << instanceIdString << '.');

			continue;
		}

		const auto instanceId = *instanceIdOpt;

		const auto it = aConnectionData.Configurations.find(*instanceIdOpt);

		if (it == aConnectionData.Configurations.cend())
		{
			WmLogError(TagWonderMake << TagWmConfigurationRemote << "Attempted to remove missing instance, id: " << instanceId << '.');

			continue;
		}

		aConnectionData.Configurations.erase(it);
	}

	SendUp([&aMessage]()
		{
			Upstream us;

			*us.mutable_push()->mutable_instance_removed() = aMessage;

			return us;
		});
}

void ConfigurationRemoteSystem::SendUp(auto aGetMessage)
{
	if (!myUpSocket || myUpSocket->GetState() != Socket::EState::Open)
		return;

	auto us = aGetMessage();

	if constexpr (std::is_same_v<std::decay_t<decltype(us)>, std::optional<Upstream>>)
	{
		if (!us)
			return;

		myUpSocket->WriteMessage(*us)
			.ThenRun(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnWriteUpMessage, this->weak_from_this()))
			.Detach();
	}
	else
	{
		myUpSocket->WriteMessage(us)
			.ThenRun(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnWriteUpMessage, this->weak_from_this()))
			.Detach();
	}
}

void ConfigurationRemoteSystem::OnWriteUpMessage(const SocketUpType::FutureTypeWrite& aFuture)
{
	auto resultOpt = aFuture.GetResult();

	if (!resultOpt)
		return;

	const auto& result = *resultOpt;

	if (result)
		return;

	WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to write to up connection, error: " << result.Err() << '.');
}

void ConfigurationRemoteSystem::OnWriteDownMessage(std::string aInstanceName, const SocketDownType::FutureTypeWrite& aFuture)
{
	auto resultOpt = aFuture.GetResult();

	if (!resultOpt)
		return;

	const auto& result = *resultOpt;

	if (result)
		return;

	WmLogError(TagWonderMake << TagWmConfigurationRemote << "Failed to write to down connection, instance name: " << aInstanceName << ", error: " << result.Err() << '.');
}
