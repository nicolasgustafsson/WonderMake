#pragma once

#include "Typedefs.h"

#include "Logging/LogMessageTags.h"

#include <atomic>
#include <filesystem>
#include <functional>
#include <limits>
#include <typeinfo>

class Dispatchable
{
public:
	virtual ~Dispatchable() noexcept = default;
	inline size_t GetTypeHash() const noexcept
	{
		return myTypeHash;
	}

protected:
	constexpr Dispatchable(const size_t aTypeHash)
		: myTypeHash(aTypeHash)
	{}

private:
	size_t myTypeHash;
};

template<typename T>
class Message
	: public Dispatchable
{
public:
	static constexpr size_t GetTypeHash()
	{
		return typeid(T).hash_code();
	}

protected:
	constexpr Message() noexcept
		: Dispatchable(GetTypeHash())
	{}
};

struct SLogMessage 
	: public Message<SLogMessage>
{
public:
	inline SLogMessage(std::string&& aLogMessage) noexcept
		:LogText(std::forward<std::string>(aLogMessage)) {}

	std::string LogText;

	inline bool HasTag(const std::string& aTag) const noexcept
	{
		return LogText.find(aTag) != std::string::npos;
	}
};

struct SDebugMessage
	: public Message<SDebugMessage>
{

};

struct SFileChangedMessage
	: public Message<SFileChangedMessage>
{
	std::filesystem::path FilePath;
};

struct RenderPassMessage
	: public Message<RenderPassMessage>
{
};
