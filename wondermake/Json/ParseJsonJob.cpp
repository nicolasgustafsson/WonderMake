#include "pch.h"
#include "ParseJsonJob.h"

ParseJsonJob::ParseJsonJob(Container<u8, Iterable, Indexable, ContiguousElements, EqualityComparable> aContainer)
	: ParseJsonJob(std::string(aContainer.begin(), aContainer.end()))
{}

ParseJsonJob::ParseJsonJob(std::string aText)
{
	try
	{
		auto parsedJson = json::parse(std::move(aText));

		CompleteSuccess(std::move(parsedJson));
	}
	catch (json::exception& e)
	{
		CompleteFailure(e);
	}
}
