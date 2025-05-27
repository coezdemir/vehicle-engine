
# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "include/**.h")

# Add an executable target
add_library(Engine ${SOURCE_LIST} ${HEADER_LIST} ${PUBLIC_HEADER_LIST})

# Require C++ 17 compiler
target_compile_features(Engine PRIVATE cxx_std_17)

# Link the executable with required libraries
target_link_libraries(Engine PUBLIC glm glad glfw Memory)
target_compile_definitions(Engine PUBLIC GLFW_INCLUDE_NONE)

# Add header directory
target_include_directories(Engine PUBLIC include/)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src/)
set_target_properties(Engine PROPERTIES LINKER_LANGUAGE CXX)

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Public Header" FILES ${PUBLIC_HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/generated" PREFIX "Generated Files" FILES ${GENERATED_FILES_LIST})