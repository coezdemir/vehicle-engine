cmake_minimum_required(VERSION 3.18)

file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "**.h")

# Add an executable target
add_library(ImGUI ${SOURCE_LIST} ${HEADER_LIST} ${GENERATED_FILES_LIST})

# Require C++ 17 compiler
target_compile_features(ImGUI PRIVATE cxx_std_17)

target_link_libraries(ImGUI PRIVATE glfw glad)

# Add header directory
target_include_directories(ImGUI PUBLIC /)

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Private Header" FILES ${HEADER_LIST})