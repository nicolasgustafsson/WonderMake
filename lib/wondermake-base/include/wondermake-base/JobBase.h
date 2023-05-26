#pragma once

#include "wondermake-base/SystemPolicy.h"

#include "wondermake-utility/Future.h"
#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/SharedReference.h"
#include "wondermake-utility/Typedefs.h"

#include <optional>
#include <tuple>
#include <type_traits>

class JobBase
	: NonCopyable
	, NonMovable
{
public:
	virtual ~JobBase() = default;

	inline InlineExecutor GetExecutor()
	{
		return InlineExecutor();
	}
};

namespace _Impl
{
	template<
		typename TPromise,
		typename... TInputArgs>
	class JobInput
	{
	public:
		virtual void Run(TPromise, TInputArgs...) = 0;
		
	};

	template<
		typename TPromise>
	class JobInput<TPromise, void>
	{
	public:
		virtual void Run(TPromise) = 0;
		
	};

}

template<
	typename... TInputArgs>
struct JobInput final
{
	template<
		typename TPromise>
	using InputType = _Impl::JobInput<TPromise, TInputArgs...>;
};

namespace _Impl
{
	template<
		typename TPromise,
		typename... TJobInputs>
	class JobInputSet
		: TJobInputs::template InputType<TPromise>...
	{
	public:
		using Inputs = std::tuple<TJobInputs...>;

		using TJobInputs::template InputType<TPromise>::Run...;
	};

}

template<
	typename... TJobInputs>
struct JobInputSet final
{
	template<
		typename TPromise>
	using Inputs = _Impl::JobInputSet<TPromise, TJobInputs...>;

};

template<
	typename TPolicySet = Policy::Set<>>
class JobSub
{
private:
	template<typename TPolicySet>
	struct SDependencyTuple
	{
		using type = std::tuple<>;
	};

	template<typename... TPolicies>
	struct SDependencyTuple<Policy::Set<TPolicies...>>
	{
		using type = std::tuple<SharedReference<typename TPolicies::Dependency>...>;
	};

public:
	using PolicySet = TPolicySet;

	using Dependencies = SDependencyTuple<TPolicySet>::type;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

protected:
	inline JobSub() noexcept
		: myDependencies(std::move(*myInjectedDependencies))
	{
		myInjectedDependencies.reset();
	}

	template<typename TDependency>
		requires(PolicySet::template HasPolicy_v<TDependency, PWrite>)
	__forceinline [[nodiscard]] TDependency& Get() noexcept
	{
		return *std::get<SharedReference<std::decay_t<TDependency>>>(myDependencies);
	}

	template<typename TDependency>
		requires(PolicySet::template HasDependency_v<TDependency>)
	__forceinline [[nodiscard]] const TDependency& Get() const noexcept
	{
		return *std::get<SharedReference<std::decay_t<TDependency>>>(myDependencies);
	}

private:
	static thread_local std::optional<Dependencies>	myInjectedDependencies;

	Dependencies myDependencies;

};

template<typename TPolicySet>
thread_local std::optional<typename JobSub<TPolicySet>::Dependencies> JobSub<TPolicySet>::myInjectedDependencies;

using NoJobInput = JobInputSet<
	JobInput<void>>;
using NoJobOutput = void;

template<
	typename TInputSet = NoJobInput,
	typename TOutput = NoJobOutput>
class JobAbstracted
	: public JobBase
	, public TInputSet::template Inputs<Promise<TOutput>>
{
public:
	using InputSet = TInputSet;
	using Output = TOutput;
};

template<
	typename TInputSet = NoJobInput,
	typename TOutput = NoJobOutput,
	typename TPolicySet = Policy::Set<>>
class Job
	: public JobAbstracted<TInputSet, TOutput>
	, public JobSub<TPolicySet>
{
public:
	using Super = Job<TInputSet, TOutput, TPolicySet>;

};

template<typename TJob>
concept CJob = std::is_base_of_v<JobBase, TJob>;
