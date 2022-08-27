#pragma once

#include "wondermake-utility/Result.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include "wondermake-io/Socket.h"

class IpcConnection
	: public Socket
{
public:
	enum class EConnectionError
	{
		InvalidArgs,
		InvalidState,
		NoConnection,
		OutOfMemory,
		InternalError
	};

	struct SConnectionError
	{
		EConnectionError Error = EConnectionError::InternalError;
		u64 Reason = 0;
	};

	virtual Result<void, SConnectionError> Connect(std::string aConnectionName) = 0;
};
