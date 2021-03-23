# Battery SOC Fuel Gauge demo application

______________________________________________________________________

## Overview

This sample code aims at demonstrating the use of the battery SOC (state of charge) fuel gauge algorithm
in conjunction with the standard BAS (Battery Service) profile.
At the application layer three FreeRTOS tasks are created: The "SOC Reporter" task which enables the BLE periheral functionality with regular BLE advertising,
the "SOCF_CAL" task which calculates the SOC every 1,5 sec and the "SOC_log" task which prints the calculated SOC value every 1 sec.

### HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro Development Kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices (10.0.10.x)
  - **SEGGER's J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Download the source code from the Support Website
- Import soc_reporter project into your workspace, which includes the application code and the SDK.
- Import socf_library project into your workspace, which is the SOCF algorithm. Triggering a build to the soc_reporter project will also trigger a build to the soc_reporter project.
- Define SOCF_PERFORMANCE_TEST in custom_config_qspi.h.
- Build, download binary to flash.
- Connect the target device to your host PC.
- Open a serial terminal (115200/8 - N - 1)
- Press the reset button on DA1469x daughterboard to start executing the application.
- SOC, Voltage and Temperature readings will appear, every second
- Recommend SOCF_PERFORMANCE_TEST is undefined in real prodcut.

## APIs in each source file

### APIs in socf_client.c

- The APIs are system independent. Any modification is not required.

### APIs in socf_hal.c

- The APIs are made for DA1469x SDK and FreeRTOS.
- Modification is required if OS or another platform is used.
- Refer the descriptions in socf_hal.h.
- The change of ADC values in temp_degree may be required according to thermistor, reference voltage or target system.

### The APIs in socf_ucodes.c

- These are used only if SNC is used for measurement of battery voltage. Any modification is not required.

## Profile data and Parameters in socf_profile_data.h

- Parameters (socf_temp and socf_conf) should be defined according to use requirement.
- Lookup table (socf_luts) and capacitance table (socf_cap) in socf_profile_data.h should be filled with results from battery profiling.

## Configurations in socf_config.h

- Configuration in socf_config.h could be redefined according to use requirement.
- Set SOCF_USE_SNC to 1 if SNC is used for measurement of battery voltage.
- Define SOCF_SAMPLING_TIME if other interval of SOC calculation is required. It is recommended to use the default.
- Define SOCF_HAL_TIMER_TIME if other interval of system wakeup is required. It must be bigger than SOCF_SAMPLING_TIME. It is recommended to use the default.
- Define SOCF_IBAT_BOOT if power consumption at initial SOC calculation is different from the default.
- Define SOCF_TEMP_NUM according to use requirement.
- Define USE_SDADC_FOR_VBAT if SADC is used for measurement of battery voltage. This cannot be used with SOCF_USE_SNC.

\##Additional documentation##

A more extensive user manual can be found here: [DA1469x SoC User Manual](https://www.dialog-semiconductor.com/da1469x-soc-user-manual)
