#pragma once

#include "WonderMakeBase/Result.h"
#include "WonderMakeBase/UniqueFunction.h"

#include <memory>
#include <functional>

class IpcConnection;

class IpcAcceptor
{
public:
	enum class State
	{
		Open,
		Closed
	};

	enum class OpenError
	{
		InvalidArgs,
		InvalidState,
		InternalError
	};

	enum class CloseReason
	{
		OutOfMemory,
		InternalError
	};

	using OnConnectionCallback = std::function<void(std::shared_ptr<IpcConnection>&&)>;
	using OnCloseCallback = UniqueFunction<void(Result<CloseReason>)>;

	struct CallbackInfo
	{
		OnConnectionCallback	OnConnection;
		OnCloseCallback			OnClose = [](auto) {};
	};

	virtual ~IpcAcceptor() noexcept = default;

	virtual Result<OpenError> Open(std::string aName, CallbackInfo&& aCallbackInfo) = 0;
	virtual void Close() = 0;

	virtual State GetState() const noexcept = 0;

protected:
	IpcAcceptor() noexcept = default;

};
