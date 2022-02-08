#pragma once

#include "wondermake-io/Socket.h"

#include "wondermake-utility/SharedReference.h"

class SocketDecorator
	: public Socket
{
public:
	Result<EWriteError, EAsynchronicity> Write(std::vector<u8> aBuffer, OnWriteCallback aOnWrite) override;
	Result<EReadError, EAsynchronicity> Read(OnReadCallback aOnRead) override;
	void OnClose(OnCloseCallback aOnClose) override;
	void Close() override;

	EState GetState() const noexcept override;

protected:
	SocketDecorator(SharedReference<Socket> aSocket) noexcept;

private:
	SharedReference<Socket> mySocket;

};
