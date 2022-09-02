#pragma once

#include "wondermake-utility/Result.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include <magic_enum.hpp>

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
		MessageToBig,
		InternalError
	};
	
	struct SWriteError
	{
		EWriteError Error = EWriteError::InternalError;
		u64 Reason = 0;
	};
	struct SReadError
	{
		EReadError Error = EReadError::InternalError;
		u64 Reason = 0;
	};

	enum class ECloseLocation
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

	struct SCloseLocation
	{
		ECloseLocation Location = ECloseLocation::ClosedLocally;
		u64 Reason = 0;
	};
	struct SCloseError
	{
		ECloseError Error = ECloseError::InternalError;
		u64 Reason = 0;
	};

	using OnWriteCallback = UniqueFunction<void(Result<void, SWriteError>)>;
	using OnReadCallback = UniqueFunction<void(Result<std::vector<u8>, SReadError>)>;
	using OnCloseCallback = UniqueFunction<void(Result<SCloseLocation, SCloseError>)>;

	virtual ~Socket() noexcept = default;

	virtual Result<EAsynchronicity, SWriteError> Write(std::vector<u8> aBuffer, OnWriteCallback aOnWrite) = 0;
	virtual Result<EAsynchronicity, SReadError> Read(OnReadCallback aOnRead) = 0;
	virtual void OnClose(OnCloseCallback aOnClose) = 0;
	virtual void Close() = 0;

	virtual EState GetState() const noexcept = 0;

protected:
	Socket() noexcept = default;

};

inline static void WmLogStream(std::ostream& aStream, const Socket::SWriteError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
inline static void WmLogStream(std::ostream& aStream, const Socket::SReadError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
inline static void WmLogStream(std::ostream& aStream, const Socket::SCloseLocation& aLocation)
{
	aStream << magic_enum::enum_name(aLocation.Location) << '(' << static_cast<std::underlying_type_t<decltype(aLocation.Location)>>(aLocation.Location) << ':' << aLocation.Reason << ')';
}
inline static void WmLogStream(std::ostream& aStream, const Socket::SCloseError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
