#pragma once

class NonCopyable
{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;

	NonCopyable(NonCopyable&&) noexcept = default;
	NonCopyable& operator=(NonCopyable&&) noexcept = default;
};
