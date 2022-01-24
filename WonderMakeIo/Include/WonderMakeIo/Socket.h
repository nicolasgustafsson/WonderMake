#pragma once

#include "WonderMakeBase/Result.h"
#include "WonderMakeBase/Typedefs.h"
#include "WonderMakeBase/UniqueFunction.h"

#include <vector>

class Socket
{
public:
	enum class EState
	{
		Opening,
		Open,
		Closing,
		Closed
	};

	enum class EAsynchronicity
	{
		Asynchronous,
		Synchronous
	};
	enum class EWriteError
	{
		InvalidArgs,
		InvalidState,
		StateChanged,
		OutOfMemory,
		InternalError
	};
	enum class EReadError
	{
		InvalidState,
		StateChanged,
		OutOfMemory,
		InternalError
	};

	enum class ECloseReason
	{
		ClosedLocally,
		ClosedRemotely
	};
	enum class ECloseError
	{
		AlreadyClosed,
		OutOfMemory,
		InternalError
	};

	using OnWriteCallback = UniqueFunction<void(Result<EWriteError>)>;
	using OnReadCallback = UniqueFunction<void(Result<EReadError, std::vector<u8>>&&)>;
	using OnCloseCallback = UniqueFunction<void(Result<ECloseError, ECloseReason>)>;

	virtual ~Socket() noexcept = default;

	virtual Result<EWriteError, EAsynchronicity> Write(std::vector<u8> aBuffer, OnWriteCallback aOnWrite) = 0;
	virtual Result<EReadError, EAsynchronicity> Read(OnReadCallback aOnRead) = 0;
	virtual void OnClose(OnCloseCallback aOnClose) = 0;
	virtual void Close() = 0;

	virtual EState GetState() const noexcept = 0;

protected:
	Socket() noexcept = default;

};
