#pragma once

#include "wondermake-io/SocketProtobuf.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/Endian.h"
#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/Result.h"
#include "wondermake-utility/SharedReference.h"

#include <mutex>
#include <span>

template<typename TProtobufWriteMessage, typename TProtobufReadMessage>
class SocketProtobufImpl
	: public SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>
	, public std::enable_shared_from_this<SocketProtobufImpl<TProtobufWriteMessage, TProtobufReadMessage>>
{
public:
	SocketProtobufImpl(
		AnyExecutor aExecutor,
		MemoryUnit<EMemoryRatio::Bytes, size_t> aBufferMaxSize,
		SharedReference<Socket> aSocket)
		: myExecutor(std::move(aExecutor))
		, myBufferMaxSize(aBufferMaxSize)
		, mySocket(std::move(aSocket))
	{}
	
	SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeWrite WriteMessage(const TProtobufWriteMessage& aMessage) override
	{
		return mySocket->Write(SerializeMessage(aMessage));
	}
	SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeRead ReadMessage() override
	{
		std::lock_guard lock(myMutex);

		return ParseBuffer();
	}

	SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeClose OnClose() override
	{
		return mySocket->OnClose();
	}
	SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeClose Close() override
	{
		return mySocket->Close();
	}

	Socket::EState GetState() const noexcept override
	{
		return mySocket->GetState();
	}

private:
	static std::vector<u8> SerializeMessage(const TProtobufWriteMessage& aMessage) noexcept
	{
		const u32 size = static_cast<u32>(aMessage.ByteSizeLong());

		std::vector<u8> serializedData(sizeof(size) + size, 0);

		*reinterpret_cast<u32*>(serializedData.data()) = ToEndianNetwork(size);

		aMessage.SerializeToArray(serializedData.data() + sizeof(size), static_cast<int>(serializedData.size()));

		return serializedData;
	}
	static Result<std::pair<TProtobufReadMessage, size_t>> DeserializeMessage(std::span<const u8> aData) noexcept
	{
		TProtobufReadMessage readMessage;

		const u32 size = ToEndianHost(*reinterpret_cast<const u32*>(aData.data()));

		if (aData.size() < sizeof(size) + size)
			return Err();

		const bool result = readMessage.ParseFromArray(aData.data() + sizeof(size), static_cast<int>(size));

		if (!result)
			return Err();

		const size_t readBytes = sizeof(size) + static_cast<size_t>(size);

		return Ok(std::make_pair(std::move(readMessage), readBytes));
	}

	SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeRead ParseBuffer()
	{
		using ResultTypeRead = SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::ResultTypeRead;
		using FutureTypeRead = SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeRead;

		const auto makeThenApply = [this]()
		{
			return FutureApplyResult(Bind(&SocketProtobufImpl<TProtobufWriteMessage, TProtobufReadMessage>::OnRead, this->weak_from_this()));
		};

		if (myReadBuffer.empty())
		{
			return mySocket->Read()
				.ThenApply(myExecutor, makeThenApply());
		}

		const auto maxBytes = myBufferMaxSize.To<EMemoryRatio::Bytes>();

		const auto endIt = myReadBuffer.size() <= maxBytes ? myReadBuffer.end() : myReadBuffer.begin() + maxBytes;

		auto result = DeserializeMessage(std::span(myReadBuffer.begin(), endIt));

		if (!result)
		{
			if (myReadBuffer.size() >= maxBytes)
				return MakeCompletedFuture<ResultTypeRead>(Err(Socket::SReadError{ Socket::EReadError::MessageToBig }));

			return mySocket->Read()
				.ThenApply(myExecutor, makeThenApply());
		}

		auto serializable = std::move(result).Unwrap();

		myReadBuffer.erase(myReadBuffer.begin(), myReadBuffer.begin() + serializable.second);

		return MakeCompletedFuture<ResultTypeRead>(Ok(std::move(serializable.first)));
	}
	SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::FutureTypeRead OnRead(Socket::ResultTypeRead aResult)
	{
		std::lock_guard lock(myMutex);

		if (!aResult)
			return MakeCompletedFuture<SocketProtobuf<TProtobufWriteMessage, TProtobufReadMessage>::ResultTypeRead>(std::move(aResult));

		auto buffer = std::move(aResult).Unwrap();

		myReadBuffer.insert(myReadBuffer.end(), buffer.begin(), buffer.end());

		return ParseBuffer();
	}
	
	std::recursive_mutex myMutex;
	AnyExecutor myExecutor;
	const MemoryUnit<EMemoryRatio::Bytes, size_t> myBufferMaxSize;
	SharedReference<Socket> mySocket;
	std::vector<u8> myReadBuffer;

};
