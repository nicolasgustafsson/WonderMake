#include "FontResourceFactory.h"

#include "JsonExtensions.h"
#include "JsonUtility.h"

#include "wondermake-ui/TextureResourceSystem.h"

#include "wondermake-base/SystemGlobal.h"

#include <json/json.hpp>

WM_REGISTER_SYSTEM(FontResourceFactory);

Future<SharedReference<FontResource>> FontResourceFactory::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	std::optional<nlohmann::json> jsonFile = JsonUtility::LoadJson(aPath.GetFirstFileFromAllPaths());

	if (!jsonFile)
	{
		WmLogError(TagWonderMake << TagWmResources << "Failed to create font with path " << aPath << '.');

		return MakeCanceledFuture<SharedReference<FontResource>>();
	}

	auto fontInfo = jsonFile->get<SFontInfo>();

	auto& texSys = Get<TextureResourceSystem>();

	return texSys.GetTexture(FilePath::Resolve(fontInfo.AtlasPath))
		.ThenApply(InlineExecutor(), FutureApplyResult([this, aPath, makeOp = std::move(aMakeOp), fontInfo = std::move(fontInfo)](auto aRef) mutable
			{
				WmLogSuccess(TagWonderMake << TagWmResources << "Created font with path " << aPath << '.');

				return MakeCompletedFuture<SharedReference<FontResource>>(makeOp(std::move(aRef), std::move(fontInfo)));
			}));
}
