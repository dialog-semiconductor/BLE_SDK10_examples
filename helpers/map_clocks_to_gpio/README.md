# Mapping System Clocks To GPIO

This sample code demonstrates mapping system clocks to GPIO, typically for debugging purposes. The clocks that can be exported are XTAL32K, RC32K, RCX, XTAL32M, RC32M as well as DIVN. The DA1469x family of devices supports two mapping modes. That is, the manual mode, where any supported clock and be mapped to any valid GPIO and the auto mode, where each of the supported clocks is mapped to a dedicated port. The demonstration example defines default clock settings that map XTAL32K to P1.10. These default values are defined in `export_clocks.h` as depicted below. Any of these configuration macros can be re-defined in the `config/custom_config_xxx.h` configuration file.

```
#ifndef CLOCK_MODE_DEFAULT
#define CLOCK_MODE_DEFAULT     CLOCK_MODE_MANUAL
#endif

#ifndef CLOCK_SOURCE_DEFAULT
#define CLOCK_SOURCE_DEFAULT   CLOCK_SOURCE_XTAL32K
#endif

#ifndef CLOCK_PORT_DEFAULT
#define CLOCK_PORT_DEFAULT     HW_GPIO_PORT_1
#endif

#ifndef CLOCK_PIN_DEFAULT
#define CLOCK_PIN_DEFAULT      HW_GPIO_PIN_10
#endif

#ifndef CLOCK_STATUS_DEFAULT
#define CLOCK_STATUS_DEFAULT   true
#endif
```

In addition to defining the clock setting at compile time, the example comes with a command line interface (CLI) which can be used to change any of the clock settings at run time. The supported commands along with their arguments and usage is demonstrated below.

- To change the clock source enter 'clock_source <arg1>', where:

  | arg1 | Clock Source                                                 |
  | :--- | ------------------------------------------------------------ |
  | 0    | To export the external XTAL32K low power clock.              |
  | 1    | To export the internal RC32K oscillator.                     |
  | 2    | To export the internal RCX low power clock. It's a pre-requisite that RCX is the selected as LP clock. |
  | 3    | To export the external XTAL32M main clock.                   |
  | 4    | To export the internal RC32M oscillator.                     |
  | 5    | To export the DIVN clock path. That path should reflect the XTAL32M clock. |

​       **NOTE:** If the auto clock mode is currently selected, the target port might be updated accordingly         

​       to meet system requirements. To get the new port assigned request the clock status.

- To change the clock mode enter 'clock_mode <arg1>', where: 

  | arg1 | Clock Mode                                                   |
  | :--- | ------------------------------------------------------------ |
  | 0    | To select the manual mode (any valid port can be mapped to any valid clock). |
  | 1    | To select the auto mode (each of the clock sources are mapped to specific ports). In this mode, the current port might be overwritten to meet system requirements. Before switching to auto mode the current port is stored so, it can be re-stored upon switching to manual mode. |

- To change the port where a clock is mapped to enter 'clock_port <arg1> <arg2>', where:

  | arg1     | Clock Port            |
  | :------- | --------------------- |
  | 0        | To select PORT0       |
  | 1        | To select PORT1       |
  | **arg2** | **Clock Port**        |
  | [0..31]  | To select a valid pin |

​      **NOTE:** If the auto clock mode is currently selected, an attempt to define a new port results in 

​      switching to manual mode.

- To enable or disable exporting a clock source enter 'clock_enable <arg1>', where:

  | arg1 | Clock Source                        |
  | :--- | ----------------------------------- |
  | 0    | To disable exporting a system clock |
  | 1    | To enable exporting a system clock  |

- To get the current clock status enter 'clock_status'. 

### HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro or USB development kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices. 
  - SEGGER's J-Link tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Download the source code from the support website or Dialog's GitHub.
- Import the `map_clocks_to_gpio` sample code into your SDK10 workspace (there should be no path dependencies inside the workspace's path).
- Connect the target device to your host PC.
- Compile the code (either in Release or Debug mode) and load it into the chip.
- Open a serial terminal (115200/8 - N - 1)
- Press the reset button on the DA1469x daughterboard to start executing the application.
- In the serial window enter a valid command to update the clock settings. The corresponding messages should be displayed in case a command is defined with invalid or incomplete parameters. A command is evaluated only if the requested clock setting value mismatches the current value.

## Known Limitations

There are no known limitations for this sample code.
