#pragma once
#include "System/SystemContainer.h"

template <typename CRTP, typename ... TPolicies>
class Systemus : public System<TPolicies...>
{
public:

private:
	Systemus() = default;
	friend CRTP;
	inline static _AutoRegister AutoRegisterFunction{ _RegisterSystem<CRTP> };
};

