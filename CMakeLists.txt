cmake_minimum_required(VERSION 3.15)
project(Hyper)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(MSDFGEN_DYNAMIC_RUNTIME ON)

set(NVRHI_WITH_DX12 ON CACHE BOOL "Enable the NVRHI D3D12 backend" FORCE)
set(NVRHI_WITH_AFTERMATH ON CACHE BOOL "Include Aftermath support (requires NSight Aftermath SDK)" FORCE)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL" CACHE STRING "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)

add_subdirectory("Thirdparty/NVRHI")
add_subdirectory("Thirdparty/spdlog")
add_subdirectory("Thirdparty/enet6")
add_subdirectory("Thirdparty/SDL3")
add_subdirectory("Thirdparty/ShaderMake")
add_subdirectory("Thirdparty/stb")
add_subdirectory("Thirdparty/glfw")
add_subdirectory("Thirdparty/imgui-cmake")
add_subdirectory("Thirdparty/freetype")
add_subdirectory("Thirdparty/msdf-atlas-gen")
add_subdirectory("Thirdparty/JoltPhysics/Build")
add_subdirectory(Game)
add_subdirectory(Runtime)
add_subdirectory(Server)
add_subdirectory(Editors)