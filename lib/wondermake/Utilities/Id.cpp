#include "Id.h"

Id IdCounter::NextId()
{
	return Id(myInternalId++);
}
