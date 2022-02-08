#pragma once

#include "wondermake-utility/Result.h"
#include "wondermake-utility/UniqueFunction.h"

#include <memory>
#include <functional>

class IpcConnection;

class IpcAcceptor
{
public:
	enum class EState
	{
		Open,
		Closed
	};

	enum class EOpenError
	{
		InvalidArgs,
		InvalidState,
		OutOfMemory,
		InternalError
	};

	enum class ECloseReason
	{
		OutOfMemory,
		InternalError
	};

	using OnConnectionCallback = std::function<void(std::shared_ptr<IpcConnection>&&)>;
	using OnCloseCallback = UniqueFunction<void(Result<ECloseReason>)>;

	struct CallbackInfo
	{
		OnConnectionCallback	OnConnection;
		OnCloseCallback			OnClose = [](auto) {};
	};

	virtual ~IpcAcceptor() noexcept = default;

	virtual Result<EOpenError> Open(std::string aName, CallbackInfo&& aCallbackInfo) = 0;
	virtual void Close() = 0;

	virtual EState GetState() const noexcept = 0;

protected:
	IpcAcceptor() noexcept = default;

};
