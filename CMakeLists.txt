cmake_minimum_required(VERSION 3.0)

project(FreeTypeWrapper)
option(FREETYPE_WRAPPER_BUILD_SAMPLES "build freetype wrapper sample" ON)
option(FREETYPE_WRAPPER_BUILD_FRIBIDI "build fribidi" ON)

if (${FREETYPE_WRAPPER_BUILD_FRIBIDI})
	add_compile_definitions(-DFREETYPE_WRAPPER_BUILD_FRIBIDI=1)
endif()

add_subdirectory(FreeTypeWrapper)

if (FREETYPE_WRAPPER_BUILD_SAMPLES)
	add_subdirectory(Test)
endif()

