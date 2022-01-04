#include "pch.h"
#include "Folder.h"

Folder::Folder(Path aPath)
	: myPath(aPath)
{
	Initialize();
}

void Folder::Initialize()
{
	for (auto&& entry : std::filesystem::directory_iterator(myPath))
	{
		if (entry.is_directory())
		{

		}
		else if (entry.is_regular_file())
		{

		}
		//currently not handling any other case
	}
}

void Folder::Inspect()
{

}
