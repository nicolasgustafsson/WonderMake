#pragma once

#include "wondermake-base/ProcessSystem.h"

#include "wondermake-utility/WinPlatform.h"

#include <memory>

class WinEventSystem;
class WinPlatformSystem;

class WinProcessSystem
	: public SystemSub<
		Policy::Set<
			PAdd<WinEventSystem, PWrite>,
			PAdd<WinPlatformSystem, PWrite>>>
	, public ProcessSystem
	, public std::enable_shared_from_this<WinProcessSystem>
{
public:
	Result<EStartError, std::shared_ptr<Process>> StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine) override;

};
