#pragma once

#include "wondermake-io/Socket.h"

#include "wondermake-utility/UniqueFunction.h"

template<typename TSerializable>
class SocketSerializing
{
public:
	using OnWriteCallback	= UniqueFunction<void(Result<void, Socket::SWriteError>)>;
	using OnReadCallback	= UniqueFunction<void(Result<TSerializable, Socket::SReadError>&&)>;

	virtual ~SocketSerializing() noexcept = default;

	virtual Result<void, Socket::SWriteError>	WriteMessage(const TSerializable& aMessage, OnWriteCallback aOnWrite) = 0;
	virtual Result<void, Socket::SReadError>	ReadMessage(OnReadCallback aOnRead) = 0;

	virtual void OnClose(Socket::OnCloseCallback aOnClose) = 0;
	virtual void Close() = 0;

	virtual Socket::EState GetState() const noexcept = 0;

};
