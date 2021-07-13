FreeRTOS template demo application {#freertos_retarget}
======================================================

## Overview

This application is a simple implementation of a FreeRTOS template application. The main.c creates a template task 
which prints periodically a '#' character to the output.

## Installation procedure

The project is located in the \b `projects/dk_apps/templates/freertos_retarget` folder.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## File structure

The following file structure will be created:

* projects/dk_apps/templates/freertos_retarget
        * config
                * custom_config_qspi.h
                * custom_config_ram.h
        * sdk
        * startup
        * main.c

## Existing build configurations

The template contains build configurations for executing it from RAM or QSPI. 

- `DA1469X-00-Debug_RAM`. The project is built to be run from RAM. The executable is built with debug (-Og) information.
- `DA1469X-00-Debug_QSPI`. The project is built to be run from QSP. The executable is built with debug (-Og) information.
- `DA1469X-00-Release_RAM`. The project is built to be run from RAM. The executable is built with no debug information and size optimization (-Os).
- `DA1469X-00-Release_QSPI`. The project is built to be run from QSPI. The executable is built with no debug information and size optimization (-Os).

## RTT ENABLE

The template gives the user the option to redirect the output either to RTT or Retarget by defining the CONFIG_RETARGET pre-processor macro.

> Note: The template uses the retarget option by default.
