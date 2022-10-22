#pragma once

#include "wondermake-io/Socket.h"
#include "wondermake-io/SocketProtobuf.h"
#include "wondermake-io/SocketAcceptor.h"

#include "wondermake-base/Configuration.h"
#include "wondermake-base/System.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Guid.h"
#include "wondermake-utility/SharedReference.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/ConfigurationRemote.pb.h"
#pragma warning(pop)

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ConfigurationSystem;
class ProcessSystem;

class ConfigurationRemoteSystem
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<ProcessSystem, PWrite>>>
	, public std::enable_shared_from_this<ConfigurationRemoteSystem>
{
public:
	struct SConfiguration
	{
		std::string		InstanceName;
		Configuration	Configuration;
	};

private:
	using SocketUpType		= SocketProtobuf<ProtoConfigurationRemote::Upstream, ProtoConfigurationRemote::Downstream>;
	using SocketDownType	= SocketProtobuf<ProtoConfigurationRemote::Downstream, ProtoConfigurationRemote::Upstream>;

	struct SConfigurationData
	{
		SConfigurationData(SharedReference<SocketDownType> aConnection) noexcept
			: Connection(std::move(aConnection))
		{}
		SConfigurationData(SConfigurationData&&) noexcept = default;
		SConfigurationData& operator=(SConfigurationData&&) noexcept = default;

		SharedReference<SocketDownType>				Connection;
		SocketDownType::FutureTypeRead				ReadFuture;
		std::unordered_map<Guid, SConfiguration>	Configurations;
	};

	std::vector<SConfigurationData> myDownConnectionList;
	
	using IteratorDataType		= std::vector<SConfigurationData>::iterator;
	using IteratorConfigType	= std::unordered_map<Guid, SConfiguration>::iterator;
	
	using CIteratorDataType		= std::vector<SConfigurationData>::const_iterator;
	using CIteratorConfigType	= std::unordered_map<Guid, SConfiguration>::const_iterator;

public:
	class Iterator
	{
	public:
		using iterator_category	= std::forward_iterator_tag;
		using difference_type	= std::ptrdiff_t;
		using value_type		= CIteratorConfigType::value_type;

		using pointer			= CIteratorConfigType::pointer;
		using reference			= CIteratorConfigType::reference;

		inline Iterator(const ConfigurationRemoteSystem& aSystem, std::optional<std::pair<CIteratorDataType, CIteratorConfigType>> aIterators) noexcept
			: mySystem(&aSystem)
			, myIterators(std::move(aIterators))
		{}
		inline Iterator(const Iterator&) noexcept = default;
		inline Iterator& operator=(const Iterator&) noexcept = default;

		inline bool operator==(const Iterator&) const noexcept = default;
		inline bool operator!=(const Iterator&) const noexcept = default;

		inline Iterator& operator++() noexcept
		{
			if (myIterators)
				*this = mySystem->NextIterator(*myIterators);

			return *this;
		}
		inline [[nodiscard]] Iterator operator++(int) noexcept
		{
			Iterator retVal = *this;

			++(*this);

			return retVal;
		}

		inline [[nodiscard]] reference operator*() const noexcept
		{
			return *myIterators->second;
		}
		inline [[nodiscard]] pointer operator->() const noexcept
		{
			return &(*myIterators->second);
		}

	private:
		const ConfigurationRemoteSystem*									mySystem;
		std::optional<std::pair<CIteratorDataType, CIteratorConfigType>>	myIterators;

	};

	using const_iterator = Iterator;

	ConfigurationRemoteSystem();

	void Initialize() override;

	inline void SetCurrentInstanceName(std::string aInstanceName) noexcept
	{
		myInstanceName = std::move(aInstanceName);
	}
	inline [[nodiscard]] std::string_view GetCurrentInstanceName() const noexcept
	{
		return myInstanceName;
	}
	inline [[nodiscard]] Guid GetCurrentInstanceId() const noexcept
	{
		return myInstanceGuid;
	}

	void AcceptConnectionFrom(std::shared_ptr<SocketAcceptor> aAcceptor);
	void ConnectTo(std::shared_ptr<Socket> aSocket);

	inline [[nodiscard]] const_iterator begin() const noexcept
	{
		return cbegin();
	}
	inline [[nodiscard]] const_iterator end() const noexcept
	{
		return cend();
	}
	inline [[nodiscard]] const_iterator cbegin() const noexcept
	{
		if (myDownConnectionList.empty())
			return cend();

		CIteratorDataType itData = myDownConnectionList.begin();

		while (itData->Configurations.empty())
		{
			++itData;

			if (itData == myDownConnectionList.end())
				return cend();
		}

		CIteratorConfigType itConfig = itData->Configurations.begin();

		return Iterator(*this, std::make_pair(std::move(itData), std::move(itConfig)));
	}
	inline [[nodiscard]] const_iterator cend() const noexcept
	{
		return Iterator(*this, std::nullopt);
	}

	template<CConfigRaw TConfigType, typename TArg>
	inline void SetOverride(Guid aInstanceId, const std::string& aId, TArg&& aValue)
	{
		const auto itOpt = FindInstance(aInstanceId);

		if (!itOpt)
		{
			WmLogError(TagWonderMake << TagWmConfigurationRemote << "Missing configuration instance: " << aInstanceId << '.');

			return;
		}

		const auto& [itData, itConfig] = *itOpt;

		itConfig->second.Configuration.SetOverride<TConfigType>(aId, aValue);

		if (!itConfig->second.Configuration.Has<TConfigType, true>(aId))
		{
			WmLogError(TagWonderMake << TagWmConfigurationRemote << "Config type mismatch, attempted to set override type for instance name: " << itConfig->second.InstanceName << ", type: " << typeid(TConfigType).name() << ", with config id: " << aId << '.');

			return;
		}

		const auto guidString = aInstanceId.ToFixedSizeString();
		const auto setConfig = [&aValue](auto& aSetOverrideMessage)
		{
			if constexpr (std::is_same_v<TConfigType, bool>)
				aSetOverrideMessage.set_config_bool(static_cast<bool>(aValue));
			else if constexpr (std::is_floating_point_v<TConfigType>)
				aSetOverrideMessage.set_config_f64(static_cast<f64>(aValue));
			else if constexpr (std::is_unsigned_v<TConfigType>)
				aSetOverrideMessage.set_config_u64(static_cast<u64>(aValue));
			else if constexpr (std::is_signed_v<TConfigType>)
				aSetOverrideMessage.set_config_i64(static_cast<i64>(aValue));
			else if constexpr (std::is_same_v<TConfigType, std::string>)
				aSetOverrideMessage.set_config_string(std::forward<TArg>(aValue));
			else if constexpr (std::is_same_v<TConfigType, FilePath>)
				SetFilePath(aValue, *aSetOverrideMessage.mutable_config_filepath());
		};

		auto& downSocket = itData->Connection;

		if (downSocket->GetState() == Socket::EState::Open)
		{
			ProtoConfigurationRemote::Downstream ds;

			auto& push = *ds.mutable_push();

			auto& setOverride = *push.mutable_set_override();

			setOverride.set_instanceid(guidString.ToString());
			setOverride.set_id(aId);
			setConfig(setOverride);

			downSocket->WriteMessage(ds)
				.ThenRun(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnWriteDownMessage, this->weak_from_this(), itConfig->second.InstanceName))
				.Detach();
		}
	}
	inline void ResetOverride(Guid aInstanceId, const std::string& aId)
	{
		const auto itOpt = FindInstance(aInstanceId);

		if (!itOpt)
		{
			WmLogError(TagWonderMake << TagWmConfigurationRemote << "Missing configuration instance: " << aInstanceId << '.');

			return;
		}

		const auto& [itData, itConfig] = *itOpt;

		itConfig->second.Configuration.ResetOverride(aId);

		const auto guidString = aInstanceId.ToFixedSizeString();

		auto& downSocket = itData->Connection;

		if (downSocket->GetState() == Socket::EState::Open)
		{
			ProtoConfigurationRemote::Downstream ds;

			auto& push = *ds.mutable_push();

			auto& setOverride = *push.mutable_set_override();

			setOverride.set_instanceid(guidString.ToString());
			setOverride.set_id(aId);

			downSocket->WriteMessage(ds)
				.ThenRun(GetExecutor(), Bind(&ConfigurationRemoteSystem::OnWriteDownMessage, this->weak_from_this(), itConfig->second.InstanceName))
				.Detach();
		}
	}

private:
	static void SetFilePath(const FilePath& aFilePath, ProtoConfigurationRemote::FilePathT& aOutFilePath);
	static FilePath GetFilePath(const ProtoConfigurationRemote::FilePathT& aFilePath);

	[[nodiscard]] Iterator NextIterator(const std::pair<CIteratorDataType, CIteratorConfigType>& aIterators) const noexcept;
	[[nodiscard]] std::optional<std::pair<IteratorDataType, IteratorConfigType>> FindInstance(Guid aInstanceId) noexcept;

	void OnOverrideChanged(const std::string& aId);

	SocketAcceptor::FutureTypeConnection AcceptConnection();
	SocketAcceptor::FutureTypeConnection OnConnection(SocketAcceptor::ResultTypeConnection&& aResult);

	SocketUpType::FutureTypeRead UpReadMessage();
	SocketUpType::FutureTypeRead OnUpReadMessage(SocketUpType::ResultTypeRead&& aResult);

	void ParseMessage(const ProtoConfigurationRemote::Downstream&			aMessage);
	void HandleMessage(const ProtoConfigurationRemote::PushDownSetOverride&	aMessage);
	
	SocketDownType::FutureTypeRead DownReadMessage(SConfigurationData& aConnectionData);
	SocketDownType::FutureTypeRead OnDownReadMessage(std::weak_ptr<SocketDownType> aConnection, SocketDownType::ResultTypeRead&& aResult);
	void OnDownClosed(std::weak_ptr<SocketDownType> aConnection, SocketDownType::ResultTypeClose aResult);
	
	void ParseMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::Upstream&				aMessage);
	void HandleMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::PushUpSetOverride&		aMessage);
	void HandleMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::PushUpInstancesAdded&	aMessage);
	void HandleMessage(SConfigurationData& aConnectionData, const ProtoConfigurationRemote::PushUpInstancesRemoved&	aMessage);

	void SendUp(auto aGetMessage);
	void OnWriteUpMessage(const SocketUpType::FutureTypeWrite& aFuture);
	void OnWriteDownMessage(std::string aInstanceName, const SocketDownType::FutureTypeWrite& aFuture);

	Guid			myInstanceGuid;
	std::string		myInstanceName;
	EventSubscriber	myOverrideSubscriber;

	std::shared_ptr<SocketAcceptor>	myAcceptor;
	std::shared_ptr<SocketUpType>	myUpSocket;

	SocketAcceptor::FutureTypeConnection	myConnectionFuture;
	SocketUpType::FutureTypeRead			myUpReadFuture;

};
