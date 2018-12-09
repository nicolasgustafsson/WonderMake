#pragma once

#include "Utilities/RestrictTypes.h"

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
	constexpr Dispatchable(const size_t TypeHash)
		: myTypeHash(TypeHash)
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
	Task(const Closure Function)
		: myFunction(std::move(Function))
	{}
	void Run() const
	{
		myFunction();
	}

private:
	const Closure myFunction;
};
