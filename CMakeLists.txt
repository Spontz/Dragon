cmake_minimum_required(VERSION 2.6)

project(sve)

SET(BUILD_UNICODE ON CACHE BOOL "Unicode")
SET(BUILD_DYNAMIC_CRT OFF CACHE BOOL "Dynamic CRT")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Shared Libs")

IF(BUILD_UNICODE)
	add_definitions(-DUNICODE -D_UNICODE)
ENDIF()

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
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


# SDL2
include_directories(dependencies/vs/SDL2-2.0.5/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vs/SDL2-2.0.5/lib/x64)

# SDL2 Image
include_directories(dependencies/vs/SDL2_image-2.0.1/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vs/SDL2_image-2.0.1/lib/x64)

# SDL2 Net
include_directories(dependencies/vs/SDL2_net-2.0.1/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vs/SDL2_net-2.0.1/lib/x64)

# FMOD
include_directories(dependencies/vs/fmod10903/lowlevel/inc)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vs/fmod10903/lowlevel/lib)

# GLEW
add_definitions(-DGLEW_STATIC)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/vs/glew-2.0.0/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vs/glew-2.0.0/lib/Release/x64)

add_subdirectory(sources)

