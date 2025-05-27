cmake_minimum_required(VERSION 3.18)

# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")

# set executable name
set(EXE_FILE Demo)

# add the executable target
add_executable(${EXE_FILE} ${SOURCE_LIST} ${HEADER_LIST} ${IMGUI_SOURCE_LIST})

# require C++ 17 compiler
target_compile_features(${EXE_FILE} PRIVATE cxx_std_17)

# link with dependencies
target_link_libraries(${EXE_FILE} PRIVATE glad glfw glm Engine Renderer Scene Physics UserInterface Audio)

target_compile_definitions(${EXE_FILE} PRIVATE DEBUG_ASSET_ROOT=${PROJECT_SOURCE_DIR})

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/generated" PREFIX "Generated Files" FILES ${GENERATED_FILES_LIST})
