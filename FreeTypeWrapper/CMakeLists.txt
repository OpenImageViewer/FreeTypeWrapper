#Freetype Wrapper
 file(GLOB_RECURSE sourceFiles
     "./Source/*.h"
     "./Source/*.cpp"
 )

set(External ./External)
set(FreeTypePath ./External/freetype2)
set(FreeTypeOutput ./ExternalBuild/freetype2)
set(TargetName FreeTypeWrapper)

add_library(${TargetName} ${sourceFiles})
target_include_directories(${TargetName} PRIVATE ${FreeTypePath}/include)
target_include_directories(${TargetName} PRIVATE ${External}/LLUtils/Include)

add_subdirectory(${FreeTypePath} ${FreeTypeOutput})
target_link_libraries(${TargetName} PRIVATE freetype)