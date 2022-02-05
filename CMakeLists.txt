cmake_minimum_required(VERSION 3.0)

project(FreeTypeWrapper)
option(FREETYPE_WRAPPER_BUILD_SAMPLES "build freetype wrapper sample" ON)

add_subdirectory(FreeTypeWrapper)

if (FREETYPE_WRAPPER_BUILD_SAMPLES)
	add_subdirectory(Test)
endif()

