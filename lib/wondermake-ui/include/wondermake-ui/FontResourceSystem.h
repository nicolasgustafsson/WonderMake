#pragma once

#include "wondermake-ui/FontResource.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/Future.h"
#include "wondermake-utility/SharedReference.h"

class FileResourceSystem;

class FontResourceSystem
	: public System<
		Policy::Set<
			PAdd<FileResourceSystem, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	[[nodiscard]] Future<FileResourceRef<FontResource>> GetFont(const FilePath& aPath);

};
