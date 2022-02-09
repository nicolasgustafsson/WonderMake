cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/json/json.hpp")

add_library(NlohmannJson INTERFACE ${SourceFiles})

target_include_directories(NlohmannJson INTERFACE "Inc")
