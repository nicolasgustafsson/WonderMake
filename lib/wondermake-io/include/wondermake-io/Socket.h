#pragma once

#include "wondermake-utility/Future.h"
#include "wondermake-utility/Result.h"
#include "wondermake-utility/Typedefs.h"

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

	using ResultTypeWrite	= Result<void, SWriteError>;
	using ResultTypeRead	= Result<std::vector<u8>, SReadError>;
	using ResultTypeClose	= Result<SCloseLocation, SCloseError>;

	using FutureTypeWrite	= Future<ResultTypeWrite>;
	using FutureTypeRead	= Future<ResultTypeRead>;
	using FutureTypeClose	= Future<ResultTypeClose>;

	virtual ~Socket() noexcept = default;

	virtual FutureTypeWrite		Write(std::vector<u8> aBuffer) = 0;
	virtual FutureTypeRead		Read() = 0;
	virtual FutureTypeClose		OnClose() = 0;
	virtual FutureTypeClose		Close() = 0;

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
