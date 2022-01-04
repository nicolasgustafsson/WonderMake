#include "pch.h"

#include "System/SystemRegistry.h"

thread_local SystemContainer_v2::InternalRep SystemRegistry::myConstructingContainer;
