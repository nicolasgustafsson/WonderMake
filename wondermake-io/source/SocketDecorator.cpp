#pragma once

#include "wondermake-io/SocketDecorator.h"

Result<Socket::EWriteError, Socket::EAsynchronicity> SocketDecorator::Write(std::vector<u8> aBuffer, OnWriteCallback aOnWrite)
{
	return mySocket->Write(std::move(aBuffer), std::move(aOnWrite));
}

Result<Socket::EReadError, Socket::EAsynchronicity> SocketDecorator::Read(OnReadCallback aOnRead)
{
	return mySocket->Read(std::move(aOnRead));
}

void SocketDecorator::OnClose(OnCloseCallback aOnClose)
{
	mySocket->OnClose(std::move(aOnClose));
}

void SocketDecorator::Close()
{
	mySocket->Close();
}

Socket::EState SocketDecorator::GetState() const noexcept
{
	return mySocket->GetState();
}

SocketDecorator::SocketDecorator(SharedReference<Socket> aSocket) noexcept
	: mySocket(std::move(aSocket))
{}
