#pragma once

class NonCopyable
{
public:
	NonCopyable() = default;
	~NonCopyable() = default;
protected:

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;

	NonCopyable(NonCopyable&&) noexcept = default;
	NonCopyable& operator=(NonCopyable&&) noexcept = default;
};

class NonMovable
{
public:
	NonMovable() = default;
	~NonMovable() = default;
protected:

	NonMovable(const NonMovable&) = default;
	NonMovable& operator=(const NonMovable&) = default;

	NonMovable(NonMovable&&) noexcept = delete;
	NonMovable& operator=(NonMovable&&) noexcept = delete;
};

