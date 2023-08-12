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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    #enabled warnings flags
    add_compile_options(-Wextra -Wpedantic -Wall -Wpessimizing-move)
    #specifically disable the following warning flags
    add_compile_options(-Wno-c++98-compat -Wno-c++11-compat -Wno-c++14-compat -Wno-c++98-compat-pedantic -Wno-reserved-id-macro -Wno-newline-eof -Wno-global-constructors -Wno-exit-time-destructors -Wno-float-equal -Wno-covered-switch-default -Wno-unreachable-code-break)
 
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   
# using Visual Studio C++
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/Wall /wd4668 /wd5039 /wd4514 /wd4820 /wd4625 /wd4626 /wd4710 /wd5045" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()

if (FREETYPE_WRAPPER_DISABLE_WARNINGS_EXTERNAL_LIBS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    #    target_compile_options(freetype PRIVATE -Wno-deprecated-declarations)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(freetype PRIVATE /wd4267 /wd4244 /wd4267 /wd4244)
    endif()
endif()

#add_subdirectory(${FreeTypePath} ${FreeTypeOutput})
target_link_libraries(${TargetName} PRIVATE freetype)
if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
    target_link_libraries(${TargetName} PRIVATE libfribidi)
endif()