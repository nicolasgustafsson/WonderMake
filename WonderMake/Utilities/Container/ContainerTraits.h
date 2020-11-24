#pragma once
#include <type_traits>

class BaseRequirement {};

template<typename T>
concept ContainerTrait = std::is_base_of_v<BaseRequirement, T>;

template<typename T, typename... Ts>
concept ContainsTrait = std::disjunction_v<std::is_same<T, Ts>...>;

class Iterable : public BaseRequirement {};
class Indexable : public BaseRequirement {};
class RandomAccess : public BaseRequirement {};
class StableElements : public BaseRequirement {};
class ConstantInsertion : public BaseRequirement {};
class ConstantDeletion : public BaseRequirement {};
//TODO: Keys