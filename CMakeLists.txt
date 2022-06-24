cmake_minimum_required(VERSION 3.16)

project(mein_chip)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/external/cmake/SDL2")
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/external/SDL2")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/external/cmake/SDL2TTF")

find_package(SDL2 REQUIRED)
find_package(SDL2TTF REQUIRED)

add_definitions(-DDEBUG)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})

file(GLOB SOURCE "./src/*.cpp" "./src/*.h")

source_group("src" FILES ${SOURCE})

add_executable(mein_chip ${SOURCE})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT mein_chip)

target_link_libraries(mein_chip ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY})

target_precompile_headers(mein_chip PRIVATE src/pch.h)

if (WIN32)
	set(ARCH "x86")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCH "x64")
	endif()

	add_custom_command(TARGET mein_chip POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${SDL2_DIR}lib/${ARCH}/SDL2.dll"
		$<TARGET_FILE_DIR:mein_chip>)
endif()

add_custom_command(TARGET mein_chip POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/data/"
	$<TARGET_FILE_DIR:mein_chip>"/data/")
