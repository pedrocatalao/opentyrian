cmake_minimum_required(VERSION 3.20)
project(opentyrian C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# The Makefile remains the primary standalone build.  This file exists for
# the DOS ex Machina core: OT_CORE=ON builds the game as a dependency-free
# static library, and OT_MODULE=ON additionally as the loadable .dxm module
# the shell downloads and opens.  It also builds the standalone game, for a
# single configure on platforms where CMake is what people have.
option(OT_CORE "Build the DXM core instead of the standalone game" OFF)
option(OT_MODULE "With OT_CORE: also build the loadable opentyrian.dxm module" OFF)
option(OT_NETWORK "Standalone: build with SDL2_net network play (auto if found)" ON)

execute_process(COMMAND git describe --tags WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE OT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
if(NOT OT_VERSION)
    execute_process(COMMAND git rev-parse --short HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE OT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
endif()

set(OT_WARNINGS -pedantic -Wall -Wextra -Wno-missing-field-initializers
    $<$<C_COMPILER_ID:GNU>:-Wno-format-truncation>)

# Everything that is the game.  No SDL, no window, no audio device, no
# joystick: those are behind platform.h and come from the files below.
file(GLOB OT_ALL_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.c)
set(OT_PLATFORM_SDL_SRC
    ${CMAKE_SOURCE_DIR}/src/platform_sdl.c
    ${CMAKE_SOURCE_DIR}/src/video_sdl.c
    ${CMAKE_SOURCE_DIR}/src/video_scale.c
    ${CMAKE_SOURCE_DIR}/src/video_scale_hqNx.c
    ${CMAKE_SOURCE_DIR}/src/joystick.c
    ${CMAKE_SOURCE_DIR}/src/main.c)
set(OT_GAME_SRC ${OT_ALL_SRC})
list(REMOVE_ITEM OT_GAME_SRC ${OT_PLATFORM_SDL_SRC})

if(WIN32)
    set(OT_TARGET_DEF TARGET_WIN32)
else()
    set(OT_TARGET_DEF TARGET_UNIX)
endif()

if(OT_CORE)
    # src/dxm/dxm_core.h is VENDORED: the core has to build with DXM checked
    # out nowhere.  DXM's own build may pass OT_DXM_INCLUDE to put its copy
    # first; DXM_ABI in the header is what catches the two drifting apart.
    set(OT_DXM_INCLUDE "" CACHE PATH "DXM's contract/ directory, if building against a checkout")
    set(OT_CORE_SRC ${OT_GAME_SRC}
        ${CMAKE_SOURCE_DIR}/src/dxm/platform_dxm.c
        ${CMAKE_SOURCE_DIR}/src/dxm/video_stub.c
        ${CMAKE_SOURCE_DIR}/src/dxm/joystick_stub.c
        ${CMAKE_SOURCE_DIR}/src/dxm/dxm_entry.c)

    function(ot_core_target name)
        target_include_directories(${name} PRIVATE ${OT_DXM_INCLUDE} src src/dxm)
        target_compile_definitions(${name} PRIVATE OT_CORE=1 NDEBUG ${OT_TARGET_DEF}
            OPENTYRIAN_VERSION="${OT_VERSION}")
        target_compile_options(${name} PRIVATE ${OT_WARNINGS})
        target_link_libraries(${name} PRIVATE m)
    endfunction()

    add_library(opentyrian_core STATIC ${OT_CORE_SRC})
    ot_core_target(opentyrian_core)

    if(OT_MODULE)
        add_library(opentyrian_module MODULE ${OT_CORE_SRC})
        ot_core_target(opentyrian_module)
        set_target_properties(opentyrian_module PROPERTIES
            OUTPUT_NAME "opentyrian" PREFIX "" SUFFIX ".dxm"
            C_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON)
        # An unresolved symbol must fail the build, not the dlopen in front of
        # the user.  Apple's linker already errors by default.
        if(WIN32)
            target_link_options(opentyrian_module PRIVATE -Wl,--exclude-all-symbols)
        elseif(NOT APPLE)
            target_link_options(opentyrian_module PRIVATE -Wl,--no-undefined)
        endif()
    endif()
    return()
endif()

# ---- standalone -----------------------------------------------------------
find_package(SDL2 REQUIRED CONFIG)
find_package(PkgConfig QUIET)
set(OT_HAVE_NET OFF)
if(OT_NETWORK AND PkgConfig_FOUND)
    pkg_check_modules(SDL2_NET IMPORTED_TARGET SDL2_net)
    if(SDL2_NET_FOUND)
        set(OT_HAVE_NET ON)
    endif()
endif()

add_executable(opentyrian ${OT_GAME_SRC} ${OT_PLATFORM_SDL_SRC})
target_include_directories(opentyrian PRIVATE src)
target_compile_definitions(opentyrian PRIVATE NDEBUG ${OT_TARGET_DEF}
    OPENTYRIAN_VERSION="${OT_VERSION}"
    TYRIAN_DIR="${CMAKE_INSTALL_PREFIX}/share/games/tyrian")
target_compile_options(opentyrian PRIVATE ${OT_WARNINGS})
target_link_libraries(opentyrian PRIVATE SDL2::SDL2 m)
if(OT_HAVE_NET)
    target_compile_definitions(opentyrian PRIVATE WITH_NETWORK)
    target_link_libraries(opentyrian PRIVATE PkgConfig::SDL2_NET)
    message(STATUS "Network play: on")
else()
    message(STATUS "Network play: off (SDL2_net not found)")
endif()
