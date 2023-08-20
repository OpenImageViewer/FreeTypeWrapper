#Freetype Wrapper

set(CMAKE_CXX_STANDARD 20)

 file(GLOB_RECURSE sourceFiles
     "./Source/*.h"
     "./Source/*.cpp"
 )
 
 
option(FREETYPE_WRAPPER_DISABLE_WARNINGS_EXTERNAL_LIBS "Disable warnings for external libraries" TRUE)

set(TargetName FreeTypeWrapper)

add_subdirectory(./External/freetype2)

option(FRIBIDI_ENABLE_DEBUG OFF)
option(FRIBIDI_BUILD_TESTS OFF)
option(FRIBIDI_BUILD_BIN OFF)

add_library(${TargetName} ${sourceFiles})

if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
	target_compile_definitions(${TargetName} PRIVATE FREETYPE_WRAPPER_BUILD_FRIBIDI=1)
    add_subdirectory(./External/fribidi)
endif()


target_include_directories(${TargetName} PRIVATE ./External/freetype2/include)
if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
    target_include_directories(${TargetName} PRIVATE ./External/fribidi/lib)
endif()
target_include_directories(${TargetName} PRIVATE ./External/utf-cpp/include)
target_include_directories(${TargetName} PRIVATE ./Source)
target_include_directories(${TargetName} PRIVATE ./Include)
target_include_directories(${TargetName} PRIVATE ./External/LLUtils/Include)


if (FREETYPE_WRAPPER_DISABLE_WARNINGS_EXTERNAL_LIBS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    #    target_compile_options(freetype PRIVATE -Wno-deprecated-declarations)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(freetype PRIVATE /wd4267 /wd4244 /wd4267 /wd4244)
        if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
            target_compile_options(libfribidi PRIVATE /wd4244)
        endif()
    endif()
endif()

#add_subdirectory(${FreeTypePath} ${FreeTypeOutput})
target_link_libraries(${TargetName} PRIVATE freetype)
if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
    target_link_libraries(${TargetName} PRIVATE libfribidi)
endif()