
#include "GenerateRegistry.h"

#include <iostream>
#include <string>

int main(int aArgCount, char* aArgs[])
{
	if (aArgCount < 2)
	{
		std::cout << "Missing config file!\n";

		return 1;
	}

	std::string configPath(aArgs[1]);

	if (!GenerateRegistry(configPath))
	{
		return 1;
	}

	return 0;
}
