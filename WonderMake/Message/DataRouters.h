#pragma once

#include "Threads/ThreadIds.h"

#include "Utilities/Singleton.h"

#include <memory>
#include <vector>

class Dispatchable;
class DispatchRouter;

class DataRouters final
	: public Singleton<DataRouters>
{
public:
	DataRouters();

	DispatchRouter& GetRouter(const EThreadId Process) const;

	template<typename T>
	void Dispatch(const T& Message);

private:
	static void Dispatch(std::unique_ptr<Dispatchable>&& Message, DispatchRouter& Router);

	std::vector<std::shared_ptr<DispatchRouter>> myRouters;
};

template<typename T>
void DataRouters::Dispatch(const T& Message)
{
	for (const auto& router : myRouters)
	{
		Dispatch(std::make_unique<T>(Message), *router);
	}
}
