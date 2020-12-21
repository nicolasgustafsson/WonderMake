#pragma once
#include "Utilities/TypeTraits/Concepts.h"

class BaseTrait {};

//[Nicos]: Traits that need to be implemented in the backend
class BackendTrait : public BaseTrait {};

//[Nicos]: Traits that can be implemented elsewhere
class ContainerTrait : public BaseTrait {};

class Iterable : public BackendTrait {};
class Indexable : public BackendTrait {};
class RandomAccess : public BackendTrait {};
class StableElements : public BackendTrait {};
class ConstantInsertion : public BackendTrait {};
class ConstantDeletion : public BackendTrait {};
class Associative : public BackendTrait {};
class Sortable : public BackendTrait {};

template<Hashable TKeyType>
class Key : public ContainerTrait 
{
public:
	using KeyType = TKeyType;
};

