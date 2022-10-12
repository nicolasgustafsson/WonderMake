cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "include/wondermake-utility/Alias.h"
    "include/wondermake-utility/AnyExecutor.h"
    "include/wondermake-utility/AutoRegister.h"
    "include/wondermake-utility/Bindable.h"
    "include/wondermake-utility/BufferExecutor.h"
    "include/wondermake-utility/CmdLineArgs.h"
    "include/wondermake-utility/DependencyInjector.h"
    "include/wondermake-utility/Endian.h"
    "include/wondermake-utility/Executor.h"
    "include/wondermake-utility/FixedSizeString.h"
    "include/wondermake-utility/Future.h"
    "include/wondermake-utility/Guid.h"
    "include/wondermake-utility/Line.h"
    "include/wondermake-utility/Math.h"
    "include/wondermake-utility/Matrix.h"
    "include/wondermake-utility/MemoryUnit.h"
    "include/wondermake-utility/plf_colony.h"
    "include/wondermake-utility/RestrictTypes.h"
    "include/wondermake-utility/Result.h"
    "include/wondermake-utility/Rotation.h"
    "include/wondermake-utility/SharedReference.h"
    "include/wondermake-utility/Singleton.h"
    "include/wondermake-utility/Stopwatch.h"
    "include/wondermake-utility/StringUtility.h"
    "include/wondermake-utility/Time.h"
    "include/wondermake-utility/TupleUtility.h"
    "include/wondermake-utility/Typedefs.h"
    "include/wondermake-utility/UniqueFunction.h"
    "include/wondermake-utility/UniqueValue.h"
    "include/wondermake-utility/Utility.h"
    "include/wondermake-utility/Vector.h"
    "include/wondermake-utility/WinPlatform.h"
    "source/AnyExecutor.cpp"
    "source/CmdLineArgs.cpp"
    "source/DependencyInjector.cpp"
    "source/Time.cpp"
    "source/WinStringUtility.cpp")

add_library(WonderMakeUtility STATIC ${SourceFiles})

set_pedantic_warning_level(WonderMakeUtility)
target_include_directories(WonderMakeUtility PUBLIC "Include")
