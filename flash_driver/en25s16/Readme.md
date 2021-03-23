# DA1469x EN25S16B Flash driver

______________________________________________________________________

## Example description

This example runs the FreRTOS operation with the EN25S16B flash attached to the
DA1469x device.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA1469X Bluetooth Smart SoC devices.
  - The Pro Development kit or USB is needed for this example.
  - Connect the Development kit to the host computer.
  - The development kit needs to have the flash replaced with EN25S16B

- **Software configuration**

  - This example requires:

  * Smartsnippets Studio 2.0.6.
  * SDK10.0.2

  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Compile the QSPI target

- Recompile the cli_programmer with the added flash driver (config/qspi_en25s16.h)

- Program the device and reboot

- Use your favorite terminal to the serial port (FTDI lower index port) with the following parameters

  - baudrate: 115200
  - data: 8 bits
  - stop: 1 bit
  - parity: None
  - flow  control: none

- The termnal displays the "!" characters every second or so.

## Known Limitations

- There are No known limitations for this example.
