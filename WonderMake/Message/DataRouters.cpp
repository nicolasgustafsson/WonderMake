#include "stdafx.h"
#include "DataRouters.h"

#include "DispatchRouter.h"

DataRouters::DataRouters()
{
	myRouters.resize(3);
	myRouters[static_cast<decltype(myRouters)::size_type>(EThreadId::Logic)] = std::make_shared<DispatchRouter>();
	myRouters[static_cast<decltype(myRouters)::size_type>(EThreadId::Render)] = std::make_shared<DispatchRouter>();
	myRouters[static_cast<decltype(myRouters)::size_type>(EThreadId::File)] = std::make_shared<DispatchRouter>();
}

DispatchRouter& DataRouters::GetRouter(const EThreadId Process) const
{
	return *myRouters[static_cast<decltype(myRouters)::size_type>(Process)];
}

void DataRouters::Dispatch(std::unique_ptr<Dispatchable>&& Message, DispatchRouter& Router)
{
	Router.Dispatch(std::move(Message));
}