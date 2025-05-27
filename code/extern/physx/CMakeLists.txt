cmake_minimum_required(VERSION 3.18)

project(physx C)

#### Build PhysX library ####
set(PHYSX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/physx)
set(PHYSX_ROOT_DIR ${PHYSX_PATH}) #This is needed for $ENV{PHYSX_PATH}/compiler/public/CMakeLists.txt
set(PHYSX_INCLUDE_DIRS $ENV{PHYSX_PATH}/include/ $ENV{PHYSX_PATH}/../pxshared/include/)
set(PHYSX_LIBRARIES PhysXExtensions PhysX PhysXPvdSDK PhysXVehicle PhysXCharacterKinematic PhysXCooking PhysXCommon PhysXFoundation)

set(TARGET_BUILD_PLATFORM "windows") # has to match the TARGET_BUILD_PLATFORM in $ENV{PHYSX_PATH}/physix/buildtools/cmake_generate_projects.py
set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PXSHARED_PATH ${PHYSX_PATH}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_PATH}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Call into PhysX's CMake scripts
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/physx/compiler/public externals/physx)

# References NvidiaBuildOptions.cmake to figure out if system is 32/64 bit
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(LIBPATH_SUFFIX "64")
ELSE()
    SET(LIBPATH_SUFFIX "32")
ENDIF()
GetPlatformBinName(PLATFORM_BIN_NAME ${LIBPATH_SUFFIX})
set(PhysxOutputPath ${PX_OUTPUT_LIB_DIR}/bin/${PLATFORM_BIN_NAME}/)
message("Physx Output Path: " ${PhysxOutputPath})