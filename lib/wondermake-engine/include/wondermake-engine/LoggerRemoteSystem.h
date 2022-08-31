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
	Result<std::shared_ptr<LoggerRemoteSocket>, IpcAcceptor::SOpenError> OpenSocketIpc(std::string aName);

	Result<std::shared_ptr<LoggerRemoteConnection>, IpcConnection::SConnectionError> ConnectIpc(std::string aName);

};
