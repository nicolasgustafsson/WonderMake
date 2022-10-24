#pragma once

#include <functional>
#include <string>
#include <vector>

class Debugged
{
public:
	Debugged(const std::string aName) noexcept;
	virtual ~Debugged() noexcept = default;

	static std::vector<std::pair<std::string, std::function<void()>>> GetAndResetDebugged();

protected:
	virtual void Debug() = 0;

private:
	static std::vector<std::pair<std::string, std::function<void()>>> ourDebugged;

	std::string myDebugName;

};

