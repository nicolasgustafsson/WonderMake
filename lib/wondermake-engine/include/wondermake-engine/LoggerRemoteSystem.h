#pragma once

#include "wondermake-io/IpcAcceptor.h"
#include "wondermake-io/IpcConnection.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Result.h"

#include <memory>

class IpcSystem;
class LoggerRemoteConnection;
class LoggerRemoteSocket;

class LoggerRemoteSystem
	: public System<
		Policy::Set<
			PAdd<IpcSystem, PWrite>>,
		STrait::Set<
			STLogger,
			STSingleton>>
{
public:
	Result<IpcAcceptor::EOpenError, std::shared_ptr<LoggerRemoteSocket>> OpenSocketIpc(std::string aName);

	Result<IpcConnection::ConnectionError, std::shared_ptr<LoggerRemoteConnection>> ConnectIpc(std::string aName);

};
