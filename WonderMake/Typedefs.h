#pragma once
#include <chrono>
#include <functional>

using f32 = float;
using f64 = double;
using i32 = int32_t;
using i64 = int64_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using u8 = uint8_t;

using Closure = std::function<void()>;

namespace WmChrono
{
	using fNanoseconds = std::chrono::duration<float, std::nano>;
	using fMicroseconds = std::chrono::duration<float, std::micro>;
	using fMilliseconds = std::chrono::duration<float, std::milli>;
	using fSeconds = std::chrono::duration<float, std::ratio<1, 1>>;
	using fMinutes = std::chrono::duration<float, std::ratio<60>>;
	using fHours = std::chrono::duration<float, std::ratio<3600>>;

	using dNanoseconds = std::chrono::duration<double, std::nano>;
	using dMicroseconds = std::chrono::duration<double, std::micro>;
	using dMilliseconds = std::chrono::duration<double, std::milli>;
	using dSeconds = std::chrono::duration<double, std::ratio<1, 1>>;
	using dMinutes = std::chrono::duration<double, std::ratio<60>>;
	using dHours = std::chrono::duration<double, std::ratio<3600>>;
}

