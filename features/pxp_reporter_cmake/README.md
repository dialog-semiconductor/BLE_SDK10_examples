# pxp-reporter CMake example

This project is an example of using SDK10 with CMake and VSCode instead of SmartSnippets Studio.

## Requirements

* cmake >= 3.16
* gcc-arm-none-eabi installed
* Cortex-Debug extension from VSCode marketplace
* [SDKv10](https://www.dialog-semiconductor.com/system/files/2020-12/SDK_10.0.10.118.zip) or latest, look at [Resources](https://www.dialog-semiconductor.com/products/bluetooth-low-energy/da1469x#tab-field_tab_content_resources) for more information.

## Project contains

* example project (pxp_reporter from SDK) in project folder.
* cmake folder with useful scripts
* BSP folder. Warning: extract SDK here! this folder should contain at least binaries/ config/ sdk/ and utilities/ folders from the SDKv10.
* Various tools in .vscode/ folder. I made commands for build and clean code, and for configuring QSPI params, downloading firmware to the QSPI and erasing (over SWD or UART). Also look at the launch.json for debugging command
* Simple GitHub action script for binary building.