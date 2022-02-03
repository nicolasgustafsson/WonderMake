cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeUtility/Alias.h"
    "Include/WonderMakeUtility/AutoRegister.h"
    "Include/WonderMakeUtility/Bindable.h"
    "Include/WonderMakeUtility/DependencyInjector.h"
    "Include/WonderMakeUtility/Guid.h"
    "Include/WonderMakeUtility/Line.h"
    "Include/WonderMakeUtility/Math.h"
    "Include/WonderMakeUtility/Matrix.h"
    "Include/WonderMakeUtility/MemoryUnit.h"
    "Include/WonderMakeUtility/plf_colony.h"
    "Include/WonderMakeUtility/RestrictTypes.h"
    "Include/WonderMakeUtility/Result.h"
    "Include/WonderMakeUtility/Rotation.h"
    "Include/WonderMakeUtility/SharedReference.h"
    "Include/WonderMakeUtility/Singleton.h"
    "Include/WonderMakeUtility/Stopwatch.h"
    "Include/WonderMakeUtility/StringUtility.h"
    "Include/WonderMakeUtility/Time.h"
    "Include/WonderMakeUtility/Typedefs.h"
    "Include/WonderMakeUtility/UniqueFunction.h"
    "Include/WonderMakeUtility/UniqueValue.h"
    "Include/WonderMakeUtility/Utility.h"
    "Include/WonderMakeUtility/Vector.h"
    "Include/WonderMakeUtility/WinPlatform.h"
    "Source/DependencyInjector.cpp"
    "Source/Time.cpp"
    "Source/WinStringUtility.cpp")

add_library(WonderMakeUtility STATIC ${SourceFiles})

set_target_properties(WonderMakeUtility PROPERTIES FOLDER "wondermake/lib")

target_include_directories(WonderMakeUtility PUBLIC "Include")
