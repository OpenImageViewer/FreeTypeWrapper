#Freetype Wrapper

set(CMAKE_CXX_STANDARD 20)

 file(GLOB_RECURSE sourceFiles
     "./Source/*.h"
     "./Source/*.cpp"
 )
 
 
option(FREETYPE_WRAPPER_DISABLE_WARNINGS_EXTERNAL_LIBS "Disable warnings for external libraries" TRUE)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_compile_definitions(NOMINMAX)
endif()

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

        target_compile_options(freetype PRIVATE -Wno-documentation 
            -Wno-documentation-unknown-command
            -Wno-unsafe-buffer-usage
            -Wno-cast-qual
            -Wno-sign-conversion
            -Wno-unused-macros
            -Wno-switch-enum
            -Wno-tautological-type-limit-compare
            -Wno-implicit-fallthrough
            -Wno-shorten-64-to-32
            -Wno-cast-align
            -Wno-cast-align
            -Wno-implicit-int-conversion
            -Wno-nonportable-system-include-path
            -Wno-missing-noreturn
            -Wno-extra-semi-stmt
        )
        if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
            target_compile_options(libfribidi PRIVATE 
            -Wno-undef 
            -Wno-unused-macros 
            -Wno-sign-conversion
            -Wno-missing-variable-declarations
            -Wno-cast-qual
            -Wno-extra-semi-stmt
            -Wno-assign-enum
            -Wno-unused-function
            -Wno-implicit-int-conversion
            -Wno-shorten-64-to-32
            -Wno-enum-conversion
            -Wno-shadow
            -Wno-unsafe-buffer-usage
            -Wno-reserved-identifier
            
            )
        endif()
    
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(freetype PRIVATE /wd4267 /wd4244 /wd4267 /wd4244)
        if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
            target_compile_options(libfribidi PRIVATE /wd4244)
        endif()
    endif()
endif()

target_link_libraries(${TargetName} PRIVATE freetype)
if (FREETYPE_WRAPPER_BUILD_FRIBIDI)
    target_link_libraries(${TargetName} PRIVATE libfribidi)
endif()