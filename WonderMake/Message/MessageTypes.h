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

class Task final
	: public Message<Task>
{
public:
	inline Task(Closure aClosure)
		: myClosure(std::move(aClosure))
	{}

	inline void Run() const
	{
		myClosure();
	}

private:
	Closure myClosure;
};

template<typename T>
class Query
	: public Message<Query<T>>
{
public:
	inline Query() noexcept
		: myId(NextQueryId++)
	{}

	inline u32 Id() const noexcept
	{
		return myId;
	}

private:
	static std::atomic_uint32_t NextQueryId;

	u32 myId;
};

template<typename T>
std::atomic_uint32_t Query<T>::NextQueryId = 0;

template<typename T>
class QueryResult
	: public Message<QueryResult<T>>
{
public:
	QueryResult() = default;
	template<typename TQuery>
	inline QueryResult(const TQuery& aQuery)
		: myId(aQuery.Id())
	{}

	template<typename TQuery>
	inline bool IsResultOf(const TQuery& aQuery) const
	{
		return Id() == aQuery.Id();
	}

	inline u32 Id() const
	{
		return myId;
	}

private:
	u32 myId = std::numeric_limits<u32>::max();
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