#pragma once
#include <any>

struct EffectBlackboard
{
	Container<std::any, Key<std::string>, Sortable> Data;
};

