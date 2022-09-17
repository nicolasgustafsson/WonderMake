#pragma once

#include "wondermake-utility/Result.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include "wondermake-io/Socket.h"

#include <magic_enum.hpp>

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

	using ResultTypeConnect = Result<void, SConnectionError>;

	virtual ResultTypeConnect Connect(std::string aConnectionName) = 0;
};

inline static void WmLogStream(std::ostream& aStream, const IpcConnection::SConnectionError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
