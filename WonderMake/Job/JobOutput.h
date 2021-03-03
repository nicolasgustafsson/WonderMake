#pragma once

template<typename... TArgs>
struct JobOutput {};

template<typename... TArgs>
using JobOutputError = JobOutput<TArgs...>;
