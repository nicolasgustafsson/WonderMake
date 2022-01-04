#pragma once
class Path
{
public:
	Path(const std::filesystem::path& aPath);
	std::filesystem::path myPath;

	operator std::filesystem::path() { return myPath; };
};

