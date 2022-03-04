#pragma once

#include "wondermake-io/Socket.h"

template<typename TProtobufWriteMessage, typename TProtobufReadMessage>
class SocketProtobuf
{
public:
	using ResultTypeWrite	= Result<void, Socket::SWriteError>;
	using ResultTypeRead	= Result<TProtobufReadMessage, Socket::SReadError>;
	using ResultTypeClose	= Result<Socket::SCloseLocation, Socket::SCloseError>;

	using FutureTypeWrite	= Future<ResultTypeWrite>;
	using FutureTypeRead	= Future<ResultTypeRead>;
	using FutureTypeClose	= Future<ResultTypeClose>;

	virtual ~SocketProtobuf() noexcept = default;

	virtual FutureTypeWrite	WriteMessage(const TProtobufWriteMessage& aMessage) = 0;
	virtual FutureTypeRead	ReadMessage() = 0;

	virtual FutureTypeClose	OnClose() = 0;
	virtual FutureTypeClose	Close() = 0;

	virtual Socket::EState	GetState() const noexcept = 0;

};
