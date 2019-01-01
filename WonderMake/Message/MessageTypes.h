#pragma once

#include "Logging/LogMessageTags.h"

#include <typeinfo>
#include <functional>

typedef std::function<void()> Closure;

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
	Task(const Closure aClosure)
		: myClosure(std::move(aClosure))
	{}

	void Run() const
	{
		myClosure();
	}

private:
	const Closure myClosure;
};

struct SLogMessage : public Message<Dispatchable>
{
public:
	SLogMessage(std::string&& aLogMessage)
		:LogText(std::forward<std::string>(aLogMessage)) {}

	std::string LogText;

	bool HasTag(const std::string& aTag) const
	{
		return LogText.find(aTag) != std::string::npos;
	}
};
