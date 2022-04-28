# Audio example application

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

### Hardware configuration

- This example runs on the DA1469x Bluetooth Smart SoC devices.
- The DA1469x Pro Development kit is needed for this example.
- The DA7212-EVAL Development kit is needed for this example.
- Connect the Development kit to the host computer.
- Connect the DA7212-EVAL to the DA1469x Pro Development kit.

### Software configuration

- This example requires:
- Smartsnippets Studio V2.0.16 or greater.
- SDK 10.0.12.139
- **SEGGER’s J-Link** tools should be downloaded and installed.



## Demonstrations

### 1. PDM-SRC-PCM  

**Description**:

The demo allows the user to listen from PDM Mic input (SPK0838HT4H-B digital microphone),
using a headset, hooked in a DA7218 module's jack. The PDM input is re-sampled to the CODEC's sampling rate.

**Hardware Setup**:

- Digital microphone
- S1 dip switch (CTS) set to off.

PDM settings: 2MHz bit clock, stereo.
CODEC settings: 16bit @ 16kHz, I2S stereo.

To enable the demo go to the `includes/audio_task.h` file and set:

```c
#define DEMO_SELECTION                  DEMO_PDM_MIC
```

### PDM-SRC-MEMORY-> MEMORY-SRC-PCM

**Description**:

When this demo is run, the user is prompted to press the K1 button to record their voice over the digital microphone to QSPI flash Log partition, resampled to 8kHz PCM. When the recording is finished the recording is played back to the CODEC resampled to the CODEC's sampling rate. The recording time is configurable and set to 2 sec by default.

**Hardware Setup**:

- Digital microphone
- S1 dip switch (CTS) set to off.

To enable the demo go to the `includes/audio_task.h` file and set:

```c
#define DEMO_SELECTION DEMO_PDM_RECORD_PLAYBACK
```

PDM settings: 2MHz bit clock, stereo.
Memory file settings: 16bit @ 8kHz, PCM stereo.
CODEC settings: 16bit @ 16kHz, I2S stereo.

### PCM-SRC-PCM

**Description**:

The demo allows the user to loopback their voice from the PCM mic input, to the Right speaker of a headset hooked in a DA7218 module jack.

**Hardware Setup**:

To enable the demo go to the `includes/audio_task.h` file and set:

```c
#define DEMO_SELECTION DEMO_PCM_MIC
```

CODEC settings: 16bit @ 16kHz, I2S stereo. 

### PCM-SRC-MEMORY-> MEMORY-SRC-PCM

**Description**:

When this demo is run, the user is prompted to press the K1 button to record their voice over the CODEC analog microphone to QSPI flash Log partition, resampled to 8kHz PCM. When the recording is finished the recording is played back to the CODEC resampled back to the CODEC's sampling rate. The recording time is configurable and set to 2 sec by default.

**Hardware Setup**:

To enable the demo go to the `includes/audio_task.h` file and set:

```c
#define DEMO_SELECTION DEMO_PCM_RECORD_PLAYBACK
```

Memory file settings: 16bit @ 8kHz, PCM mono.
CODEC settings: 16bit @ 16kHz, I2S stereo.

#### MEMORY-SRC-PCM

**Description**:

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

## Known Limitations

None
