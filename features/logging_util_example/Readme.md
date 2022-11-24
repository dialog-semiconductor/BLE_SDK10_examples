Logging Mechanism Example
======================================================

## Example Description

This sample code provides a demonstration of the SDK10 logging utility. The logging mechanism is part of the SDK 10 and provides 4 seperate behavioral modes.
- **LOGGING_MODE_STANDALONE** : In this mode a logging queue is created as well as a logging dispatch task that runs periodically and checks for any available messages in the queue. If there are messages available the task will print them on UART.
- **LOGGING_MODE_QUEUE** : In this mode a queue is created as well but no dispatch task, its up to the application to create the dispatching task and push the available messages via an application defined channel.
- **LOGGING_MODE_RETARGET** : In this mode no queue is created the messages are printed via the RETARGET utility on UART. In the case of many logging tasks, messages will be shuffled.
- **LOGGING_MODE_RTT** : Identical to the LOGGING_MODE_RETARGET messages are printed on the RTT module and can be viewed via the RTT Viewer.

The logging mechanism enables the end user to divide the application logs on to 5 different severity levels and also choose which messages to be pushed into the logging queue depending on the message's severity level.

The mode of operation on the example can be configured from either the custom_config_qspi.h file or the custom_config_ram.h depending on the build. The current example creates 5 dummy tasks running every **mainCOUNTER_FREQUENCY_MS**. Each task is pushing a log message with each message having a different severity. 

For the **LOGGING_MODE_QUEUE** a BLE service is set (service UUID 00000000-1111-2222-2222-333333333333) with 3 characteristics:
 - **UUID 11111111-0000-0000-0000-111111111111**: A notifiable characteristic used for sending the logging messages.
 - **UUID 11111111-0000-0000-0000-222222222222**: A read only characteristic exposing the amount of discarded messages (messages that are pushed to queue but discarded since the queue was full).
 - **UUID 11111111-0000-0000-0000-333333333333**: A read, write  characteristic for read/write the severity level of the messages that should be pushed into the queue.

 The example comes with a web based BLE central that can be used to connect and acquire the logs created by the dummy tasks.

> Note: For exposing the queue that holds the logs as well as the variable that holds the suppressed messages a copy of the logging.c file is done (mod_logging.c). In the mod_logging.c the **static** qualifier of the queue as well the the suppressed_messages variable is removed.

## HW and SW configuration
* **Hardware configuration**

    - This example runs on the DA1469x Bluetooth Smart SoC devices.
    - The DA1469x Pro Development kit is needed for this example.
    - Connect the Development kit to the host computer.

        **UART terminal with the following settings is needed to observe the master task printouts**

        | Setting      | Value    |
        |:------------:|:--------:|
        | Baudrate     | 115200   |
        | Data bits    | 8        |
        | Stop bits    | 1        |
        | Parity       | None     |                                 

* **Software configuration**
  - Smartsnippets Studio V2.0.16 or greater.
  - Download the latest SDK version (10.0.10)
  - **SEGGER J-Link** tools should be downloaded and installed.

## File structure

The following file structure will be created:

* projects/dk_apps/templates/freertos_retarget
  * config
    * custom_config_qspi.h
    * custom_config_ram.h
  * Logging_chrome_central
  * sdk
  * segger_tools
  * startup
  * ble_queue_task.c
  * main.c
  * mod_logging.c

## Existing build configurations

The template contains build configurations for executing it from RAM or QSPI. 

- `DA1469X-00-Debug_RAM`. The project is built to be run from RAM. The executable is built with debug (-Og) information.
- `DA1469X-00-Debug_QSPI`. The project is built to be run from QSP. The executable is built with debug (-Og) information.
- `DA1469X-00-Release_RAM`. The project is built to be run from RAM. The executable is built with no debug information and size optimization (-Os).
- `DA1469X-00-Release_QSPI`. The project is built to be run from QSPI. The executable is built with no debug information and size optimization (-Os).

## How to run the example

### Initial Setup

- Download the source code from the Support Website.
- Import the project into your workspace.
- Compile and launch RAM or QSPI target
- Run from RAM or load in the flash

## Known Limitations

- There are No known limitations for this example. But you can check and refer to the following application note for
  [known hardware limitations](https://www.dialog-semiconductor.com/products/da1469x-product-family "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum link").
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.
