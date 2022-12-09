SUOART application {#suouart}
======================

## Example Description

This example demonstrates Software Update over UART (SUOUART). The Pro Development Kit is used to demonstrate updating the firmware, but the firmware can also be updated by connecting an external microcontroller directly to the DA1469x UART Pins. An executable, `host_usb.exe`, is provided to run the example with a development kit connected to a PC. The source code used to create this executable is provided in `host_usb_updater.c`

Please note only Windows has been tested at this time.

## Software configuration

- [SDK10](https://www.dialog-semiconductor.com/da1469x_sdk_latest).
- [Smart Snippets Studio](https://www.renesas.com/us/en/software-tool/smartbond-development-tools) should be installed.

## Hardware Configurations

- This example runs on a DA1469x Bluetooth Smart SoC.
- A DA1469x Pro Development Kit is needed for this example.

### Prepare suouart

1. Import the project `suouart` into Smart Snippets Studio.
2. Build the project.
3. Erase flash entirely using `erase_qspi_jtag`.
4. Download `suouart` to flash using `program_qspi_jtag`.
5. Press the K2 (RESET) button on the DA1469x Daughterboard to reset the device. 

### Create a binary that will be used to udpate firmware 

1. Import `pxp_reporter` from `<path_to_SDK>/projects/dk_apps/demos/pxp_reporter` into Smart Snippets Studio.
2. To make the flash partition table match the `suouart` project, the below define must be included.
	- #define USE_PARTITION_TABLE_4MB_WITH_SUOTA

	The build configurations for SUOTA `DA1469x-00-Release_QSPI_SUOTA` and `DA1469x-00-Debug_QSPI_SUOTA` have the defines already. They can be used without any changes. 

3. Build this project with a QSPI configuration such as `DA1469x-00-Release_QSPI`, `DA1469x-00-Debug_QSPI`, `DA1469x-00-Release_QSPI_SUOTA` or `DA1469x-00-Debug_QSPI_SUOTA`.

### Add an image header to the binary

The `suouart` project is expecting a binray with a proper image header. See the [Flash Layout](http://lpccs-docs.renesas.com/um-b-092-da1469x_software_platform_reference/User_guides/User_guides.html#flash-layout) section of the Software Platform Reference for details on the image header image header format. 

To create an .img file with the proper header, you can use the mkimage.py script included with SDK10. Navigate to `<path_to_SDK>/utilities/python_scripts/suota/v11`
and open a command prompt. 

To run the script call: 

```
> python mkimage.py <path_to_build_configuration>/<binary> <path_to_build_configuration>/<output_img>
```

For example, to run the script with the binary created in [Create a binary that will be used to udpate firmware](#create-a-binary-that-will-be-used-to-udpate-firmware) call:

```
> python mkimage.py ../../../projects/dk_apps/demos/pxp_reporter/DA1469x-00-Release_QSPI_SUOTA/pxp_reporter.bin ../../../projects/dk_apps/demos/pxp_reporter/DA1469x-00-Release_QSPI_SUOTA/pxp_reporter.img
```
Note above assumes you built the `DA1469x-00-Release_QSPI_SUOTA` build configuration.

The output is an image file such as `pxp_reporter.1.0.0.1.img`. The file name contains a version number taken from `sw_version.h`.

### Run the host update script 

- When the development kit is connected to a PC, two virtual COM ports are created as described in the [Getting Started Guide](http://lpccs-docs.renesas.com/um-b-090-da1469x_getting_started/Connecting_The_Board/DA1469x_Connecting_The_Board.html#driver-installation). The `suouart` example uses pins P0_8 (Rx) and P0_9 (Tx) for UART. On the Pro Development Kit these are connected to the USB To UART Serial Converter IC, which allows us to communicate with a PC over a serial interface. The lower of the two virtual COM ports corresponds to the UART interface on the DA1469x. 

- Open a command prompt at `<path_on_your_machine>/suouart/suouart_host`. The executable `host_usb.exe` is provided to run with the example.

- Run `host_usb.exe 40 <path_to_image_file>/pxp_reporter.1.0.0.1.img -verbose`.
	- Where `40` is the lower virtual COM port of Pro Development Kit.
    - `<path_to_image_file>/pxp_reporter.1.0.0.1.img` is the path to the image file you created above
	- Debug message can be enabled with the `-verbose` option.

- Once the script finishes it will indicate the result (e.g Result: Pass): 

![test_result](assets/test_results.png)

Once the update process has completed successfully, the DA1469x will reset and start advertising the `pxp_reporter` application. 

![pxp_reporter](assets/pxp_reporter.png)

Please note the pxp_reporter built above did not include a `suouart` implementation, so the host_usb.exe script cannot be run again once pxp_reporter has been downloaded. To enable updating with `suouart` once your image is donwloaded, the `suouart` code must be integrated into your project.

### Command Flow Overview

The below sequence diagram provides of an overview of the commands/responses exchanged between the `host_usb.exe` script running on the PC and the DA1469x running `suouart`. 

![command_flow](assets/command_flow.png)