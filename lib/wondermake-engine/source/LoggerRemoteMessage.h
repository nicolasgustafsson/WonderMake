#pragma once

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/LoggerRemote.pb.h"
#pragma warning(pop)

#include "wondermake-utility/Result.h"
#include "wondermake-utility/Typedefs.h"

#include <span>
#include <vector>

std::vector<u8> SerializeLogline(const ProtoLoggerRemote::LogLine& aMessage) noexcept;
Result<std::pair<ProtoLoggerRemote::LogLine, size_t>> DeserializeLogline(std::span<const u8> aData) noexcept;
