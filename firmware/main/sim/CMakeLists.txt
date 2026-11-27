# InhabitX main UI, PC simulator (Linux, X11).
# Builds the same ui/ and logic/ sources that go onto the E84.
cmake_minimum_required(VERSION 3.16)
project(inhabitx_main_sim C)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE STRING "" FORCE)
FetchContent_Declare(lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG        v9.2.2
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(lvgl)

# MQTT client: coreMQTT (MIT), the library inside Infineon's ModusToolbox
# "mqtt" middleware, so the E84 port uses the same calls. 2.x = MQTT 3.1.1,
# the version that middleware is built on.
FetchContent_Declare(coremqtt
    GIT_REPOSITORY https://github.com/FreeRTOS/coreMQTT.git
    GIT_TAG        v2.3.1
    GIT_SHALLOW    TRUE)
FetchContent_GetProperties(coremqtt)
if(NOT coremqtt_POPULATED)
    FetchContent_Populate(coremqtt)
endif()

find_package(X11 REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB UI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.c)
add_executable(main_sim
    main.c
    mqtt_link.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../logic/ix_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../logic/ix_msg.c
    ${coremqtt_SOURCE_DIR}/source/core_mqtt.c
    ${coremqtt_SOURCE_DIR}/source/core_mqtt_serializer.c
    ${coremqtt_SOURCE_DIR}/source/core_mqtt_state.c
    ${UI_SRC})
set(UI_HOR_RES 1024 CACHE STRING "screen width  (1024 = 7\"/10.1\", 800 = kit 4.3\")")
set(UI_VER_RES 600  CACHE STRING "screen height (600 or 480)")
set(GHOST_RELEASE_S 30 CACHE STRING "release a no-show booking after this many seconds (demo 30, real 600)")
target_compile_definitions(main_sim PRIVATE UI_HOR_RES=${UI_HOR_RES} UI_VER_RES=${UI_VER_RES}
    IX_GHOST_RELEASE_S=${GHOST_RELEASE_S}
    IX_SIM_DIR="${CMAKE_CURRENT_SOURCE_DIR}" MQTT_DO_NOT_USE_CUSTOM_CONFIG)
target_include_directories(main_sim PRIVATE ../logic ../ui
    ${coremqtt_SOURCE_DIR}/source/include ${coremqtt_SOURCE_DIR}/source/interface)
target_compile_options(main_sim PRIVATE -Wall -Wextra -Wno-unused-parameter)
target_link_libraries(main_sim PRIVATE lvgl X11::X11 OpenSSL::SSL m pthread)
