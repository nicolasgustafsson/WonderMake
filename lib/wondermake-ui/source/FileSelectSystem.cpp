#include "wondermake-ui/FileSelectSystem.h"

#include "wondermake-ui/ImguiInclude.h"

#include "wondermake-base/GuidGeneratorSystem.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/SystemGlobal.h"

#include <numeric>

WM_REGISTER_SYSTEM(FileSelectSystem);

void FileSelectSystem::Initialize()
{
	Get<ScheduleSystemSingleton>()
		.ScheduleRepeating(GetExecutor(), [this]() { Tick(); })
		.Detach();
}

Future<FilePath> FileSelectSystem::OpenFileBrowser(std::string aWindowTitle, FilePath aStartPath, EFileSelectFlags aFlags)
{
	auto id = Get<GuidGeneratorSystem>().GenerateNew();

	if (!id)
		return MakeCanceledFuture<FilePath>();

	auto [promise, future] = MakeAsync<FilePath>();

	std::unordered_map<FilePath, bool> paths;
	FilePath displayPath = aStartPath;

	GetFilesAndFolders(aStartPath, paths);

	aWindowTitle += "###";

	aWindowTitle += id->ToFixedSizeString().ToStringView();

	if (!std::filesystem::is_directory(displayPath))
		displayPath.Path = displayPath.Path.parent_path();

	myOpenFileRequests.Emplace(SOpenFileData
		{
			.Promise			= std::move(promise),
			.WindowTitleWithId	= std::move(aWindowTitle),
			.CurrentPath		= std::move(aStartPath),
			.Id					= *id,
			.SelectFlags		= aFlags,
			.DisplayPath		= std::move(displayPath),
			.Paths				= std::move(paths)
		});

	return std::move(future);
}

void FileSelectSystem::Tick()
{
	for (auto it = myOpenFileRequests.begin(); it != myOpenFileRequests.end();)
	{
		auto& data = *it;

		auto resultOpt = TickWindow(data.WindowTitleWithId, data.CurrentPath, data.SelectFlags, data.DisplayPath, data.Paths, data.Selection);

		if (!resultOpt)
		{
			++it;

			continue;
		}

		auto& result = *resultOpt;

		if (result)
			data.Promise.Complete(std::move(data.CurrentPath));
		
		it = myOpenFileRequests.erase(it);
	}
}

std::optional<bool> FileSelectSystem::TickWindow(const std::string& aWindowTitleWithId, FilePath& aCurrentPath, EFileSelectFlags aSelectFlags, FilePath& aDisplayPath, std::unordered_map<FilePath, bool>& aPaths, ESelection& aSelection)
{
	static bool isOpen = true;

	isOpen = true;
	const bool hasParent =
		(aSelection == ESelection::Parent || aCurrentPath.Location != FilePath::EFolder::Unset) &&
		(aSelection != ESelection::Child || !aCurrentPath.Path.empty());

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_Appearing);

	if (!ImGui::Begin(aWindowTitleWithId.c_str(), &isOpen))
	{
		ImGui::End();

		return std::nullopt;
	}

	if (!isOpen)
	{
		ImGui::End();

		return false;
	}
	
	TickWindowHeader(aCurrentPath, aDisplayPath, aPaths, aSelection);
	
	if (TickWindowContent(aCurrentPath, aSelectFlags, aDisplayPath, aPaths, aSelection))
	{
		ImGui::End();

		return true;
	}

	bool selectable =
		aCurrentPath.Location != FilePath::EFolder::Unset
		&& (FlagSet(aSelectFlags).TestAll(EFileSelectFlags::AllowFiles | EFileSelectFlags::AllowDirectories) ||
			FlagSet(aSelectFlags).TestAll(EFileSelectFlags::AllowDirectories) == std::filesystem::is_directory(aCurrentPath.GetFirstFileFromAllPaths()));

	if (TickWindowFooter(selectable))
	{
		ImGui::End();

		return true;
	}

	ImGui::End();

	return std::nullopt;
}

void FileSelectSystem::TickWindowHeader(FilePath& aCurrentPath, FilePath& aDisplayPath, std::unordered_map<FilePath, bool>& aPaths, ESelection& aSelection)
{
	ImGui::PushID("header");

	const bool hasParent =
		(aSelection == ESelection::Parent || aCurrentPath.Location != FilePath::EFolder::Unset) &&
		(aSelection != ESelection::Child || !aCurrentPath.Path.empty());

	if (!hasParent)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("<"))
	{
		aCurrentPath.Path.clear();
		aCurrentPath.Location = FilePath::EFolder::Unset;
		aDisplayPath.Path.clear();
		aDisplayPath.Location = FilePath::EFolder::Unset;
		aSelection = ESelection::Current;

		ImGui::PopID();

		return;
	}
	
	if (!hasParent)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SameLine();

	if (hasParent)
	{
		auto getRootName = [&aDisplayPath]() -> const char*
		{
			switch (aDisplayPath.Location)
			{
			case FilePath::EFolder::Unset:		return "unset";
			case FilePath::EFolder::Bin:		return "bin";
			case FilePath::EFolder::Data:		return "data";
			case FilePath::EFolder::User:		return "user";
			case FilePath::EFolder::UserData:	return "userdata";
			}

			return "unset";
		};

		if (ImGui::Button(getRootName()))
		{
			aDisplayPath.Path.clear();
			aCurrentPath.Path.clear();
			aSelection = ESelection::Current;

			GetFilesAndFolders(aCurrentPath, aPaths);

			ImGui::PopID();

			return;
		}

		ImGui::SameLine();

		ImGui::TextUnformatted(":/");

		ImGui::SameLine();

		for (const auto& [i, part] : Utility::Enumerate(aDisplayPath.Path))
		{
			ImGui::PushID(static_cast<int>(i));

			if (ImGui::Button(part.string().c_str()))
			{
				auto it = aDisplayPath.Path.begin();

				++it;

				for (size_t j = 0; j < i; ++j)
					++it;

				aDisplayPath.Path = std::accumulate(aDisplayPath.Path.begin(), it, std::filesystem::path{}, std::divides{});

				aCurrentPath = aDisplayPath;
				aSelection = ESelection::Current;

				GetFilesAndFolders(aCurrentPath, aPaths);

				ImGui::PopID();
				ImGui::PopID();

				return;
			}

			ImGui::SameLine();

			ImGui::TextUnformatted("/");

			ImGui::SameLine();

			ImGui::PopID();
		}

		ImGui::NewLine();
	}
	else
	{
		ImGui::TextUnformatted("Select root");
	}

	ImGui::Separator();

	ImGui::PopID();

	return;
}

bool FileSelectSystem::TickWindowContent(FilePath& aCurrentPath, EFileSelectFlags aSelectFlags, FilePath& aDisplayPath, std::unordered_map<FilePath, bool>& aPaths, ESelection& aSelection)
{
	const bool hasParent =
		(aSelection == ESelection::Parent || aCurrentPath.Location != FilePath::EFolder::Unset) &&
		(aSelection != ESelection::Child || !aCurrentPath.Path.empty());

	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	ImGui::BeginChild("###PathSelection", ImVec2(0, ImGui::GetContentRegionAvail().y - footerHeight));

	if (!hasParent)
	{
		auto selectable = [this, &aCurrentPath, &aDisplayPath, &aPaths, &aSelection](auto aText, FilePath::EFolder aFolder)
		{
			if (ImGui::Selectable(aText, aCurrentPath.Location == aFolder))
			{
				aCurrentPath = FilePath(aFolder, "");
				aSelection = ESelection::Child;
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				aSelection = ESelection::Current;
				aDisplayPath = aCurrentPath;

				GetFilesAndFolders(aCurrentPath, aPaths);
			}
		};

		selectable("bin:/", FilePath::EFolder::Bin);
		selectable("data:/", FilePath::EFolder::Data);
		selectable("user:/", FilePath::EFolder::User);
		selectable("userdata:/", FilePath::EFolder::UserData);

		ImGui::EndChild();

		return false;
	}

	if (ImGui::Selectable(".", aSelection == ESelection::Current))
	{
		aSelection = ESelection::Current;
		aCurrentPath = aDisplayPath;
	}

	if (ImGui::Selectable("..", aSelection == ESelection::Parent))
	{
		aSelection = ESelection::Parent;
		if (aDisplayPath.Path.empty())
			aCurrentPath.Location = FilePath::EFolder::Unset;
		else
			aCurrentPath.Path = aDisplayPath.Path.parent_path();
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		aSelection = ESelection::Current;
		aDisplayPath = aCurrentPath;

		GetFilesAndFolders(aCurrentPath, aPaths);
	}

	for (const auto& [path, isDir] : aPaths)
	{
		if (!isDir && !FlagSet(aSelectFlags).TestAll(EFileSelectFlags::AllowFiles))
			continue;

		bool isSelected = path == aCurrentPath;
		auto name = path.Path.filename().string();

		if (isDir)
			name += '/';

		if (ImGui::Selectable(name.c_str(), isSelected))
		{
			aCurrentPath = path;
			aSelection = ESelection::Child;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (!isDir)
			{
				ImGui::EndChild();

				return true;
			}

			aSelection = ESelection::Current;
			aDisplayPath = aCurrentPath;

			GetFilesAndFolders(aCurrentPath, aPaths);

			break;
		}
	}

	ImGui::EndChild();

	return false;
}

bool FileSelectSystem::TickWindowFooter(bool aIsSelectable)
{
	ImGui::Separator();

	if (!aIsSelectable)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	bool select = ImGui::Button("Select");

	if (!aIsSelectable)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	return select;
}

void FileSelectSystem::GetFilesAndFolders(const FilePath& aPath, std::unordered_map<FilePath, bool>& aPaths)
{
	aPaths.clear();

	if (aPath.Location == FilePath::EFolder::Unset)
		return;

	auto firstPath = aPath.GetFirstFileFromAllPaths();
	FilePath path = aPath;

	if (!std::filesystem::is_directory(firstPath))
		path.Path = path.Path.parent_path();

	auto folderPaths = path.GetAllPaths();

	for (const auto& folderPath : folderPaths)
	{
		if (!std::filesystem::is_directory(folderPath))
			continue;

		for (const auto& filePath : std::filesystem::directory_iterator(folderPath))
			aPaths.emplace(std::make_pair(FilePath::Resolve(filePath.path()), std::filesystem::is_directory(filePath)));
	}
}
