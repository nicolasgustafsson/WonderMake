#include "wondermake-ui/FontResourceSystem.h"

#include "wondermake-io/FileResourceSystem.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(FontResourceSystem);

Future<FileResourceRef<FontResource>> FontResourceSystem::GetFont(const FilePath& aPath)
{
	return Get<FileResourceSystem>()
		.GetResource<FontResource>(aPath);
}
