cmake_minimum_required(VERSION 3.20)
project(mmo-game C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_C_FLAGS_RELEASE "-O2")

find_package(SDL2 REQUIRED)

add_executable(mmo-game
    src/main.c
    src/map.c
    src/noise.c
    src/world_gen.c
    src/chunk_renderer.c
    src/chunk_manager.c
    src/camera.c
    src/player.c
    src/sprite_motor.c
)

target_include_directories(mmo-game PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(mmo-game PRIVATE ${SDL2_LIBRARIES} m)
