# AES encryption/decryption & HASH example using secured keys.#

----------

##Overview

This example is a sample implementation of the AES encryption- and decryption algorithm using the built-in encryption engine with 128-, 192- or 256-bit secured key, stored in OTP.

**Features:**

The system is initialized @ 32 MHz and the SECURE_BOOT_REGISTER is set, meaning that the Symmetric key area in OTP (0x10080A00 - 0x10080B00) can only be accessed by the 'secured' DMA engine using channel 7.
 The encryption /decryption task is started which takes (user) data from an array ***vector[64]***. The encryption keys ***key_256b[32]***, ***key_192b[24]*** or ***key_128b[16]***, which is programmed in OTP (see later how-to) is copied to the encryption engine, the vector[64] data is encrypted and stored in RAM. The encrypted data is then compared with a pre-encrypted array called ***cbc_256_cipher*** (or cbc_192_cipher / cbc_128_cipher for the other keys).
Secondly the encrypted data is decrypted by the same aes engine, using the same (en)cryption key and compared with the original input (user)data.
This process restarts every 2.5 seconds. 
The serial port gives some debug information.

## How to run the example

### HW & SW Configurations

- **Hardware Configurations**
    - This example runs on a DA1469x Bluetooth Smart SoC.
    - A DA1469x Pro Development Kit is needed for this example.
- **Software Configurations**
    - Download the latest SDK version for the DA1469x family of devices (10.0.10.x)
    - **SEGGER's J-Link** tools should be downloaded and installed.

### Initial Setup

- Download the source code from the Support Website
- Import the project into your workspace.
- Connect the target device to your host PC.
- Compile the code (either in Release or Debug mode) and load it into the chip.
- Open a serial terminal (115200/8 - N - 1)
- Press the reset button on DA1469x daughterboard to start executing the application. 

##How is the key programmed in OTP:##

In the secured area of the OTP there's place for 8 256-bit symmetric keys. (For the 192- / 128-bit example only the first 192 / 128 bits of the key are used).

The aes_hash application itself checks if there is a key programmed in OTP already and if it's not programmed than it is programmed by the application using the following code snippet:

    void aes_check_otp_keys(void)
    {
            uint32_t otp_data;
    
            otp_data = hw_otpc_word_read(MEMORY_OTP_USER_DATA_KEYS_PAYLOAD_START / 4);

            /*
             * Check if OTP Key area is empty. If it is empty (0xffffffff), the 256-bit key (key_256b[32])     is programmed into
             * the OTP
             *
             *  */

            if (otp_data == 0xFFFFFFFF)
                    {
                    uint32_t aes_key[8];
                    /* write keys into otp */
                    aes_construct_words(aes_key, key_256b, 32);
                    hw_otpc_prog_and_verify(aes_key, (MEMORY_OTP_USER_DATA_KEYS_PAYLOAD_START / 4), 8);
            }
    } 

The keys can also be programmed into OTP using the ***cli_programmer***. 

To program the **key_256b** into the OTP use the following (after starting the gdbserver): *cli_programmer gdbserver write_otp 0x10080A00 8 0x2b7e1516 0x28aed2a6 0xabf71588 0x09cf4f3c 0x809079e5 0x62f8ead2 0x522c6b7b 0xc810f32b*

The keys can be read back using the following:
*cli_programmer gdbserver read_otp 0x10080A00 64*

Result:

    0280   16 15 7E 2B   ..~+
    0281   A6 D2 AE 28   ...(
    0282   88 15 F7 AB   ....
    0283   3C 4F CF 09   <O..
    0284   E5 79 90 80   .y..
    0285   D2 EA F8 62   ...b
    0286   7B 6B 2C 52   {k,R
    0287   2B F3 10 C8   +...

**Note 1:**

Set (only) one of the following flags to compile for the different options:

    /* MACROS for enabling/disabling tasks, ONE AT A TIME!*/
    #define AES_CBC_128_NON_FRAG_DATA           (0)
    #define AES_CTR_192_FRAG_DATA               (0)
    #define AES_CBC_256_NON_FRAG_DATA          	(1)
    #define HASH_SHA_256                        (0)

**Note 2:**

In the file(s) *custom_config_xxx.h* there are 2 other flags:

    #define dg_configAES_USE_OTP_KEYS				(1)

If set to (1) then the encryption key in the OTP is used, if set to (0) the encryption key in file *vectors_keys.h* is used.

    #define dg_configAES_USE_SECURE_DMA_CHANNEL		(1)

(If set to (1) then the secured encryption key is transferred over the secured DMA(7) channel, else it's copied using the M33 CPU (not secured).

##Remark
This example is only to show how the encryption engine works with secured keys. Since the secure bit is set in the application code, the key(s) in OTP can be read back using the cli-programmer.

More information about encryption keys and the secure boot mechanism can be found at Dialogs website:

[DA1469x_secure_boot](http://lpccs-docs.dialog-semiconductor.com/da1469x_secure_boot/index.html) 

## Known Issues

There are no known issues.

 