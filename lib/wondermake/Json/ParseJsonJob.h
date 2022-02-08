#pragma once

#include "Job/Job.h"
#include "Utilities/Container/Container.h"

#include "Json/json.hpp"

class ParseJsonJob
	: public Job<
		Policy::Set<>,
		JobOutput<json>,
		JobOutputError<json::exception>>
{
public:
	ParseJsonJob(Container<u8, Iterable, Indexable, ContiguousElements, EqualityComparable> aContainer);
	ParseJsonJob(std::string aText);

};
