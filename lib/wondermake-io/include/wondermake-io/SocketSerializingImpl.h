#pragma once

#include "wondermake-io/SocketSerializing.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/SharedReference.h"

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
		std::function<Result<std::pair<TSerializable, size_t>>(std::span<const u8>)> aDeserialize,
		SharedReference<Socket> aSocket)
		: myBufferMaxSize(aBufferMaxSize)
		, mySerialize(std::move(aSerialize))
		, myDeserialize(std::move(aDeserialize))
		, mySocket(std::move(aSocket))
	{}

	Result<void, Socket::SWriteError> WriteMessage(const TSerializable& aMessage, SocketSerializing<TSerializable>::OnWriteCallback aOnWrite) override
	{
		const auto result = mySocket->Write(mySerialize(aMessage), std::move(aOnWrite));

		if (result)
			return Ok();

		return result;
	}
	Result<void, Socket::SReadError> ReadMessage(SocketSerializing<TSerializable>::OnReadCallback aOnRead) override
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
	Result<void, Socket::SReadError> ParseBuffer(SocketSerializing<TSerializable>::OnReadCallback&& aOnRead)
	{
		if (myReadBuffer.empty())
		{
			auto readResult = mySocket->Read(Bind(&SocketSerializingImpl<TSerializable>::OnRead, this->weak_from_this(), std::move(aOnRead)));

			if (!readResult)
				return readResult;

			return Ok();
		}

		const auto maxBytes = myBufferMaxSize.To<EMemoryRatio::Bytes>();

		const auto endIt = myReadBuffer.size() <= maxBytes ? myReadBuffer.end() : myReadBuffer.begin() + maxBytes;

		auto result = myDeserialize(std::span(myReadBuffer.begin(), endIt));

		if (!result)
		{
			if (myReadBuffer.size() >= maxBytes)
			{
				std::move(aOnRead)(Err(Socket::SReadError{ Socket::EReadError::MessageToBig }));

				return Err(Socket::SReadError{ Socket::EReadError::MessageToBig });
			}

			auto readResult = mySocket->Read(Bind(&SocketSerializingImpl<TSerializable>::OnRead, this->weak_from_this(), std::move(aOnRead)));

			if (!readResult)
				return readResult;

			return Ok();
		}

		auto serializable = std::move(result).Unwrap();

		myReadBuffer.erase(myReadBuffer.begin(), myReadBuffer.begin() + serializable.second);

		std::move(aOnRead)(Ok(std::move(serializable.first)));

		return Ok();
	}
	void OnRead(SocketSerializing<TSerializable>::OnReadCallback&& aOnRead, Result<std::vector<u8>, Socket::SReadError>&& aResult)
	{
		if (!aResult)
		{
			std::move(aOnRead)(aResult);

			return;
		}

		auto buffer = std::move(aResult).Unwrap();

		myReadBuffer.insert(myReadBuffer.end(), buffer.begin(), buffer.end());

		ParseBuffer(std::move(aOnRead));
	}

	const MemoryUnit<EMemoryRatio::Bytes, size_t> myBufferMaxSize;
	std::function<std::vector<u8>(const TSerializable&)> mySerialize;
	std::function<Result<std::pair<TSerializable, size_t>>(std::span<const u8>)> myDeserialize;

	SharedReference<Socket> mySocket;
	std::vector<u8> myReadBuffer;

};
