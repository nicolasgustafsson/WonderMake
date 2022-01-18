#pragma once

#include "WonderMakeBase/Result.h"
#include "WonderMakeBase/Typedefs.h"
#include "WonderMakeBase/UniqueFunction.h"

#include "WonderMakeIo/Socket.h"

#include <functional>
#include <vector>

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
	enum class CloseReason
	{
		ClosedLocally,
		ClosedRemotely
	};
	enum class CloseError
	{
		OutOfMemory,
		InternalError
	};

	using OnCloseCallback = UniqueFunction<void(Result<CloseError, CloseReason>)>;

	struct CallbackInfo
	{
		OnCloseCallback	OnClosed = [](auto) {};
	};

	virtual Result<ConnectionError> Connect(std::string aConnectionName, CallbackInfo&& aCallbackInfo) = 0;
};
