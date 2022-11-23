# USB MSD with FatFs application
---

## Example description
This application is an example application implementing the MSD block mode USB storage 
device combined with an open source implementation internal FatFs system.
The application demonstrates the basic concept about how to implement USB MSD 
class in an application.

## HW and SW configuration

* **Hardware configuration**

	- This example runs on the DA1469x Bluetooth Smart SoC devices.
	- The DA1469x Pro Development kit is needed for this example.
	- Connect the Development kit to the host computer.

* **Software configuration**
  - This example requires:
  - Smartsnippets Studio V2.0.16 or greater.
  - SDK 10.0.10.x
  - **SEGGER’s J-Link** tools should be downloaded and installed.

## USB feature
- USB functionality is enabled with below defines in ``\config\custom_config_qspi.h``.

```c
#define dg_configUSE_USB_ENUMERATION            1
```
- USB framework is based on Segger emUSB-Device. Refer to below link for more 
  information and detailed documentation of the MSD class used.

  -  [https://www.segger.com/emusb.html](<https://www.segger.com/emusb.html>)

## Configurable parameters
- The default values for usb PID/VID can be used during development period. 
  It should be changed to your company's values.
	- USB PID/VID and com port name are in in ``usb_cdc_smsd.c``.
	- Windows driver(dialog_usb.inf).
    - The size of the NVMS partition used for storage medium for the MSD/FatFs.

## Operation of MSD
- Connect the debug/power USB ( USB1 on a proDK ).
- Compile and program the device with the example app.
- Reset the device.
- Connect the USB on the daughterboard to Host PC (Windows/Linux/OS X).
- No special driver is required.
- Confirm a portable storage disk is attached to Host.
- One text file will appear in the portable storage in Host.
- Any file or folder can be read/write/created/copied/deleted from Host.
- It is highly recommended to follow the safe removal procedure from the host 
  and not hot-unplug the device.
- No need to format the device. In case you decide to do so though format it 
  with FAT-12 to be compatible with the internal FatFs implementation and 
  enable th built-in code to read also the files.
  
> **_IMPORTANT NOTE:_**  
    It is important NOT to use the internal FatFs while the device is plugged to 
    USB as MSD device, because the internal FatFs and the MSD are operating at 
    raw block level both and they cannot be aware of what each-other is about to 
    write to the medium. 
    If both FatFs and MSD are operated at the same time the storage content and 
    structure will be corrupted and the contents will be possibly lost.
---


## How to run the example

### Initial Setup

To install the project follow the [General Installation and Debugging Procedure](<http://lpccs-docs.dialog-semiconductor.com/da1469x_starting_project/index.html>).

Please see user manual [DA1469x Getting Started with the Development Kit](<http://lpccs-docs.dialog-semiconductor.com/um-b-090-da1469x_getting_started/index.html>) for more information on the hardware development environment, how to install required software and download and run an example application on the DA1469x development platform.
  
### SDK file needed changes
For this project to be properly compiled the following changes are needed in the SDK files:

 1. Modification in ``sdk/bsp/config/bsp_memory_defaults.h``: 

    ``` diff
    --- a/sdk/bsp/config/bsp_memory_defaults.h
    +++ b/sdk/bsp/config/bsp_memory_defaults.h
    @@ -36,7 +36,7 @@
    #define BSP_MEMORY_DEFAULTS_H_

    #define PARTITION2(...)
    -#include "partition_table.h"
    +#include <partition_table.h>
    #undef PARTITION2

    /* ---------------------------------- Heap size configuration ----------------------------------- */

    ```

2. Modification in ``sdk/middleware/adapters/include/ad_nvms.h`` : 
    ``` diff
    --- a/sdk/middleware/adapters/include/ad_nvms.h
    +++ b/sdk/middleware/adapters/include/ad_nvms.h
    @@ -31,7 +31,7 @@
    #include <stdint.h>
    #include <stdbool.h>
    #include "ad_flash.h"
    -#include "partition_def.h"
    +#include <partition_def.h>
    #include "sdk_defs.h"
    ```
