string(TOUPPER -D${CMAKE_BUILD_TYPE}_BUILD LD_DEFS)

function(generate_ldscripts)
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -I ${BSP_CONFIG_DIR} -I ${MIDDLEWARE_CONFIG_DIR} 
    ${PRE_BUILD_EXTRA_DEFS} -imacros ${SDK_CONFIG_H} ${LD_DEFS} -Ddg_configDEVICE=${DEVICE} 
    -E -P -c ${LDSCRIPT_PATH}/mem.ld.h -o ${CMAKE_CURRENT_BINARY_DIR}/mem.ld
)  
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -I ${BSP_CONFIG_DIR} -I ${MIDDLEWARE_CONFIG_DIR} 
    ${PRE_BUILD_EXTRA_DEFS} -imacros ${SDK_CONFIG_H} ${LD_DEFS} -Ddg_configDEVICE=${DEVICE} 
    -E -P -c ${LDSCRIPT_PATH}/sections.ld.h -o ${CMAKE_CURRENT_BINARY_DIR}/sections.ld
)  
endfunction(generate_ldscripts)