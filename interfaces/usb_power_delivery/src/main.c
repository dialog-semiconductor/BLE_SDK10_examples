/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Main file
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "osal.h"

#include "ble_mgr.h"
#include "hw_gpio.h"
#include "hw_wkup.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_usb.h"

#include "i2c.h"
#include "periph_setup.h"
#include "spi.h"
#include "usbpd.h"
#include "usbpd_spi.h"

/****************************************************************************************************************
 * Defines
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Types
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Constants
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Local Data
 ****************************************************************************************************************/

OS_TASK task_init = NULL;
OS_TASK task_usbpd_snk = NULL;
OS_TASK task_usbpd_src = NULL;

/****************************************************************************************************************
 * Local functions
 ****************************************************************************************************************/

static void wkup_gpio_p0_interrupt_cb()
{
	// handle pd sink irq
	if (hw_wkup_get_status(PD_SINK_IRQ_N_PORT) & (1 << PD_SINK_IRQ_N_PIN))
	{
		hw_wkup_clear_status(PD_SINK_IRQ_N_PORT, (1 << PD_SINK_IRQ_N_PIN));
		usbpd_alert(USBPD_SINK);
	}
	// handle pd sink spi
	if (hw_wkup_get_status(PD_SINK_CS_PORT) & (1 << PD_SINK_CS_PIN))
	{
		hw_wkup_clear_status(PD_SINK_CS_PORT, (1 << PD_SINK_CS_PIN));
		usbpd_spi_cs(USBPD_SINK);
	}
}

static void wkup_gpio_p1_interrupt_cb()
{
	// handle pd source irq
	if (hw_wkup_get_status(PD_SRC_IRQ_N_PORT) & (1 << PD_SRC_IRQ_N_PIN))
	{
		hw_wkup_clear_status(PD_SRC_IRQ_N_PORT, (1 << PD_SRC_IRQ_N_PIN));
		usbpd_alert(USBPD_SOURCE);
	}
	// handle pd source spi
	if (hw_wkup_get_status(PD_SRC_CS_PORT) & (1 << PD_SRC_CS_PIN))
	{
		hw_wkup_clear_status(PD_SRC_CS_PORT, (1 << PD_SRC_CS_PIN));
		usbpd_spi_cs(USBPD_SOURCE);
	}
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
#ifdef BOARD_TEST
	// uart on usb pins
	hw_usb_enable_usb_pads_without_pullup();

	// gpio required on test board
	hw_gpio_configure_pin(HW_GPIO_PORT_1, HW_GPIO_PIN_1, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
	hw_gpio_pad_latch_enable(HW_GPIO_PORT_1, HW_GPIO_PIN_1);
	hw_gpio_configure_pin_power(HW_GPIO_PORT_0, HW_GPIO_PIN_3, HW_GPIO_POWER_VDD1V8P);
	hw_gpio_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_3, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);
	hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, HW_GPIO_PIN_3);
#endif
}

static void system_init_task( void *pvParameters )
{
	cm_sys_clk_init(dg_configDEFAULT_CLK);

	cm_apb_set_clock_divider(apb_div1);
	cm_ahb_set_clock_divider(ahb_div1);
	cm_lp_clk_init();

	// Set system clock
	cm_sys_clk_set(dg_configDEFAULT_CLK);

	// Init hardware
	pm_system_init(periph_init);
	pm_sleep_mode_set(pm_mode_active);
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);

	printf("\r\nUSB power delivery\r\n");

	// Init wkup
	hw_wkup_init(NULL);
	hw_wkup_register_gpio_p0_interrupt(wkup_gpio_p0_interrupt_cb, 1);
	hw_wkup_register_gpio_p1_interrupt(wkup_gpio_p1_interrupt_cb, 1);

	// Init I2C
	i2c_init();

	// Init SPI
	spi_init();

	// Init USB-PD
	usbpd_init();

	// Init BLE
	ble_mgr_init();
	ble_enable();

	// Start USB-PD
	OS_TASK_CREATE( "USBPD snk",
			usbpd_task,
			(void *)USBPD_SINK,
			256 * OS_STACK_WORD_SIZE,
			OS_TASK_PRIORITY_HIGHEST,
			task_usbpd_snk );
	OS_ASSERT(task_usbpd_snk);

#ifndef BOARD_SINK_ONLY
	OS_TASK_CREATE( "USBPD src",
			usbpd_task,
			(void *)USBPD_SOURCE,
			256 * OS_STACK_WORD_SIZE,
			OS_TASK_PRIORITY_HIGHEST,
			task_usbpd_src );
	OS_ASSERT(task_usbpd_src);
#endif
	OS_DELAY_MS(100);
	usbpd_start();

	OS_TASK_DELETE(task_init);
}

/****************************************************************************************************************
 * External functions
 ****************************************************************************************************************/

int main( void )
{
	OS_BASE_TYPE status;

	// system init task
	status = OS_TASK_CREATE("SysInit",
			system_init_task,
			( void * ) 0,
			512 * OS_STACK_WORD_SIZE,
			OS_TASK_PRIORITY_HIGHEST,
			task_init );
	OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

	// start tasks and timer
	vTaskStartScheduler();
	for ( ;; );
}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook( void )
{
	ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook( void )
{
}

/**
 * @brief Application stack overflow hook
 */
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName )
{
	ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook( void )
{
}
