#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/FilePath.h"

inline constexpr auto locPathBin		= "bin";
inline constexpr auto locPathData		= "data";
inline constexpr auto locPathUser		= "user";
inline constexpr auto locPathUserData	= "userdata";

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
