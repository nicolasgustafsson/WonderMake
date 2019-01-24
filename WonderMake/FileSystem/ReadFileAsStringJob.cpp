#include "stdafx.h"
#include "ReadFileAsStringJob.h"

#include "Message/Messages.h"

#include <fstream>
#include <string>

void ReadFileAsStringJob::Start()
{
	WmDispatchTask(BindTask(&ReadFileAsStringJob::ReadFile, weak_from_this()), ERoutineId::File);
}

void ReadFileAsStringJob::ReadFile()
{
	if (!std::filesystem::exists(myFilePath))
	{
		CompleteOnRoutine(EResult::Failure, myRoutineChecker.GetRoutineId());
		return;
	}

	std::ifstream fileStream(myFilePath);

	if (!fileStream)
	{
		CompleteOnRoutine(EResult::Failure, myRoutineChecker.GetRoutineId());
		return;
	}

	myOutData.reserve(std::filesystem::file_size(myFilePath));
	myOutData.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	CompleteOnRoutine(EResult::Success, myRoutineChecker.GetRoutineId());
}