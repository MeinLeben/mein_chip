cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mein_chip)

if (WIN32)
	set(ARCH "x86")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCH "x64")
	endif()
endif()

set(CMAKE_BUILD_TYPE Debug)

if (WIN32)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/external/cmake/SDL2")
	set(SDL2_DIR "${CMAKE_SOURCE_DIR}/external/SDL2")
	set(SDL2TTFDIR "${CMAKE_SOURCE_DIR}/external/SDL2_ttf")
	set(SDL2TTF_LIBRARY "${CMAKE_SOURCE_DIR}/external/SDL2_ttf/lib/${ARCH}/SDL2_ttf.lib")
	set(SDL2TTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/SDL2_ttf/include/")
endif()

find_package(SDL2 REQUIRED)

if (NOT WIN32)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/external/cmake/SDL2TTF")
	find_package(SDL2TTF REQUIRED)
endif()

add_definitions(-DDEBUG)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})

file(GLOB SOURCE "./src/*.cpp" "./src/*.h")

source_group("src" FILES ${SOURCE})

add_executable(mein_chip ${SOURCE})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT mein_chip)
set_property(TARGET mein_chip PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:mein_chip>")
set_target_properties(mein_chip PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "\"D:\\Development\\Data\\Roms\\chip8-roms\\programs\\Keypad Test [Hap, 2006].ch8\"")

target_link_libraries(mein_chip ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY})
target_precompile_headers(mein_chip PRIVATE src/pch.h)

if (WIN32)

	add_custom_command(TARGET mein_chip POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${SDL2_DIR}/lib/${ARCH}/SDL2.dll"
		$<TARGET_FILE_DIR:mein_chip>)

	add_custom_command(TARGET mein_chip POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${SDL2TTFDIR}/lib/${ARCH}/SDL2_ttf.dll"
		$<TARGET_FILE_DIR:mein_chip>)
endif()

add_custom_command(TARGET mein_chip POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/data/"
	"$<TARGET_FILE_DIR:mein_chip>/data/")
