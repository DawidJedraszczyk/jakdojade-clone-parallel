cmake_minimum_required(VERSION 3.10)
project(rownolegle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)
find_package(CURL REQUIRED)

include_directories(${LIBPQXX_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/include)

link_directories(${LIBPQXX_LIBRARY_DIRS})

add_executable(rownolegle src/main.cpp src/algorithm.cpp src/database_queries.cpp)

target_link_libraries(rownolegle ${LIBPQXX_LIBRARIES} ${CURL_LIBRARIES})
