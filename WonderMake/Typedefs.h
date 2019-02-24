#pragma once
#include <chrono>
#include <functional>

using f32 = float;
using f64 = double;
using i32 = int;
using i64 = long long;
using u32 = unsigned int;
using u64 = unsigned long long;
using i8 = char;
using u8 = unsigned char;

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

