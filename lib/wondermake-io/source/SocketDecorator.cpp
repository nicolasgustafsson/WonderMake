#pragma once

#include "wondermake-io/SocketDecorator.h"

Socket::FutureTypeWrite SocketDecorator::Write(std::vector<u8> aBuffer)
{
	return mySocket->Write(std::move(aBuffer));
}

Socket::FutureTypeRead SocketDecorator::Read()
{
	return mySocket->Read();
}

Socket::FutureTypeClose SocketDecorator::OnClose()
{
	return mySocket->OnClose();
}

Socket::FutureTypeClose SocketDecorator::Close()
{
	return mySocket->Close();
}

Socket::EState SocketDecorator::GetState() const noexcept
{
	return mySocket->GetState();
}

SocketDecorator::SocketDecorator(SharedReference<Socket> aSocket) noexcept
	: mySocket(std::move(aSocket))
{}
