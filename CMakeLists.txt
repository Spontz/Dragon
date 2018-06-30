cmake_minimum_required(VERSION 2.6)

project(Dragon)

SET(BUILD_UNICODE ON CACHE BOOL "Unicode")

if(MSVC)
	option(STATIC_CRT "Statically link MSVC CRT" ON)
endif()

if(MSVC AND STATIC_CRT)
    add_compile_options(
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:RelWithDebInfo>:/MT>"
        "$<$<CONFIG:Release>:/MT>"
        "$<$<CONFIG:MinSizeRel>:/MT>"
    )
endif()

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
include_directories(dependencies/vc141/SDL2-2.0.5/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2-2.0.5/lib/x64)

# SDL2 Image
include_directories(dependencies/vc141/SDL2_image-2.0.1/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_image-2.0.1/lib/x64)

# SDL2 Net
include_directories(dependencies/vc141/SDL2_net-2.0.1/include)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_net-2.0.1/lib/x64)

# BASS
include_directories(dependencies/vc141/bass24/inc)
link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/bass24/lib/x64)

# GLEW
add_definitions(-DGLEW_STATIC)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/glew-2.1.0/include)
if(CMAKE_CL_64)
	if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
		link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/glew-2.1.0/lib/Debug/x64)
	else()
		link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/glew-2.1.0/lib/Release/x64)
	endif()
else()
	if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
		link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/glew-2.1.0/lib/Debug/Win32)
	else()
		link_directories(${CMAKE_SOURCE_DIR}/dependencies/vc141/glew-2.1.0/lib/Release/Win32)
	endif()
endif()

add_subdirectory(sources)

