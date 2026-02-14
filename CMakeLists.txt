cmake_minimum_required(VERSION 3.28.1)
project(Desktop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/Dependencies/ImGui)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/UI/*.h"
        "${CMAKE_SOURCE_DIR}/Core/*.h"
        "${CMAKE_SOURCE_DIR}/Utils/*.h"
        "${CMAKE_SOURCE_DIR}/Audio/*.h"
        "${CMAKE_SOURCE_DIR}/FileSystem/*.h"
        "${CMAKE_SOURCE_DIR}/Math/*.h"
        "${CMAKE_SOURCE_DIR}/Scripting/*.h"
        "${CMAKE_SOURCE_DIR}/FS/*.h"
        "${CMAKE_SOURCE_DIR}/AUDIO/*.h"
        "${CMAKE_SOURCE_DIR}/MATH/*.h"
        "${CMAKE_SOURCE_DIR}/SCR/*.h"
)

file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/UI/*.cpp"
        "${CMAKE_SOURCE_DIR}/Core/*.cpp"
        "${CMAKE_SOURCE_DIR}/Utils/*.cpp"
        "${CMAKE_SOURCE_DIR}/Audio/*.cpp"
        "${CMAKE_SOURCE_DIR}/FileSystem/*.cpp"
        "${CMAKE_SOURCE_DIR}/Math/*.cpp"
        "${CMAKE_SOURCE_DIR}/Scripting/*.cpp"
        "${CMAKE_SOURCE_DIR}/FS/*.cpp"
        "${CMAKE_SOURCE_DIR}/AUDIO/*.cpp"
        "${CMAKE_SOURCE_DIR}/MATH/*.cpp"
        "${CMAKE_SOURCE_DIR}/SCR/*.cpp"
)

list(FILTER CPP_FILES EXCLUDE REGEX "buildtrees/")
list(FILTER CPP_FILES EXCLUDE REGEX "test_ports/")
list(FILTER CPP_FILES EXCLUDE REGEX ".*/test.cpp$")
list(FILTER CPP_FILES EXCLUDE REGEX ".*/example.cpp$")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-unused-command-line-argument)
endif()

add_executable(Desktop WIN32
        main.cpp
        ${IMGUI_SOURCES}
        ${CPP_FILES}
)

target_include_directories(Desktop PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Desktop PRIVATE Dependencies/fmt)
target_include_directories(Desktop PRIVATE Dependencies/tfd)
target_compile_definitions(Desktop PUBLIC FMT_HEADER_ONLY)

find_package(CURL REQUIRED)
target_link_libraries(Desktop PRIVATE CURL::libcurl)

if(EXISTS "${CMAKE_SOURCE_DIR}/Dependencies/tfd/tinyfiledialogs.cpp")
    target_sources(Desktop PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/tfd/tinyfiledialogs.cpp")
    target_include_directories(Desktop PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/tfd")

    if(WIN32)
        target_link_libraries(Desktop PRIVATE comdlg32 user32 shell32)
    endif()

    message(STATUS "Using tinyfiledialogs from Dependencies/tfd/")
else()
    message(FATAL_ERROR "tinyfiledialogs not found in Dependencies/tfd/")
endif()

file(GLOB QUICKJS_SOURCES "${CMAKE_SOURCE_DIR}/Dependencies/quickjs/*.c")

list(FILTER QUICKJS_SOURCES EXCLUDE REGEX ".*/qjsc.c$")
list(FILTER QUICKJS_SOURCES EXCLUDE REGEX ".*/run%-test262.c$")
list(FILTER QUICKJS_SOURCES EXCLUDE REGEX ".*/quickjs%-libc.c$")

add_library(quickjs STATIC ${QUICKJS_SOURCES})
target_include_directories(quickjs PUBLIC ${CMAKE_SOURCE_DIR}/Dependencies/quickjs)

file(READ ${CMAKE_SOURCE_DIR}/Dependencies/quickjs/VERSION QUICKJS_VERSION)
string(STRIP "${QUICKJS_VERSION}" QUICKJS_VERSION)
target_compile_definitions(quickjs PRIVATE CONFIG_VERSION=\"${QUICKJS_VERSION}\")
target_compile_definitions(quickjs PRIVATE CONFIG_BIGNUM)
target_compile_definitions(quickjs PRIVATE _GNU_SOURCE)

target_link_libraries(Desktop PRIVATE quickjs)

if(UNIX)
    target_link_libraries(Desktop PRIVATE m)
endif()

if(UNIX AND NOT APPLE)
    add_definitions(-D__cdecl=)
endif()

add_subdirectory(Dependencies/glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(Desktop PRIVATE glfw OpenGL::GL)

target_include_directories(Desktop PRIVATE ${IMGUI_DIR})
