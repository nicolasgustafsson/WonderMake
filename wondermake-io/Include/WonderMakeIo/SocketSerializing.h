#pragma once

#include "WonderMakeIo/Socket.h"

#include "WonderMakeUtility/UniqueFunction.h"

template<typename TSerializable>
class SocketSerializing
{
public:
	using OnWriteCallback	= UniqueFunction<void(Result<Socket::EWriteError>)>;
	using OnReadCallback	= UniqueFunction<void(Result<Socket::EReadError, TSerializable>&&)>;

	virtual ~SocketSerializing() noexcept = default;

	virtual Result<Socket::EWriteError>	WriteMessage(const TSerializable& aMessage, OnWriteCallback aOnWrite) = 0;
	virtual Result<Socket::EReadError>	ReadMessage(OnReadCallback aOnRead) = 0;

	virtual void OnClose(Socket::OnCloseCallback aOnClose) = 0;
	virtual void Close() = 0;

	virtual Socket::EState GetState() const noexcept = 0;

};
