#pragma once

#include "WonderMakeBase/Result.h"
#include "WonderMakeBase/Typedefs.h"
#include "WonderMakeBase/UniqueFunction.h"

#include "WonderMakeIo/Socket.h"

class IpcConnection
	: public Socket
{
public:
	enum class ConnectionError
	{
		InvalidArgs,
		InvalidState,
		NoConnection,
		OutOfMemory,
		InternalError
	};

	virtual Result<ConnectionError> Connect(std::string aConnectionName) = 0;
};
