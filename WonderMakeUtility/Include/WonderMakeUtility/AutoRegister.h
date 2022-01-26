#pragma once

class _AutoRegister
{
public:
	template<typename TRegisterFunc>
	inline _AutoRegister(TRegisterFunc aRegFunc)
	{
		aRegFunc();
	}
};

#define WM_AUTO_REGISTER(aRegisterFunction, aUniqueName) _AutoRegister _AutoReg##aUniqueName (aRegisterFunction);