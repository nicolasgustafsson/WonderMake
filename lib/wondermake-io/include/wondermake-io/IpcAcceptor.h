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
	enum class ECloseError
	{
		OutOfMemory,
		InternalError
	};

	struct SOpenError
	{
		EOpenError Error = EOpenError::InternalError;
		u64 Reason = 0;
	};
	struct SCloseError
	{
		ECloseError Error = ECloseError::InternalError;
		u64 Reason = 0;
	};

	using OnConnectionCallback = std::function<void(std::shared_ptr<IpcConnection>&&)>;
	using OnCloseCallback = UniqueFunction<void(Result<void, SCloseError>)>;

	struct CallbackInfo
	{
		OnConnectionCallback	OnConnection;
		OnCloseCallback			OnClose = [](auto) {};
	};

	virtual ~IpcAcceptor() noexcept = default;

	virtual Result<void, SOpenError> Open(std::string aName, CallbackInfo&& aCallbackInfo) = 0;
	virtual void Close() = 0;

	virtual EState GetState() const noexcept = 0;

protected:
	IpcAcceptor() noexcept = default;

};
