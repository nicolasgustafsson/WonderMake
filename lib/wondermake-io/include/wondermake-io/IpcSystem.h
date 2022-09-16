#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/SharedReference.h"

#include <memory>

class IpcAcceptor;
class IpcConnection;

class IpcSystem
	: public SystemAbstracted
{
public:
	virtual SharedReference<IpcAcceptor>	CreateAcceptor() = 0;
	virtual SharedReference<IpcConnection>	CreateConnection() = 0;
};
