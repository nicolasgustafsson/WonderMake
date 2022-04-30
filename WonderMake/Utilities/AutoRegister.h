#pragma once
#include "Utilities/UniqueFunction.h"

class _AutoRegister
{
public:

//template<typename TRegisterFunc>
//constexpr static auto RegisterFunction = []()
//{

//    StaticInitializationTimeFunction<VariableGetterFunc<T, Name, TDefaultValue>> itNeedsAName;
//};

    template<typename TRegisterFunc>
	inline _AutoRegister(TRegisterFunc aRegFunc)
	{
        //StaticInitializationTimeFunction<aRegFunc> itNeedsAName;
        aRegFunc();
	}
};
#define WM_AUTO_REGISTER(aRegisterFunction, aUniqueName) _AutoRegister _AutoReg##aUniqueName(aRegisterFunction);