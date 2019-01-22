#include "stdafx.h"
#include "ReadFileAsString.h"

#include "Message/Messages.h"

#include <fstream>
#include <string>

void ReadFileAsString::Start()
{
	WmDispatchTask(BindTask(&ReadFileAsString::ReadFile, weak_from_this()), ERoutineId::File);
}

void ReadFileAsString::ReadFile()
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

	fileStream.seekg(0, std::ios::end);
	myOutData.reserve(fileStream.tellg());
	fileStream.seekg(0, std::ios::beg);
	myOutData.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	CompleteOnRoutine(EResult::Success, myRoutineChecker.GetRoutineId());
}