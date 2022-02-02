#pragma once

#include "WonderMakeIo/SocketSerializing.h"

#include "WonderMakeUtility/Bindable.h"
#include "WonderMakeUtility/MemoryUnit.h"
#include "WonderMakeUtility/SharedReference.h"

#include <functional>

template<typename TSerializable>
class SocketSerializingImpl
	: public SocketSerializing<TSerializable>
	, public std::enable_shared_from_this<SocketSerializingImpl<TSerializable>>
{
public:
	SocketSerializingImpl(
		MemoryUnit<EMemoryRatio::Bytes, size_t> aBufferMaxSize,
		std::function<std::vector<u8>(const TSerializable&)> aSerialize,
		std::function<Result<decltype(Failure), std::pair<TSerializable, size_t>>(std::span<const u8>)> aDeserialize,
		SharedReference<Socket> aSocket)
		: myBufferMaxSize(aBufferMaxSize)
		, mySerialize(std::move(aSerialize))
		, myDeserialize(std::move(aDeserialize))
		, mySocket(std::move(aSocket))
	{}

	Result<Socket::EWriteError> WriteMessage(const TSerializable& aMessage, SocketSerializing<TSerializable>::OnWriteCallback aOnWrite) override
	{
		const auto result = mySocket->Write(mySerialize(aMessage), std::move(aOnWrite));

		if (result)
			return Success;

		return static_cast<Socket::EWriteError>(result);
	}
	Result<Socket::EReadError> ReadMessage(SocketSerializing<TSerializable>::OnReadCallback aOnRead) override
	{
		return ParseBuffer(std::move(aOnRead));
	}

	void OnClose(Socket::OnCloseCallback aOnClose) override
	{
		mySocket->OnClose(std::move(aOnClose));
	}
	void Close() override
	{
		mySocket->Close();
	}

	Socket::EState GetState() const noexcept override
	{
		return mySocket->GetState();
	}

private:
	Result<Socket::EReadError> ParseBuffer(SocketSerializing<TSerializable>::OnReadCallback&& aOnRead)
	{
		if (myReadBuffer.empty())
		{
			auto readResult = mySocket->Read(Bind(&SocketSerializingImpl<TSerializable>::OnRead, this->weak_from_this(), std::move(aOnRead)));

			if (!readResult)
				return static_cast<Socket::EReadError>(readResult);

			return Success;
		}

		auto result = myDeserialize(myReadBuffer);

		if (!result)
		{
			auto readResult = mySocket->Read(Bind(&SocketSerializingImpl<TSerializable>::OnRead, this->weak_from_this(), std::move(aOnRead)));

			if (!readResult)
				return static_cast<Socket::EReadError>(readResult);

			return Success;
		}

		std::pair<TSerializable, size_t> serializable = std::move(result);

		myReadBuffer.erase(myReadBuffer.begin(), myReadBuffer.begin() + serializable.second);

		std::move(aOnRead)(std::move(serializable.first));

		return Success;
	}
	void OnRead(SocketSerializing<TSerializable>::OnReadCallback&& aOnRead, Result<Socket::EReadError, std::vector<u8>>&& aResult)
	{
		if (!aResult)
		{
			std::move(aOnRead)(static_cast<Socket::EReadError>(aResult));

			return;
		}

		std::vector<u8> buffer = std::move(aResult);

		if (buffer.size() + myReadBuffer.size() > myBufferMaxSize.To<EMemoryRatio::Bytes>())
		{
			std::move(aOnRead)(Socket::EReadError::OutOfMemory);

			return;
		}

		myReadBuffer.insert(myReadBuffer.end(), buffer.begin(), buffer.end());

		ParseBuffer(std::move(aOnRead));
	}

	const MemoryUnit<EMemoryRatio::Bytes, size_t> myBufferMaxSize;
	std::function<std::vector<u8>(const TSerializable&)> mySerialize;
	std::function<Result<decltype(Failure), std::pair<TSerializable, size_t>>(std::span<const u8>)> myDeserialize;

	SharedReference<Socket> mySocket;
	std::vector<u8> myReadBuffer;

};
