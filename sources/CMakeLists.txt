file(GLOB_RECURSE SRCS *.cpp *.hpp *.c *.h)

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
source_group(main FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/core/*.*)
source_group(core FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/core/drivers/*.*)
source_group(core\\drivers FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/sections/*.*)
source_group(sections FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/effects/*.*)
source_group(effects FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/expreval/*.*)
source_group(expreval FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/interface/*.*)
source_group(interface FILES ${SVE_SRCS})

file(GLOB SVE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/net2/*.*)
source_group(net2 FILES ${SVE_SRCS})


add_executable(sve WIN32 ${SRCS})

configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/fmod10903/lowlevel/lib/fmod64.dll       ${CMAKE_CURRENT_BINARY_DIR}/debug/fmod64.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2-2.0.5/lib/x64/SDL2.dll             ${CMAKE_CURRENT_BINARY_DIR}/debug/SDL2.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_image-2.0.1/lib/x64/SDL2_Image.dll ${CMAKE_CURRENT_BINARY_DIR}/debug/SDL2_Image.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_image-2.0.1/lib/x64/libjpeg-9.dll  ${CMAKE_CURRENT_BINARY_DIR}/debug/libjpeg-9.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_net-2.0.1/lib/x64/SDL2_Net.dll     ${CMAKE_CURRENT_BINARY_DIR}/debug/SDL2_Net.dll COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/fmod10903/lowlevel/lib/fmod64.dll       ${CMAKE_CURRENT_BINARY_DIR}/release/fmod64.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2-2.0.5/lib/x64/SDL2.dll             ${CMAKE_CURRENT_BINARY_DIR}/release/SDL2.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_image-2.0.1/lib/x64/SDL2_Image.dll ${CMAKE_CURRENT_BINARY_DIR}/release/SDL2_Image.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dependencies/vc141/SDL2_net-2.0.1/lib/x64/SDL2_Net.dll     ${CMAKE_CURRENT_BINARY_DIR}/release/SDL2_Net.dll COPYONLY)

set_target_properties(sve PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

