#pragma once

#include "wondermake-utility/Result.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include "wondermake-io/Socket.h"

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
