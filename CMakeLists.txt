cmake_minimum_required(VERSION 3.14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_HOME_DIRECTORY}/bin>)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY $<1:${CMAKE_HOME_DIRECTORY}/bin>)
add_compile_definitions($<$<CONFIG:Debug,RelWithDebInfo>:HM_DEBUG>)

add_compile_options(-ZI -GR- -Oi -W4 -WX -wd4201 -wd4100 -wd4189 -wd4505)


project(HandmadeHero)

file(GLOB SOURCES CONFIGURE_DEPENDS src/game/*.cpp)
add_library(${PROJECT_NAME}Lib SHARED ${SOURCES})

if (WIN32)
	file(GLOB SOURCES CONFIGURE_DEPENDS src/windows/*.cpp)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
	add_definitions(-DHM_WIN32)
elseif (LINUX)
	file(GLOB SOURCES CONFIGURE_DEPENDS src/linux/*.cpp)
	add_executable(${PROJECT_NAME} LINUX ${SOURCES})
	add_definitions(-DHM_LINUX)
endif()

target_link_options(${PROJECT_NAME}Lib PUBLIC LINKER:-MAP=${PROJECT_NAME}Lib.map)
target_link_options(${PROJECT_NAME} PUBLIC LINKER:-MAP=${PROJECT_NAME}.map)

include_directories(src src/game)
target_link_libraries(${PROJECT_NAME} PRIVATE Winmm.lib)

add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory 
${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
add_dependencies(${PROJECT_NAME} copy_assets)
add_dependencies(${PROJECT_NAME}Lib copy_assets)
