SPI Producer Consumer example application.
======================================================

## Example Description

This example implements a simple producer / consumer communication scenario over 
the SPI interface using the SPI adapter layer.

Interface SPI1 is configured as master and SPI2 as slave, so there is the  
option to connect the two interfaces in loopback, or connect two SDKs together.

The producer task (SPI master) runs periodically every **producer_FREQUENCY_MS** and initiates a write transaction sending **producer_SPI_TX_DATA_LENGTH** bytes to the slave. 
The consumer task (SPI slave) initiates a read of **consumer_SPI_RCV_CB** bytes and runs periodically every **consumer_FREQUENCY_MS** or when the **consumer_SPI_RCV_CB** bytes are received in order to check if the **consumer_THRESHOLD** threshold is reached. If the consumer task runs and the threshold is not reached the task will print out the corresponding message. When the threshold is reached then the task will "consume" the data by simply printing them out in UART.

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


        **The following wiring / pairs (loopback wiring from MASTER to SLAVE) are needed before starting the app. The pins settings are located in `peripheral_setup.h`**
    
        | DA1469x     |             |
        |:-----------:|:-----------:|
        | Master      | Slave       |
        | P0_28 (CLK) | P0_24 (CLK) |
        | P0_31 (CS)  | P0_25 (CS)  |
        | P0_20 (DI)  | P0_26 (DO)  |
        | P0_21 (DO)  | P0_27 (DI)  |                                   

* **Software configuration**
  - Smartsnippets Studio V2.0.16 or greater.
  - Download the latest SDK version (10.0.10)
  - **SEGGER J-Link** tools should be downloaded and installed.

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

