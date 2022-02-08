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

class NonMovable
{
protected:
	NonMovable() = default;
	~NonMovable() = default;

	NonMovable(const NonMovable&) = default;
	NonMovable& operator=(const NonMovable&) = default;

	NonMovable(NonMovable&&) noexcept = delete;
	NonMovable& operator=(NonMovable&&) noexcept = delete;
};

