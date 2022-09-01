# WonderMake

## Introduction
WonderMake is a game engine written using c++ 20. It's main design point is that you have to list all dependencies when declaring different systems or functionalities. These dependencies will be created and then be injected so they're ready to be used before the constructor of your system/functionality is called.

## Dependencies
- [CMake 3.19](https://cmake.org/)
- [Python 3.7](https://www.python.org/)
- [Conan 1.51.3](https://conan.io/)

## Using WonderMake
In order to use WonderMake in your project, you need to use CMake to generate the project. Any external dependencies are downloaded using Conan, which is written in Python. So make sure all the correct versions are installed before trying to use WonderMake.

To setup your application, simply add the WonderMake folder as a subdirectory in CMake. Without changing any options, this will:
- Set the C++ standard
- Set the available configurations
- Download any external dependencies
- Setup the projects required to link against WonderMake

## Building via CMake
First run cmake. As an example, create a folder called "out" in the project folder, then run the following command in the folder:

`cmake -DCMAKE_BUILD_TYPE=Debug ..`

To build the engine, run this command in the "out" folder:

`cmake --build . --config Debug`

This should result in all library files being compiled into static libraries that you can link against.
