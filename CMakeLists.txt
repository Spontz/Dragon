cmake_minimum_required(VERSION 2.6)

project(sve)

SET(BUILD_UNICODE ON CACHE BOOL "Unicode")
SET(BUILD_DYNAMIC_CRT OFF CACHE BOOL "Dynamic CRT")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Shared Libs")

IF(BUILD_UNICODE)
	add_definitions(-DUNICODE -D_UNICODE)
ENDIF()

if (MSVC)
	if (NOT MSVC_VERSION LESS 1500)
		set(CMAKE_C_FLAGS "/MP ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")
	endif ()
	if (NOT MSVC_VERSION LESS 1900)
		set(CMAKE_C_FLAGS "/std:c++14 ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "/std:c++14 ${CMAKE_CXX_FLAGS}")
	endif ()
	set(CMAKE_C_FLAGS "/bigobj ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "/bigobj ${CMAKE_CXX_FLAGS}")
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG:NO" )
endif ()

include_directories(dependencies/vs/SDL2-2.0.5/include)
include_directories(dependencies/vs/SDL2_image-2.0.1/include)
include_directories(dependencies/vs/SDL2_net-2.0.1/include)

include_directories(dependencies/vs/fmod10903/lowlevel/inc)

add_subdirectory(sources)

