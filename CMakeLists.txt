cmake_minimum_required(VERSION 3.20)
project(mmo-game C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_C_FLAGS_RELEASE "-O2")

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

add_executable(mmo-game
    src/core/main.c
    src/world/noise.c
    src/world/world_gen.c
    src/world/tile_sprites.c
    src/world/object_renderer.c
    src/world/chunk_renderer.c
    src/world/chunk_manager.c
    src/camera/camera.c
    src/entity/player.c
    src/entity/sprite_motor.c
    src/props/deco.c
)

target_include_directories(mmo-game PRIVATE src ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(mmo-game PRIVATE ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m)

# Symlink assets into the build directory so the binary finds them
# regardless of where it is launched from.
add_custom_command(TARGET mmo-game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:mmo-game>/assets
    COMMENT "Symlinking assets into build directory")
