#pragma once

#include "wondermake-io/SocketAcceptor.h"

class IpcConnection;

class IpcAcceptor
	: public SocketAcceptor
{
public:
	enum class EOpenError
	{
		InvalidArgs,
		InvalidState,
		OutOfMemory,
		InternalError
	};

	struct SOpenError
	{
		EOpenError Error = EOpenError::InternalError;
		u64 Reason = 0;
	};

	using ResultTypeOpen	= Result<void, SOpenError>;

	virtual ResultTypeOpen	Open(std::string aName) = 0;

protected:
	IpcAcceptor() noexcept = default;

};

inline static void WmLogStream(std::ostream& aStream, const IpcAcceptor::SOpenError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
