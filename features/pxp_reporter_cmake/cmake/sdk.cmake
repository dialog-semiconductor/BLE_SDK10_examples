set(MIDDLEWARE_PATH ${SDK_PATH}/middleware)
set(MIDDLEWARE_ADAPTERS_PATH ${MIDDLEWARE_PATH}/adapters)
set(MIDDLEWARE_CLI_PATH ${MIDDLEWARE_PATH}/cli)
set(MIDDLEWARE_CONFIG_PATH ${MIDDLEWARE_PATH}/config)
set(MIDDLEWARE_CONSOLE_PATH ${MIDDLEWARE_PATH}/console)
set(MIDDLEWARE_DGTL_PATH ${MIDDLEWARE_PATH}/dgtl)
set(MIDDLEWARE_HAPTICS_PATH ${MIDDLEWARE_PATH}/haptics)
set(MIDDLEWARE_HW_TOOLS_PATH ${MIDDLEWARE_PATH}/hw_tools)
set(MIDDLEWARE_LOGGING_PATH ${MIDDLEWARE_PATH}/logging)
set(MIDDLEWARE_MCIF_PATH ${MIDDLEWARE_PATH}/mcif)
set(MIDDLEWARE_MONITORING_PATH ${MIDDLEWARE_PATH}/monitoring)
set(MIDDLEWARE_OSAL_PATH ${MIDDLEWARE_PATH}/osal)
set(MIDDLEWARE_SEGGER_TOOLS_PATH ${MIDDLEWARE_PATH}/segger_tools)

set(MIDDLEWARE_ADAPTERS_SRCS 
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_crypto.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_flash.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_gpadc.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_haptic.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_i2c.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_iso7816.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_keyboard_scanner.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_lcdc.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_nvms.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_nvms_direct.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_nvms_ves.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_nvparam.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_pmu.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_sdadc.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_snc.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_spi.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_template.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/ad_uart.c
    ${MIDDLEWARE_ADAPTERS_PATH}/src/sys_platform_devices_internal.c
)
set(MIDDLEWARE_ADAPTERS_INCLUDES
    ${MIDDLEWARE_ADAPTERS_PATH}/include
    ${MIDDLEWARE_ADAPTERS_PATH}/src
)

set(MILLDEWARE_CLI_SRCS 
    ${MIDDLEWARE_CLI_PATH}/src/cli.c
)
set(MIDDLEWARE_CLI_INCLUDES 
    ${MIDDLEWARE_CLI_PATH}/include
) 

set(MIDDLEWARE_CONFIG_INCLUDE_FILES 
    ${MIDDLEWARE_CONFIG_PATH}/middleware_defaults.h
)
set(MIDDLEWARE_CONFIG_INCLUDES 
    ${MIDDLEWARE_CONFIG_PATH}
)

set(MIDDLEWARE_CONSOLE_SRCS 
    ${MIDDLEWARE_CONSOLE_PATH}/src/console.c
)
set(MIDDLEWARE_CONSOLE_INCLUDES 
    ${MIDDLEWARE_CONSOLE_PATH}/include
)

set(MIDDLEWARE_DGTL_SRCS ${MIDDLEWARE_DGTL_PATH}/src/dgtl.c
    ${MIDDLEWARE_DGTL_PATH}/src/dgtl_msg.c)
set(MIDDLEWARE_DGTL_INCLUDES 
    ${MIDDLEWARE_DGTL_PATH}/include
)

set(MIDDLEWARE_HAPTICS_SRCS 
    ${MIDDLEWARE_HAPTICS_PATH}/src/wm_decoder.c
)
set(MIDDLEWARE_HAPTICS_INCLUDES 
    ${MIDDLEWARE_HAPTICS_PATH}/include
)

set(MIDDLEWARE_HW_TOOLS_SRCS 
    ${MIDDLEWARE_HW_TOOLS_PATH}/src/Xtal_TRIM_da1469x.c
    ${MIDDLEWARE_HW_TOOLS_PATH}/src/PULSE.S
)
set(MIDDLEWARE_HW_TOOLS_INCLUDES 
    ${MIDDLEWARE_HW_TOOLS_PATH}/include
)

set(MIDDLEWARE_LOGGING_SRCS ${MIDDLEWARE_LOGGING_PATH}/src/logging.c)
set(MIDDLEWARE_LOGGING_INCLUDES ${MIDDLEWARE_LOGGING_PATH}/include)

set(MIDDLEWARE_MCIF_SRCS 
    ${MIDDLEWARE_MCIF_PATH}/src/mcif.c
    ${MIDDLEWARE_MCIF_PATH}/src/mcif_ascii.c
)
set(MIDDLEWARE_MCIF_INCLUDES 
    ${MIDDLEWARE_MCIF_PATH}/src
    ${MIDDLEWARE_MCIF_PATH}/include
)

set(MIDDLEWARE_MONITORING_SRCS 
    ${MIDDLEWARE_MONITORING_PATH}/task_monitoring.c
)
set(MIDDLEWARE_MONITORING_INCLUDES 
    ${MIDDLEWARE_MONITORING_PATH}
)

set(MIDDLEWARE_OSAL_SRCS 
    ${MIDDLEWARE_OSAL_PATH}/msg_queues.c
    ${MIDDLEWARE_OSAL_PATH}/resmgmt.c
    ${MIDDLEWARE_OSAL_PATH}/usb_osal_wrapper.c
)
set(MIDDLEWARE_OSAL_INCLUDES 
    ${MIDDLEWARE_OSAL_PATH}
)

set(MIDDLEWARE_SEGGER_TOOLS_SRCS 
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/Config/SEGGER_SYSVIEW_Config_FreeRTOS_da1469x.c
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/OS/SEGGER_SYSVIEW_FreeRTOS.c
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/SEGGER/SEGGER_RTT.c
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/SEGGER/SEGGER_RTT_printf.c
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/SEGGER/SEGGER_SYSVIEW.c
)
set(MIDDLEWARE_SEGGER_TOOLS_INCLUDES
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/Config
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/OS
    ${MIDDLEWARE_SEGGER_TOOLS_PATH}/SEGGER
)

set(MIDDLEWARE_SRCS
    ${MIDDLEWARE_ADAPTERS_SRCS}
    ${MILLDEWARE_CLI_SRCS}
    ${MIDDLEWARE_CONSOLE_SRCS}
    # ${MIDDLEWARE_DGTL_SRCS}
    ${MIDDLEWARE_HAPTICS_SRCS}
    ${MIDDLEWARE_HW_TOOLS_SRCS}
    ${MIDDLEWARE_LOGGING_SRCS}
    ${MIDDLEWARE_MCIF_SRCS}
    ${MIDDLEWARE_MONITORING_SRCS}
    ${MIDDLEWARE_OSAL_SRCS}
    ${MIDDLEWARE_SEGGER_TOOLS_SRCS}
)
set(MIDDLEWARE_INCLUDES 
    ${MIDDLEWARE_ADAPTERS_INCLUDES}
    ${MIDDLEWARE_CLI_INCLUDES}
    ${MIDDLEWARE_CONFIG_INCLUDES}
    ${MIDDLEWARE_CONSOLE_INCLUDES}
    ${MIDDLEWARE_DGTL_INCLUDES}
    ${MIDDLEWARE_HAPTICS_INCLUDES}
    ${MIDDLEWARE_HW_TOOLS_INCLUDES}
    ${MIDDLEWARE_LOGGING_INCLUDES}
    ${MIDDLEWARE_MCIF_INCLUDES}
    ${MIDDLEWARE_MONITORING_INCLUDES}
    ${MIDDLEWARE_OSAL_INCLUDES}
    ${MIDDLEWARE_SEGGER_TOOLS_INCLUDES}
)

set(INTERFACES_PATH ${SDK_PATH}/interfaces)
set(INTERFACES_BLE_PATH ${INTERFACES_PATH}/ble)
set(INTERFACES_CRYPTO_PATH ${INTERFACES_PATH}/crypto)
set(INTERFACES_USB_PATH ${INTERFACES_PATH}/usb)

set(INTERFACES_BLE_SRCS
    ${INTERFACES_BLE_PATH}/adapter/src/ad_ble.c
    ${INTERFACES_BLE_PATH}/api/src/ble_attribdb.c
    ${INTERFACES_BLE_PATH}/api/src/ble_common.c
    ${INTERFACES_BLE_PATH}/api/src/ble_gap.c
    ${INTERFACES_BLE_PATH}/api/src/ble_gattc.c
    ${INTERFACES_BLE_PATH}/api/src/ble_gattc_util.c
    ${INTERFACES_BLE_PATH}/api/src/ble_gatts.c
    ${INTERFACES_BLE_PATH}/api/src/ble_l2cap.c
    ${INTERFACES_BLE_PATH}/api/src/ble_storage.c
    ${INTERFACES_BLE_PATH}/api/src/ble_uuid.c
    ${INTERFACES_BLE_PATH}/clients/src/bas_client.c
    ${INTERFACES_BLE_PATH}/clients/src/ble_client.c
    ${INTERFACES_BLE_PATH}/clients/src/cscs_client.c
    ${INTERFACES_BLE_PATH}/clients/src/dis_client.c
    ${INTERFACES_BLE_PATH}/clients/src/gatt_client.c
    ${INTERFACES_BLE_PATH}/clients/src/hids_client.c
    ${INTERFACES_BLE_PATH}/clients/src/hrs_client.c
    ${INTERFACES_BLE_PATH}/clients/src/scps_client.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_ad_msg.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_cmd.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_common.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_gap.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_gattc.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_gatts.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_gtl.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_helper.c
    ${INTERFACES_BLE_PATH}/manager/src/ble_mgr_l2cap.c
    ${INTERFACES_BLE_PATH}/manager/src/storage.c
    ${INTERFACES_BLE_PATH}/manager/src/storage_flash.c
    ${INTERFACES_BLE_PATH}/services/src/bas.c
    ${INTERFACES_BLE_PATH}/services/src/bcs.c
    ${INTERFACES_BLE_PATH}/services/src/ble_service.c
    ${INTERFACES_BLE_PATH}/services/src/bls.c
    ${INTERFACES_BLE_PATH}/services/src/bms.c
    ${INTERFACES_BLE_PATH}/services/src/cts.c
    ${INTERFACES_BLE_PATH}/services/src/dis.c
    ${INTERFACES_BLE_PATH}/services/src/dlg_debug.c
    ${INTERFACES_BLE_PATH}/services/src/dlg_suota.c
    ${INTERFACES_BLE_PATH}/services/src/hids.c
    ${INTERFACES_BLE_PATH}/services/src/hrs.c
    ${INTERFACES_BLE_PATH}/services/src/ias.c
    ${INTERFACES_BLE_PATH}/services/src/lls.c
    ${INTERFACES_BLE_PATH}/services/src/scps.c
    ${INTERFACES_BLE_PATH}/services/src/sps.c
    ${INTERFACES_BLE_PATH}/services/src/tps.c
    ${INTERFACES_BLE_PATH}/services/src/uds.c
    ${INTERFACES_BLE_PATH}/services/src/wss.c
    ${INTERFACES_BLE_PATH}/stack/da14690/src/arch_main.c
    ${INTERFACES_BLE_PATH}/stack/da14690/src/jump_table.c
)
set(INTERFACES_BLE_INCLUDES 
    ${INTERFACES_BLE_PATH}/adapter/include
    ${INTERFACES_BLE_PATH}/api/include
    ${INTERFACES_BLE_PATH}/clients/include
    ${INTERFACES_BLE_PATH}/config
    ${INTERFACES_BLE_PATH}/manager/include
    ${INTERFACES_BLE_PATH}/services/include
    ${INTERFACES_BLE_PATH}/stack/config
    ${INTERFACES_BLE_PATH}/stack/da14690/include
)

set(INTERFACES_CRYPTO_SRCS
    ${INTERFACES_CRYPTO_PATH}/src/crypto_ec.c
    ${INTERFACES_CRYPTO_PATH}/src/crypto_ecc_provider_functions.c
    ${INTERFACES_CRYPTO_PATH}/src/crypto_ecc_provider_params.c
    ${INTERFACES_CRYPTO_PATH}/src/crypto_ecdh.c
    ${INTERFACES_CRYPTO_PATH}/src/crypto_hmac.c
)
set(INTERFACES_CRYPTO_INCLUDES 
    ${INTERFACES_CRYPTO_PATH}/include
)

set(INTERFACES_USB_INCLUDES ${INTERFACES_USB_PATH}/include)

set(INTERFACES_SRCS 
    ${INTERFACES_BLE_SRCS}
    # ${INTERFACES_CRYPTO_SRCS}
)
set(INTERFACES_INCLUDES
    ${INTERFACES_BLE_INCLUDES}
    # ${INTERFACES_CRYPTO_INCLUDES}
    # ${INTERFACES_USB_INCLUDES}
)

set(FREERTOS_PATH ${SDK_PATH}/free_rtos)

set(FREERTOS_SRCS
    ${FREERTOS_PATH}/croutine.c
    ${FREERTOS_PATH}/event_groups.c
    ${FREERTOS_PATH}/list.c
    ${FREERTOS_PATH}/queue.c
    ${FREERTOS_PATH}/stream_buffer.c
    ${FREERTOS_PATH}/tasks.c
    ${FREERTOS_PATH}/timers.c
    ${FREERTOS_PATH}/portable/Common/mpu_wrappers.c
    ${FREERTOS_PATH}/portable/GCC/DA1469x/port.c
    ${FREERTOS_PATH}/portable/MemMang/heap_4.c
)
set(FREERTOS_INCLUDES ${FREERTOS_PATH}/include
    ${FREERTOS_PATH}/portable/GCC/DA1469x
)

set(BSP_PATH ${SDK_PATH}/bsp)
set(BSP_SRCS
    ${BSP_PATH}/startup/config.c
    ${BSP_PATH}/startup/DA1469x/GCC/deepsleep.S 
    ${BSP_PATH}/startup/DA1469x/GCC/startup_da1469x.S 
    ${BSP_PATH}/startup/DA1469x/GCC/exception_handlers.S 
    ${BSP_PATH}/startup/DA1469x/GCC/vector_table_da1469x.S 
    ${BSP_PATH}/startup/DA1469x/init_da1469x.c
    ${BSP_PATH}/startup/DA1469x/system_da1469x.c
    ${BSP_PATH}/memory/src/qspi_automode.c
    ${BSP_PATH}/peripherals/src/hw_aes_hash.c
    ${BSP_PATH}/peripherals/src/hw_apu_src.c
    ${BSP_PATH}/peripherals/src/hw_bod_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_cache.c
    ${BSP_PATH}/peripherals/src/hw_charger_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_clk_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_cpm_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_crypto.c
    ${BSP_PATH}/peripherals/src/hw_dma.c
    ${BSP_PATH}/peripherals/src/hw_gpadc.c
    ${BSP_PATH}/peripherals/src/hw_gpio.c
    ${BSP_PATH}/peripherals/src/hw_haptic.c
    ${BSP_PATH}/peripherals/src/hw_hard_fault.c
    ${BSP_PATH}/peripherals/src/hw_i2c.c
    ${BSP_PATH}/peripherals/src/hw_iso7816.c
    ${BSP_PATH}/peripherals/src/hw_lcdc.c
    ${BSP_PATH}/peripherals/src/hw_memctrl.c
    ${BSP_PATH}/peripherals/src/hw_otpc_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_pcm.c
    ${BSP_PATH}/peripherals/src/hw_pdc.c
    ${BSP_PATH}/peripherals/src/hw_pdm.c
    ${BSP_PATH}/peripherals/src/hw_pmu_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_qspi.c
    ${BSP_PATH}/peripherals/src/hw_rtc.c
    ${BSP_PATH}/peripherals/src/hw_sdadc.c
    ${BSP_PATH}/peripherals/src/hw_smotor.c
    ${BSP_PATH}/peripherals/src/hw_snc.c
    ${BSP_PATH}/peripherals/src/hw_spi.c
    ${BSP_PATH}/peripherals/src/hw_sys_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_timer.c
    ${BSP_PATH}/peripherals/src/hw_trng.c
    ${BSP_PATH}/peripherals/src/hw_uart.c
    ${BSP_PATH}/peripherals/src/hw_usb_da1469x.c
    ${BSP_PATH}/peripherals/src/hw_watchdog.c
    ${BSP_PATH}/peripherals/src/hw_wkup_da1469x.c
    ${BSP_PATH}/snc/src/SeNIS.c
    ${BSP_PATH}/snc/src/snc_debug.c
    ${BSP_PATH}/snc/src/snc_defs.c
    ${BSP_PATH}/snc/src/snc_emu.c
    ${BSP_PATH}/snc/src/snc_hw_gpadc.c
    ${BSP_PATH}/snc/src/snc_hw_gpio.c
    ${BSP_PATH}/snc/src/snc_hw_i2c.c
    ${BSP_PATH}/snc/src/snc_hw_spi.c
    ${BSP_PATH}/snc/src/snc_hw_sys.c
    ${BSP_PATH}/snc/src/snc_hw_uart.c
    ${BSP_PATH}/snc/src/snc_main.c
    ${BSP_PATH}/snc/src/snc_queues.c
    ${BSP_PATH}/system/sys_man/sys_adc.c
    ${BSP_PATH}/system/sys_man/sys_audio_mgr.c
    ${BSP_PATH}/system/sys_man/sys_bsr.c
    ${BSP_PATH}/system/sys_man/sys_charger_da1469x.c
    ${BSP_PATH}/system/sys_man/sys_clock_mgr_da1469x.c
    ${BSP_PATH}/system/sys_man/sys_power_mgr_da1469x.c
    ${BSP_PATH}/system/sys_man/sys_rcx_calibrate.c
    ${BSP_PATH}/system/sys_man/sys_tcs_da1469x.c
    ${BSP_PATH}/system/sys_man/sys_timer.c
    ${BSP_PATH}/system/sys_man/sys_trng.c
    ${BSP_PATH}/system/sys_man/sys_usb_da1469x.c
    ${BSP_PATH}/system/sys_man/sys_watchdog.c
    ${BSP_PATH}/util/src/sdk_crc16.c
    ${BSP_PATH}/util/src/sdk_list.c
    ${BSP_PATH}/util/src/sdk_queue.c
)
set(BSP_INCLUDES
    ${BSP_PATH}/config
    ${BSP_PATH}/include
    ${BSP_PATH}/ldscripts/ble_projects
    ${BSP_PATH}/memory/include
    ${BSP_PATH}/memory/src
    ${BSP_PATH}/peripherals/include
    ${BSP_PATH}/snc/include
    ${BSP_PATH}/snc/src
    ${BSP_PATH}/system/sys_man
    ${BSP_PATH}/system/sys_man/include
    ${BSP_PATH}/util/include
)

set(SDK_SRCS 
    ${BSP_SRCS}
    ${MIDDLEWARE_SRCS}
    ${INTERFACES_SRCS}
    ${FREERTOS_SRCS}
)
set(SDK_INCLUDES 
    ${MIDDLEWARE_INCLUDES}
    ${INTERFACES_INCLUDES}
    ${FREERTOS_INCLUDES}
    ${BSP_INCLUDES}
)