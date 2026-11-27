# InhabitX master UI, PC simulator (Linux, X11).
# Builds the same ui/ and logic/ sources that go onto the E84.
cmake_minimum_required(VERSION 3.16)
project(inhabitx_master_sim C)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE STRING "" FORCE)
FetchContent_Declare(lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG        v9.2.2
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(lvgl)

find_package(X11 REQUIRED)

file(GLOB UI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.c)
add_executable(master_sim
    main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../logic/ix_model.c
    ${UI_SRC})
set(UI_HOR_RES 1024 CACHE STRING "screen width  (1024 = 7\"/10.1\", 800 = kit 4.3\")")
set(UI_VER_RES 600  CACHE STRING "screen height (600 or 480)")
target_compile_definitions(master_sim PRIVATE UI_HOR_RES=${UI_HOR_RES} UI_VER_RES=${UI_VER_RES})
target_include_directories(master_sim PRIVATE ../logic ../ui)
target_compile_options(master_sim PRIVATE -Wall -Wextra -Wno-unused-parameter)
target_link_libraries(master_sim PRIVATE lvgl X11::X11 m pthread)
