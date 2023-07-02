#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/FilePath.h"

inline constexpr auto locPathBin		= "bin";
inline constexpr auto locPathData		= "data";
inline constexpr auto locPathUser		= "user";
inline constexpr auto locPathUserData	= "userdata";

TEST(FilePathTests, filepathdata_extract_bin_paths_correctly)
{
	FilePathData data;

	data.Initialize("bin|bin2|binner:the binening", locPathData, locPathUser, locPathUserData);

	EXPECT_EQ(data.GetPathBin(), "bin");

	EXPECT_EQ(data.GetAllPathsBin(), std::vector<std::filesystem::path>({ "bin", "bin2", "binner:the binening" }));
}

TEST(FilePathTests, filepathdata_extract_data_paths_correctly)
{
	FilePathData data;

	data.Initialize(locPathBin, "data|lore", locPathUser, locPathUserData);

	EXPECT_EQ(data.GetPathData(), "data");

	EXPECT_EQ(data.GetAllPathsData(), std::vector<std::filesystem::path>({ "data", "lore" }));
}

TEST(FilePathTests, filepathdata_extract_user_paths_correctly)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathUser, "user|superuser|poweruser|extrasuperuser", locPathUserData);

	EXPECT_EQ(data.GetPathUser(), "user");

	EXPECT_EQ(data.GetAllPathsUser(), std::vector<std::filesystem::path>({ "user", "superuser", "poweruser", "extrasuperuser" }));
}

TEST(FilePathTests, filepathdata_extract_userdata_paths_correctly)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathUser, locPathUser, "userdata|ud|userd|udata");

	EXPECT_EQ(data.GetPathUserData(), "userdata");

	EXPECT_EQ(data.GetAllPathsUserData(), std::vector<std::filesystem::path>({ "userdata", "ud", "userd", "udata" }));
}

TEST(FilePathTests, default_constructed_filepath_yields_empty_path)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, "");
}

TEST(FilePathTests, filepath_constructed_with_path_yields_same_path)
{
	const std::filesystem::path expectedPath = "test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, filepath_constructed_with_path_and_unset_location_yields_same_path)
{
	const std::filesystem::path expectedPath = "test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::Unset, expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, filepath_constructed_with_path_and_bin_location_yields_path_prefixed_with_data_paths)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::Bin, "test", data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, "bin/test");
}

TEST(FilePathTests, filepath_constructed_with_path_and_data_location_yields_path_prefixed_with_data_paths)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::Data, "test", data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, "data/test");
}

TEST(FilePathTests, filepath_constructed_with_path_and_user_location_yields_path_prefixed_with_data_paths)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::User, "test", data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, "user/test");
}

TEST(FilePathTests, filepath_constructed_with_path_and_userdata_location_yields_path_prefixed_with_data_paths)
{
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::UserData, "test", data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, "userdata/test");
}

TEST(FilePathTests, filepath_constructed_with_absolute_path_and_unset_location_yields_same_path)
{
	const std::filesystem::path expectedPath = "c:/test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::Unset, expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, filepath_constructed_with_absolute_path_and_bin_location_yields_same_path)
{
	const std::filesystem::path expectedPath = "c:/test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::Bin, expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, filepath_constructed_with_absolute_path_and_data_location_yields_same_path)
{
	const std::filesystem::path expectedPath = "c:/test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::Data, expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, filepath_constructed_with_absolute_path_and_user_location_yields_same_path)
{
	const std::filesystem::path expectedPath = "c:/test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::User, expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, filepath_constructed_with_absolute_path_and_userdata_location_yields_same_path)
{
	const std::filesystem::path expectedPath = "c:/test";
	FilePathData data;

	data.Initialize(locPathBin, locPathData, locPathUser, locPathUserData);

	const FilePath filepath(FilePath::EFolder::UserData, expectedPath, data);

	const std::filesystem::path path = filepath;

	EXPECT_EQ(path, expectedPath);
}

TEST(FilePathTests, lexicallynormal_returns_filepath_without_dots)
{
	const FilePath passedFilePath	(FilePath::EFolder::Bin, "path\\..\\path\\.\\to\\..\\to\\dir");
	const FilePath expectedFilePath	(FilePath::EFolder::Bin, "path\\to\\dir");

	EXPECT_EQ(passedFilePath.LexicallyNormal(), expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_is_in_bin_when_path_is_relative)
{
	const std::filesystem::path	path = "path/to/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Bin, "path/to/dir");

	EXPECT_EQ(FilePath::Resolve(path), expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_is_in_bin_when_path_is_inside_bin_dir)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/|d:/bin/",
		"c:/data/|d:/data/",
		"c:/user/|d:/user/",
		"c:/userdata/|d:/userdata/");

	const std::filesystem::path	pathA = "c:/bin/dir";
	const std::filesystem::path	pathB = "d:/bin/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Bin, "dir", data);

	const FilePath				returnedFilePathA = FilePath::Resolve(pathA, data);
	const FilePath				returnedFilePathB = FilePath::Resolve(pathB, data);

	EXPECT_EQ(returnedFilePathA, expectedFilePath);
	EXPECT_EQ(returnedFilePathB, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_is_in_data_when_path_is_inside_data_dir)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/|d:/bin/",
		"c:/data/|d:/data/",
		"c:/user/|d:/user/",
		"c:/userdata/|d:/userdata/");

	const std::filesystem::path	pathA = "c:/data/dir";
	const std::filesystem::path	pathB = "d:/data/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Data, "dir", data);
	
	const FilePath				returnedFilePathA = FilePath::Resolve(pathA, data);
	const FilePath				returnedFilePathB = FilePath::Resolve(pathB, data);

	EXPECT_EQ(returnedFilePathA, expectedFilePath);
	EXPECT_EQ(returnedFilePathB, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_is_in_user_when_path_is_inside_user_dir)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/|d:/bin/",
		"c:/data/|d:/data/",
		"c:/user/|d:/user/",
		"c:/userdata/|d:/userdata/");

	const std::filesystem::path	pathA = "c:/user/dir";
	const std::filesystem::path	pathB = "d:/user/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::User, "dir", data);

	const FilePath				returnedFilePathA = FilePath::Resolve(pathA, data);
	const FilePath				returnedFilePathB = FilePath::Resolve(pathB, data);

	EXPECT_EQ(returnedFilePathA, expectedFilePath);
	EXPECT_EQ(returnedFilePathB, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_is_in_userdata_when_path_is_inside_userdata_dir)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/|d:/bin/",
		"c:/data/|d:/data/",
		"c:/user/|d:/user/",
		"c:/userdata/|d:/userdata/");

	const std::filesystem::path	pathA = "c:/userdata/dir";
	const std::filesystem::path	pathB = "c:/userdata/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::UserData, "dir", data);

	const FilePath				returnedFilePathA = FilePath::Resolve(pathA, data);
	const FilePath				returnedFilePathB = FilePath::Resolve(pathB, data);

	EXPECT_EQ(returnedFilePathA, expectedFilePath);
	EXPECT_EQ(returnedFilePathB, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_is_unset_and_same_path_when_path_is_outside_of_dirs)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/",
		"c:/data/",
		"c:/user/",
		"c:/userdata/");

	const std::filesystem::path	path = "c:/other/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Unset, "c:/other/dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_prioritizes_bin_first)
{
	FilePathData				data;

	data.Initialize(
		"c:/same/",
		"c:/same/",
		"c:/same/",
		"c:/same/");

	const std::filesystem::path	path = "c:/same/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Bin, "dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_prioritizes_data_second)
{
	FilePathData				data;

	data.Initialize(
		"c:/diff/",
		"c:/same/",
		"c:/same/",
		"c:/same/");

	const std::filesystem::path	path = "c:/same/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Data, "dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_prioritizes_user_third)
{
	FilePathData				data;

	data.Initialize(
		"c:/diff/",
		"c:/diff/",
		"c:/same/",
		"c:/same/");

	const std::filesystem::path	path = "c:/same/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::User, "dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_prioritizes_userdata_fourth)
{
	FilePathData				data;

	data.Initialize(
		"c:/diff/",
		"c:/diff/",
		"c:/diff/",
		"c:/same/");

	const std::filesystem::path	path = "c:/same/dir";
	const FilePath				expectedFilePath(FilePath::EFolder::UserData, "dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_removes_dots)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/",
		"c:/data/",
		"c:/user/",
		"c:/userdata/");

	const std::filesystem::path	path = "c:/bin/../bin/./dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Bin, "dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}

TEST(FilePathTests, resolved_filepath_outside_of_dirs_removes_dots)
{
	FilePathData				data;

	data.Initialize(
		"c:/bin/",
		"c:/data/",
		"c:/user/",
		"c:/userdata/");

	const std::filesystem::path	path = "c:/bin/../unset/./dir";
	const FilePath				expectedFilePath(FilePath::EFolder::Unset, "c:/unset/dir", data);

	const FilePath				returnedFilePath = FilePath::Resolve(path, data);

	EXPECT_EQ(returnedFilePath, expectedFilePath);
}
