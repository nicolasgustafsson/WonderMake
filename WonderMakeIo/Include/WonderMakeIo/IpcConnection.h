#pragma once

#include "WonderMakeUtility/Result.h"
#include "WonderMakeUtility/Typedefs.h"
#include "WonderMakeUtility/UniqueFunction.h"

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
