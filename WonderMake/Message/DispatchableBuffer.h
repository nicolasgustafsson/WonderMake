#pragma once
#include "Typedefs.h"

#include "Utilities/Singleton.h"

#include <vector>

class DispatchableBuffer final
	: public Singleton<DispatchableBuffer>
{
public:
	template<typename TMessage>
	void Dispatch(TMessage aDispatchable) noexcept
	{
		auto ptr = std::make_shared<TMessage>(std::move(aDispatchable));

		myList.emplace_back(std::static_pointer_cast<Dispatchable>(ptr));
	}

	std::vector<std::shared_ptr<Dispatchable>> myList;
};
