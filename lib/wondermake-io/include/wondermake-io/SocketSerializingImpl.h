#pragma once

#include "wondermake-io/SocketSerializing.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/SharedReference.h"

#include <functional>
#include <mutex>

template<typename TSerializable>
class SocketSerializingImpl
	: public SocketSerializing<TSerializable>
	, public std::enable_shared_from_this<SocketSerializingImpl<TSerializable>>
{
public:
	SocketSerializingImpl(
		AnyExecutor aExecutor,
		MemoryUnit<EMemoryRatio::Bytes, size_t> aBufferMaxSize,
		std::function<std::vector<u8>(const TSerializable&)> aSerialize,
		std::function<Result<std::pair<TSerializable, size_t>>(std::span<const u8>)> aDeserialize,
		SharedReference<Socket> aSocket)
		: myExecutor(std::move(aExecutor))
		, myBufferMaxSize(aBufferMaxSize)
		, mySerialize(std::move(aSerialize))
		, myDeserialize(std::move(aDeserialize))
		, mySocket(std::move(aSocket))
	{}

	SocketSerializing<TSerializable>::FutureTypeWrite WriteMessage(const TSerializable& aMessage) override
	{
		return mySocket->Write(mySerialize(aMessage));
	}
	SocketSerializing<TSerializable>::FutureTypeRead ReadMessage() override
	{
		std::lock_guard lock(myMutex);

		return ParseBuffer();
	}

	SocketSerializing<TSerializable>::FutureTypeClose OnClose() override
	{
		return mySocket->OnClose();
	}
	SocketSerializing<TSerializable>::FutureTypeClose Close() override
	{
		return mySocket->Close();
	}

	Socket::EState GetState() const noexcept override
	{
		return mySocket->GetState();
	}

private:
	SocketSerializing<TSerializable>::FutureTypeRead ParseBuffer()
	{
		using ResultTypeRead = SocketSerializing<TSerializable>::ResultTypeRead;
		using FutureTypeRead = SocketSerializing<TSerializable>::FutureTypeRead;

		const auto makeThenApply = [this]()
		{
			return MoveFutureResult([selfWeak = this->weak_from_this()](auto&& aResult) -> FutureTypeRead
			{
				auto self = selfWeak.lock();

				if (!self)
					return MakeCanceledFuture<ResultTypeRead>();

				return self->OnRead(std::move(aResult));
			});
		};

		if (myReadBuffer.empty())
		{
			return mySocket->Read()
				.ThenApply(myExecutor, makeThenApply());
		}

		const auto maxBytes = myBufferMaxSize.To<EMemoryRatio::Bytes>();

		const auto endIt = myReadBuffer.size() <= maxBytes ? myReadBuffer.end() : myReadBuffer.begin() + maxBytes;

		auto result = myDeserialize(std::span(myReadBuffer.begin(), endIt));

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
	SocketSerializing<TSerializable>::FutureTypeRead OnRead(Result<std::vector<u8>, Socket::SReadError>&& aResult)
	{
		std::lock_guard lock(myMutex);

		if (!aResult)
			return MakeCompletedFuture<SocketSerializing<TSerializable>::ResultTypeRead>(std::move(aResult));

		auto buffer = std::move(aResult).Unwrap();

		myReadBuffer.insert(myReadBuffer.end(), buffer.begin(), buffer.end());

		return ParseBuffer();
	}

	std::recursive_mutex myMutex;
	AnyExecutor myExecutor;
	const MemoryUnit<EMemoryRatio::Bytes, size_t> myBufferMaxSize;
	std::function<std::vector<u8>(const TSerializable&)> mySerialize;
	std::function<Result<std::pair<TSerializable, size_t>>(std::span<const u8>)> myDeserialize;

	SharedReference<Socket> mySocket;
	std::vector<u8> myReadBuffer;

};
