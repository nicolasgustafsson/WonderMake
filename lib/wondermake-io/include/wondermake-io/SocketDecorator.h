#pragma once

#include "wondermake-io/Socket.h"

#include "wondermake-utility/SharedReference.h"

class SocketDecorator
	: public Socket
{
public:
	FutureTypeWrite		Write(std::vector<u8> aBuffer) override;
	FutureTypeRead		Read() override;
	FutureTypeClose		OnClose() override;
	FutureTypeClose		Close() override;

	EState GetState() const noexcept override;

protected:
	SocketDecorator(SharedReference<Socket> aSocket) noexcept;

private:
	SharedReference<Socket> mySocket;

};
