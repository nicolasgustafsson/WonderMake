#pragma once

#include "GTestInclude.h"

#include "WonderMakeEngine/Process.h"

class ProcessMock
	: public Process
{
public:
	MOCK_METHOD(EState, GetState, (), (const override));

	MOCK_METHOD(void, Terminate, (
		i64 aExitCode), (override));

	MOCK_METHOD(void, OnExit, (
		OnExitCallback&& aOnExit), (override));
};
