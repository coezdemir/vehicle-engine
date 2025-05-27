file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "include/**.h")

# Add an executable target
add_library(Renderer ${SOURCE_LIST} ${HEADER_LIST} ${PUBLIC_HEADER_LIST} ${GENERATED_FILES_LIST})

# Require C++ 17 compiler
target_compile_features(Renderer PRIVATE cxx_std_17)

# Link the executable with required libraries
target_link_libraries(Renderer PUBLIC Engine Scene assimp glad glm glfw)

# Add header directory
target_include_directories(Renderer PUBLIC include/ ../extern/stb/)

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Public Header" FILES ${PUBLIC_HEADER_LIST})
