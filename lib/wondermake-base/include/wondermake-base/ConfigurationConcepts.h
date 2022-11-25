#pragma once

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/Result.h"

#include <string>

template<typename TConfig>
concept CConfigRaw =
	std::is_arithmetic_v<std::remove_const_t<TConfig>> ||
	std::is_same_v<std::decay_t<TConfig>, std::string> ||
	std::is_same_v<std::decay_t<TConfig>, FilePath>;

template<typename TType>
struct WmConfig {};

template<typename TType>
concept CConfigurableValue =
	requires(const TType& aType, const std::decay_t<decltype(WmConfig<TType>::To(aType))>& aConfigType)
	{
		{ WmConfig<TType>::To(aType) } -> CConfigRaw;
		{ WmConfig<TType>::From(aConfigType) } -> std::convertible_to<Result<TType>>;
	};

template<typename TConfig>
concept CConfigType =
	!std::is_const_v<TConfig> && 
	!std::is_reference_v<TConfig> && (
		std::is_arithmetic_v<TConfig> ||
		std::is_enum_v<TConfig> ||
		CConfigurableValue<TConfig> ||
		CMemoryUnit<TConfig> ||
		std::is_same_v<TConfig, FilePath> ||
		std::is_same_v<std::string, TConfig>);

template<typename TConfig>
concept CConfig =
	std::is_arithmetic_v<std::remove_const_t<TConfig>> ||
	std::is_enum_v<std::remove_const_t<TConfig>> ||
	CConfigurableValue<TConfig> ||
	CMemoryUnit<TConfig> ||
	std::is_same_v<TConfig, FilePath> ||
	std::is_constructible_v<std::string, TConfig> ||
	std::is_convertible_v<TConfig, std::string> ||
	CConfigRaw<TConfig>;
