#pragma once
#include "BaseObject.h"
#include "Functionalities/FunctionalitySystem.h"

class Object : public BaseObject, public NonCopyable
{
public:
    template<typename TType>
    inline TType& Add() {
        return SystemPtr<FunctionalitySystem<TType>>()->AddFunctionality(*this, true);
    }
};
