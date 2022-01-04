#pragma once
#include "File.h"
#include "Utilities/String/Name.h"
#include "Utilities/String/Path.h"

//contains a list of subfolders and files
class Folder
{
public:
	explicit Folder(Path aPath);
private:
	Name myName;
	Path myPath;

	Container<File> myFiles;
	std::vector<Folder> mySubfolders;
	//path

	void Initialize();

	void Inspect();

};

