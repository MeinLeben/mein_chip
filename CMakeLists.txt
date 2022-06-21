cmake_minimum_required(VERSION 3.10)

project(mein_chip)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB SOURCE "./src/*.cpp" "./src/*.h")

source_group("src" FILES ${SOURCE})

add_executable(mein_chip ${SOURCE})

target_link_libraries(mein_chip ${SDL2_LIBRARIES})
