#pragma once

#include "wondermake-io/Socket.h"

template<typename TSerializable>
class SocketSerializing
{
public:
	using ResultTypeWrite	= Result<void, Socket::SWriteError>;
	using ResultTypeRead	= Result<TSerializable, Socket::SReadError>;
	using ResultTypeClose	= Result<Socket::SCloseLocation, Socket::SCloseError>;

	using FutureTypeWrite	= Future<ResultTypeWrite>;
	using FutureTypeRead	= Future<ResultTypeRead>;
	using FutureTypeClose	= Future<ResultTypeClose>;

	using OnWriteCallback	= UniqueFunction<void(Result<void, Socket::SWriteError>)>;
	using OnReadCallback	= UniqueFunction<void(Result<TSerializable, Socket::SReadError>&&)>;

	virtual ~SocketSerializing() noexcept = default;

	virtual FutureTypeWrite	WriteMessage(const TSerializable& aMessage) = 0;
	virtual FutureTypeRead	ReadMessage() = 0;
	virtual FutureTypeClose	OnClose() = 0;
	virtual FutureTypeClose	Close() = 0;

	virtual Socket::EState GetState() const noexcept = 0;

};
