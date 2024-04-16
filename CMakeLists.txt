cmake_minimum_required(VERSION 3.10)

project(Practice)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG(-O3 COMPILER_SUPPORTS_O3)
if(COMPILER_SUPPORTS_O3)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

CHECK_CXX_COMPILER_FLAG(-g COMPILER_SUPPORTS_G)
if(COMPILER_SUPPORTS_G)
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

include_directories(${PROJECT_SOURCE_DIR}/lib/cli11/)
include_directories(${PROJECT_SOURCE_DIR}/lib/linalg/)
include_directories(${PROJECT_SOURCE_DIR}/lib/flip/)
include_directories(${PROJECT_SOURCE_DIR}/lib/filesystem/)
include_directories(${PROJECT_SOURCE_DIR}/lib/fmt/)
include_directories(${PROJECT_SOURCE_DIR}/lib/nanothread/)
include_directories(${PROJECT_SOURCE_DIR}/lib/pcg32/)
include_directories(${PROJECT_SOURCE_DIR}/lib/spdlog/)
include_directories(${PROJECT_SOURCE_DIR}/lib/stb/)
include_directories(${PROJECT_SOURCE_DIR}/lib/tinyexr/)
include_directories(${PROJECT_SOURCE_DIR}/lib/tinyobjloader/)
include_directories(${PROJECT_SOURCE_DIR}/lib/nlohmann/)

file(GLOB_RECURSE _source_list ${PROJECT_SOURCE_DIR}/source/*.cpp ${PROJECT_SOURCE_DIR}/source/*.hpp ${PROJECT_SOURCE_DIR}/scenes/*.json)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

add_executable(${PROJECT_NAME} ${_source_list})

target_link_libraries(${PROJECT_NAME} Threads::Threads)
