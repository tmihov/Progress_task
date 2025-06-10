cmake_minimum_required(VERSION 3.15...4.0)

project(ProgressUploader VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
include(cmake/FindLibNlohmann_json.cmake)
find_package(CURL REQUIRED)
add_subdirectory(ProgressUploader)