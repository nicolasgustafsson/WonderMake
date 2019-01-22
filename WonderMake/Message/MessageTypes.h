#pragma once

#include "Typedefs.h"

#include "Logging/LogMessageTags.h"

#include <atomic>
#include <limits>
#include <typeinfo>
#include <functional>

class Dispatchable
{
public:
	virtual ~Dispatchable() = default;
	inline size_t GetTypeHash() const
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
	constexpr Message()
		: Dispatchable(GetTypeHash())
	{}
};

class Task final
	: public Message<Task>
{
public:
	inline Task(const Closure aClosure)
		: myClosure(std::move(aClosure))
	{}

	inline void Run() const
	{
		myClosure();
	}

private:
	const Closure myClosure;
};

template<typename T>
class Query
	: public Message<Query<T>>
{
public:
	inline Query()
		: myId(NextQueryId++)
	{}

	inline u32 Id() const
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

struct SLogMessage : public Message<SLogMessage>
{
public:
	inline SLogMessage(std::string&& aLogMessage)
		:LogText(std::forward<std::string>(aLogMessage)) {}

	std::string LogText;

	inline bool HasTag(const std::string& aTag) const
	{
		return LogText.find(aTag) != std::string::npos;
	}
};
