# DA1469x AT25XE321 Flash driver

______________________________________________________________________

## Example description

This example runs the FreRTOS operation with the Adesto AT25XE321 flash attached to the
DA1469x device.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA1469X Bluetooth Smart SoC devices.
  - The Pro Development kit or USB is needed for this example.
  - Connect the Development kit to the host computer.
  - The development kit needs to have the flash replaced with AT25XE321

- **Software configuration**

  - This example requires:

  * Smartsnippets Studio 2.0.6 or newer
  * SDK10.0.4 or newer

  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Compile the QSPI target

- Recompile the cli_programmer with the added flash driver (config/qspi_at25xe321.h)

- Program the device and reboot

- Use your favorite terminal to the serial port (FTDI lower index port) with the following parameters

  - baudrate: 115200
  - data: 8 bits
  - stop: 1 bit
  - parity: None
  - flow  control: none

- The termnal displays the "!" characters every second or so.

### Flash Configuration Setup

<configuration name="AT25XE321"> 
	<flash_size>0x400000</flash_size> 
	<flash_burstcmda_reg_value>0xa8a000eb</flash_burstcmda_reg_value> 
	<flash_burstcmdb_reg_value>0x00000076</flash_burstcmdb_reg_value> 
	<flash_write_config_command>0x71 0x02 0x02 0xE0 0x80 0x40 0x07</flash_write_config_command>
</configuration>

## Known Limitations

- There are No known limitations for this example.
