# CMakeList.txt : CMake project for GFXRendererD3D11, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


set(CMAKE_CXX_STANDARD 20)

if(NOT TARGET FreeTypeTest)
    include_directories(../External/FreeTypeWrapper/Include)
	# Add source to this project's executable.
	add_executable (FreeTypeTest "Test.cpp")

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
endif()

target_include_directories(FreeTypeTest PRIVATE ../FreeTypeWrapper/Include)
target_include_directories(FreeTypeTest PRIVATE ../FreeTypeWrapper/External/LLUtils/Include)

target_link_libraries(FreeTypeTest PRIVATE FreeTypeWrapper)