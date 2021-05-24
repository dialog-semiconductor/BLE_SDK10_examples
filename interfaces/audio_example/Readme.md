Audio example application {#audio_example}
======================================================

## Overview

This application demonstrates the DA1469x Audio Processing Unit in combination with the DA7212 / DA7213 and DA7217 / DA7218 CODECs.
In this example the CODEC is connected as I2S slave.

There are five demos supplied as examples:
1. PDM-SRC-PCM: Loopback and resample digital microphone (PDM) input to speaker output over CODEC PCM interface.
2. PDM-SRC-MEMORY-> MEMORY-SRC-PCM: Record and resample digital microphone to flash memory then play back to speaker.
3. PCM-SRC-PCM: Loopback analog microphone (PCM) input to speaker (PCM) output.
4. PCM-SRC-MEMORY-> MEMORY-SRC-PCM: Record and resample analog microphone to flash memory then play back to speaker.
5. (Default) MEMORY-SRC-PCM: Playback a raw PCM file from flash memory periodically.

## HW and SW configuration

* **Hardware configuration**

	- This example runs on the DA1469x Bluetooth Smart SoC devices.
	- The DA1469x Pro Development kit is needed for this example.
	- Connect the Development kit to the host computer.

* **Software configuration**
  - This example requires:
  - Smartsnippets Studio V2.0.12 or greater.
  - SDK 10.0.10.118
  - **SEGGER’s J-Link** tools should be downloaded and installed.

## Demos

### 1. PDM-SRC-PCM  

- <b>Description</b>:<br> 
The demo allows the user to listen from PDM Mic input (SPK0838HT4H-B digital microphone), 
using a headset, hooked in a DA7218 module jack. The PDM input is resampled to the CODEC's sampling rate.

- <b>Hardware Setup</b>: 
	- Dialog DA7218 mikrobus module
	- SPK0838HT4H-B digital microphone
	- S1 dip switch (CTS) set to off.
PDM settings: 2MHz bit clock, stereo.
CODEC settings: 16bit @ 16kHz, I2S stereo.

To enable the demo go to the `includes/audio_task.h` file and set:

~~~{.c}
#define DEMO_SELECTION                  DEMO_PDM_MIC
~~~

### 2. PDM-SRC-MEMORY-> MEMORY-SRC-PCM 

- <b>Description</b>:<br> 
When this demo is run, the user is prompted to press the K1 button to record their voice over the digital microphone to QSPI flash Log partition, resampled to 8kHz PCM. When the recording is finished the recording is played back to the CODEC resampled to the CODEC's sampling rate. The recording time is configurable and set to 2 sec by default.

- <b>Hardware Setup</b>: 
	- Dialog DA7218 mikrobus module
	- SPK0838HT4H-B digital microphone
	- S1 dip switch (CTS) set to off.
	
To enable the demo go to the `includes/audio_task.h` file and set:

~~~{.c}
#define DEMO_SELECTION DEMO_PDM_RECORD_PLAYBACK
~~~

PDM settings: 2MHz bit clock, stereo.
Memory file settings: 16bit @ 8kHz, PCM stereo.
CODEC settings: 16bit @ 16kHz, I2S stereo.

### 3. PCM-SRC-PCM 

- <b>Description</b>:<br> 
The demo allows the user to loopback their voice from the PCM mic input, to the Right speaker of a headset hooked in a DA7218 module jack.

- <b>Hardware Setup</b>: 
	- Dialog DA7218 mikrobus module

To enable the demo go to the `includes/audio_task.h` file and set:

~~~{.c}
#define DEMO_SELECTION DEMO_PCM_MIC
~~~

CODEC settings: 16bit @ 16kHz, I2S stereo. 

### 4. PCM-SRC-MEMORY-> MEMORY-SRC-PCM  
- <b>Description</b>:<br> 
When this demo is run, the user is prompted to press the K1 button to record their voice over the CODEC analog microphone to QSPI flash Log partition, resampled to 8kHz PCM. When the recording is finished the recording is played back to the CODEC resampled back to the CODEC's sampling rate. The recording time is configurable and set to 2 sec by default.

- <b>Hardware Setup</b>: 
	- Dialog DA7218 mikrobus module

To enable the demo go to the `includes/audio_task.h` file and set:

~~~{.c}
#define DEMO_SELECTION DEMO_PCM_RECORD_PLAYBACK
~~~

Memory file settings: 16bit @ 8kHz, PCM mono.
CODEC settings: 16bit @ 16kHz, I2S stereo.

#### 5. MEMORY-SRC-PCM 
- <b>Description</b>:<br> 
To run this demo, upload the reference 1kHz and 500Hz tones raw audio data files to the QSPI flash Log partition first and run the demo. Then the audio data is resampled and played back as stereo channels to the CODEC periodically, following the sequence: 
- CODEC is suspended for 1 sec.
- CODEC is enabled for 1 sec.
- Audio is played back for 2 sec.

This demo is enabled by default.

Memory file settings: 16bit @ 8kHz, PCM stereo.
CODEC settings: 16bit @ 16kHz, I2S stereo.

To upload the reference files, run:

- binaries\cli_programmer.exe COMX write_partition 4 0x80000 500Hz_sin_8kHz_16bit_mono.raw
- binaries\cli_programmer.exe COMX write_partition 4 0 1kHz_sin_8kHz_16bit_mono.raw

(Where X the default UART COM port of DA14690)

## Existing build configurations

The template contains build configurations for executing it from RAM or QSPI. 

- `DA1469X-00-Debug_RAM`. The project is built to be run from RAM. The executable is built with debug (-Og) information.
- `DA1469X-00-Debug_QSPI`. The project is built to be run from QSPI. The executable is built with debug (-Og) information.
- `DA1469X-00-Release_RAM`. The project is built to be run from RAM. The executable is built with no debug information and size optimization (-Os).
- `DA1469X-00-Release_QSPI`. The project is built to be run from QSPI. The executable is built with no debug information and size optimization (-Os).

## Dependencies / Considerations
- This Software Example is compatible with the SDK10 release 10.0.10.118 
and with changes in the Audio Processing Unit driver. 
- The changes are included in the lpc_cs_da1469x_sdk_10/ repository and also in the patch files sdk10.0.8.patch, sdk10.0.10.patch
- The patch is a unified diff. To apply, copy to the SDK root directory and use the GNU patch program like so:
	``patch -u -p1 < sdk10.0.10.patch``
- The patch contains changes in the APU interface, so any existing application code should change accordingly.  
- More specifically, any application call to sys_audio_mgr_default_pcm_data_init() should check that the number of PCM channels is 2.
- This project can not be shared outside Dialog before checking if the customer has signed the SLA. 
- "Integer Only" I2S mode is not supported. 
- To connect a different CODEC instead of DA721X consult the datasheets and contact Customer support [Customer support](<https://support.dialog-semiconductor.com/forums/bluetooth>).



## License

**************************************************************************************

 Copyright (c) 2020 Dialog Semiconductor. All rights reserved.

 This software ("Software") is owned by Dialog Semiconductor. By using this Software
 you agree that Dialog Semiconductor retains all intellectual property and proprietary
 rights in and to this Software and any use, reproduction, disclosure or distribution
 of the Software without express written permission or a license agreement from Dialog
 Semiconductor is strictly prohibited. This Software is solely for use on or in
 conjunction with Dialog Semiconductor products.

 EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.

**************************************************************************************
