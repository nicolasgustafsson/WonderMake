#pragma once

#include "wondermake-base/System.h"

#include <memory>

class IpcAcceptor;
class IpcConnection;

class IpcSystem
	: public SystemAbstracted
{
public:
	virtual std::shared_ptr<IpcAcceptor> CreateAcceptor() = 0;
	virtual std::shared_ptr<IpcConnection> CreateConnection() = 0;
};
