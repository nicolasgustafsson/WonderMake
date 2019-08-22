#pragma once

class NonCopyable
{
protected:
	NonCopyable() = default;

private:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;

	NonCopyable(const NonCopyable&&) noexcept {};
	NonCopyable& operator=(const NonCopyable&&) noexcept { return *this; };
};
