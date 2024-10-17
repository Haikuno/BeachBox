cmake_minimum_required(VERSION 3.13)

set(KOS_BASE /opt/toolchains/dc/kos)
set(KOS_CC_BASE /opt/toolchains/dc/sh-elf)
set(KOS_SUBARCH pristine)
set(KOS_PORTS ${KOS_BASE}/../kos-ports)
include_directories(${KOS_PORTS}/include/raylib)

include(${KOS_BASE}/utils/cmake/dreamcast.toolchain.cmake)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BeachBox C)

set(CMAKE_C_STANDARD 23)

SET (SRCS main.c
    src/scenes/credits.c
    src/scenes/game.c
    src/scenes/loading.c
    src/scenes/mainmenu.c
    src/scenes/options.c
    src/scenes/raylogo.c
    src/scenes/shop.c
    src/scenes/unlockables.c
    src/audio.c
    src/background.c
    src/controller.c
    src/hats.c
    src/objects.c
    src/player.c
    src/save.c
    src/ui.c
    src/scene.c
    src/vmu.c
)

add_executable(BeachBox ${SRCS})
target_link_libraries(BeachBox raylib GL m z ADX)

kos_add_romdisk(BeachBox romdisk)
