#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/LogTag.h"

void StaticTest_LoggerTag()
{
	static constexpr LogTag<5> tagHello("Hello");
	static constexpr LogTag<5> tagWorld = MakeLogTag("World");

	static_assert(tagHello.FormattedSize == 8);
	static_assert(tagWorld.FormattedSize == 8);

	static_assert(tagHello.GetFormattedString() == "[Hello] ");
	static_assert(tagWorld.GetFormattedString() == "[World] ");
}
