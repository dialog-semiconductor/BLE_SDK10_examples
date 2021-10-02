cmake_minimum_required(VERSION 3.16)

# project definitions
project(pxp_reporter C CXX ASM)
set(BLE ON)
set(DEVICE DA14699_00)
set(PRE_BUILD_EXTRA_DEFS "")
set(SDK_CONFIG_H ${CMAKE_CURRENT_SOURCE_DIR}/config/sdk_config.h)
# provide path to the your SDK/sdk location
# in my case root of SDK is named as BSP
set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/sdk)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/system.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ldscript.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/sdk.cmake)

generate_ldscripts()

add_definitions(-Ddg_configDEVICE=${DEVICE})
add_definitions(-DAPP_CONFIG_H=${SDK_CONFIG_H})
add_definitions(-include${SDK_CONFIG_H})

include_directories(.)
include_directories(config)
include_directories(${SDK_INCLUDES})

set(PROJECT_SRCS 
    main.c
    platform_devices.c 
    pxp_reporter_task.c
)

set(SOURCE_FILES ${SDK_SRCS} ${PROJECT_SRCS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(BLE)
find_library(BLE_LIB ble_stack_da1469x ${LIB_BLE_PATH})
target_link_libraries(${PROJECT_NAME} ${BLE_LIB})
message("BLE library location: " ${BLE_LIB})
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${SIZE} ARGS --format=berkeley ${PROJECT_NAME}.elf)