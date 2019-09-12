
#include "GenerateRegistry.h"

int main()
{
	// TODO(Kevin): Use command line argument instead.
	if (!GenerateRegistry("registryConfig.json"))
	{
		return 1;
	}

	return 0;
}
