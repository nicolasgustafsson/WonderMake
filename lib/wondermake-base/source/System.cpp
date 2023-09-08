#include "wondermake-base/System.h"

thread_local std::optional<SystemId> SystemAbstracted::myInjectedId = std::nullopt;
