/**
 ****************************************************************************************
 *
 * @file usbpd.c
 *
 * @brief USB power delivery driver (STUSB1602)
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include "osal.h"
#include "sys_timer.h"
#include "hw_clk.h"
#include "hw_timer.h"
#include "hw_wkup.h"
#include "usbpd_def.h"
#include "usbpd_registers.h"
#include "usbpd_peripherals.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_user.h"
#include "usbpd_stusb_dpm_if.h"
#include "usbpd_pwr_if.h"
#include "periph_setup.h"
#include "usbpd_spi.h"
#include "usbpd.h"

extern OS_TASK task_usbpd_snk;
extern OS_TASK task_usbpd_src;

/****************************************************************************************************************
 * Defines
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Types
 ****************************************************************************************************************/

typedef enum
{
	TIM_PORT0_CRC,	// CRC check on Port0 not used in STUSB1602
	TIM_PORT0_RETRY,// Retry on Port0
	TIM_PORT1_CRC,	// CRC check on Port1 not used in STUSB1602
	TIM_PORT1_RETRY,// Retry on Port1
	TIM_PORT0_CA,	// CA check on Port0 used in STUSB1602 to control received byte duration for collision avoidance
	TIM_PORT1_CA,	// CA check on Port1 used in STUSB1602 to control received byte duration for collision avoidance
	TIM_MAX
} usbpd_timer_t;

/****************************************************************************************************************
 * Constants
 ****************************************************************************************************************/

/*
 * Timer config for USBPD:
 * clk_src = lp_clk -> input 32kHz
 * prescaler = 31 -> timer clock 1kHz
 * reload_val = 0 -> period 1ms
 */
static const timer_config usbpd_timer_cfg =
{
        .clk_src = HW_TIMER_CLK_SRC_INT,
        .prescaler = 31,
        .mode = HW_TIMER_MODE_TIMER,
        .timer = {
                .direction = HW_TIMER_DIR_UP,
                .reload_val = 0,
                .free_run = false
        },
        .pwm = {
                .frequency = 0,
                .duty_cycle = 0,
        }
};

/****************************************************************************************************************
 * Local Data
 ****************************************************************************************************************/

static uint64_t usbpd_counters[TIM_MAX];
static bool usbpd_link_state[2];

/****************************************************************************************************************
 * Local functions
 ****************************************************************************************************************/

static void usbpd_set_gpio(HW_GPIO_PORT port, HW_GPIO_PIN pin, bool value)
{
	if (value)
	{
		hw_gpio_set_active(port, pin);
	}
	else
	{
		hw_gpio_set_inactive(port, pin);
	}
	hw_gpio_pad_latch_enable(port, pin);
	hw_gpio_pad_latch_disable(port, pin);
}

static void usbpd_timer_cb(void)
{
	if (task_usbpd_snk)
	{
		OS_TASK_NOTIFY_FROM_ISR(task_usbpd_snk, USBPD_NOTIF_TICK, OS_NOTIFY_SET_BITS);
	}
}

static void usbpd_init_stack(void)
{
	// Global Init of USBPD HW
	STUSB1602_GlobalHwInit();

	// Initialize the Device Policy Manager
	if (USBPD_DPM_InitCore() != USBPD_OK)
	{
		printf("usbpd: err initcore\r\n");
		return;
	}

	// Initialise the DPM application
	if (USBPD_DPM_UserInit() != USBPD_OK)
	{
		printf("usbpd: err userinit\r\n");
		return;
	}

	// Initialize the Device Policy Manager
	if (USBPD_DPM_InitOS() != USBPD_OK)
	{
		printf("usbpd: err initos\r\n");
		return;
	}
}

/****************************************************************************************************************
 * External functions - gpio
 ****************************************************************************************************************/

void usbpd_gpio_reset_set(uint8_t port, bool state)
{
	HW_GPIO_PORT gport = (port == USBPD_SINK) ? PD_SINK_RST_PORT : PD_SRC_RST_PORT;
	HW_GPIO_PIN gpin = (port == USBPD_SINK) ? PD_SINK_RST_PIN : PD_SRC_RST_PIN;

	// reset device
	if (state)
	{
		hw_gpio_set_active(gport, gpin);
		hw_gpio_pad_latch_enable(gport, gpin);
	}
	else
	{
		hw_gpio_set_inactive(gport, gpin);
		hw_gpio_pad_latch_disable(gport, gpin);
	}
}

void usbpd_gpio_reset(uint8_t port)
{
	usbpd_gpio_reset_set(port, true);
	OS_DELAY_MS(50);
	usbpd_gpio_reset_set(port, false);
}

bool usbpd_gpio_get_alert(uint8_t port)
{
	if (port == USBPD_SINK)
	{
		return hw_gpio_get_pin_status(PD_SINK_IRQ_N_PORT, PD_SINK_IRQ_N_PIN);
	}
	else
	{
		return hw_gpio_get_pin_status(PD_SRC_IRQ_N_PORT, PD_SRC_IRQ_N_PIN);
	}
}

bool usbpd_gpio_get_cs(uint8_t port)
{
	if (port == USBPD_SINK)
	{
		return hw_gpio_get_pin_status(PD_SINK_CS_PORT, PD_SINK_CS_PIN);
	}
	else
	{
		return hw_gpio_get_pin_status(PD_SRC_CS_PORT, PD_SRC_CS_PIN);
	}
}

void usbpd_gpio_set_txen(uint8_t port, bool active)
{
	HW_GPIO_PORT gport = (port == USBPD_SINK) ?  PD_SINK_TXE_PORT : PD_SRC_TXE_PORT;
	HW_GPIO_PIN gpin = (port == USBPD_SINK) ?  PD_SINK_TXE_PIN : PD_SRC_TXE_PIN;
	usbpd_set_gpio(gport, gpin, active);
}

/****************************************************************************************************************
 * External functions - timers
 ****************************************************************************************************************/

void USBPD_TIM_Start(uint8_t id, uint16_t us_time)
{
	if (id < TIM_MAX)
	{
		usbpd_counters[id] = sys_timer_get_uptime_usec() + us_time;
	}
}

uint8_t USBPD_TIM_IsExpired(uint8_t id)
{
	if (id < TIM_MAX)
	{
		if (sys_timer_get_uptime_usec() >= usbpd_counters[id])
		{
			return 1;
		}
	}
	return 0;
}

/****************************************************************************************************************
 * External functions - callbacks
 ****************************************************************************************************************/

void usbpd_set_link(uint8_t port, bool state)
{
	if (state && !usbpd_link_state[port])
	{
		// enable source if sink connected
		if (port == USBPD_SINK)
		{
			usbpd_enable(USBPD_SOURCE, true);
		}
	}
	else if (!state && usbpd_link_state[port])
	{
		// disable source after disconnect if sink not connected
		if ((port == USBPD_SOURCE) && !usbpd_link_state[USBPD_SINK])
		{
			usbpd_enable(USBPD_SOURCE, false);
		}
	}
	usbpd_link_state[port] = state;
}

uint8_t usbpd_set_voltage(uint8_t port, uint32_t vbus_mv)
{
	if (DPM_Ports[port].DPM_IsConnected)
	{
		printf("usbpd (%d): vol = %lu\r\n", port, vbus_mv);
	}

	if (port == USBPD_SINK)
	{
		// update source pdo
		if ((DPM_Ports[port].DPM_IsConnected) && (vbus_mv > 5000))
		{
			USBPD_PWR_IF_Update_PDO_Values(USBPD_SOURCE, 0, DPM_Ports[port].DPM_RequestedVoltage, DPM_Ports[port].DPM_RequestedCurrent);
			USBPD_PWR_IF_Update_PDO_Count(USBPD_SOURCE, 1);
		}
		else
		{
			USBPD_PWR_IF_Update_PDO_Count(USBPD_SOURCE, 0);
		}
	}
	else if (port == USBPD_SOURCE)
	{
		// update source vin passthrough
		if ((vbus_mv > 5000) && (vbus_mv == DPM_Ports[USBPD_SINK].DPM_RequestedVoltage))
		{
			usbpd_set_gpio(PD_VIN_ENA_PORT, PD_VIN_ENA_PIN, true);
		}
		else
		{
			usbpd_set_gpio(PD_VIN_ENA_PORT, PD_VIN_ENA_PIN, false);
		}
	}

	#ifdef USBPD_VBUS_RANGING
	// update vbus ranging
	STUSB16xx_HW_IF_Set_VBus_Monitoring(port, vbus_mv, 10, 10);
	#endif

	return USBPD_OK;
}

uint8_t usbpd_set_currentlimit(uint8_t port, uint32_t ibus_ma)
{
	if (DPM_Ports[port].DPM_IsConnected)
	{
		printf("usbpd (%d): cur = %lu\r\n", port, ibus_ma);
	}
	return USBPD_OK;
}

uint16_t usbpd_get_vbus(uint8_t port)
{
	// no measurement possible, return set voltage
	printf("usbpd (%d): vbus %lu\r\n", port, DPM_Ports[port].DPM_RequestedVoltage);
	return DPM_Ports[port].DPM_RequestedVoltage;
}

int16_t usbpd_get_ivbus(uint8_t port)
{
	return 0;
}

/****************************************************************************************************************
 * External functions
 ****************************************************************************************************************/

void usbpd_init(void)
{
	// create timer
	hw_timer_init(HW_TIMER4, &usbpd_timer_cfg);
	hw_timer_register_int(HW_TIMER4, usbpd_timer_cb);

	// init counters
	for (int i = 0; i < TIM_MAX; i++)
	{
		usbpd_counters[i] = 0;
	}

	// init state
	for (int p = 0; p < 2; p++)
	{
		usbpd_link_state[p] = false;
	}

        // init spi
        usbpd_spi_init();

	// vin
	hw_gpio_configure_pin_power(PD_VIN_ENA_PORT, PD_VIN_ENA_PIN, PD_VIN_GPIO_POWER);
	hw_gpio_configure_pin(PD_VIN_ENA_PORT, PD_VIN_ENA_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);

	// sink
	hw_gpio_configure_pin_power(PD_SINK_RST_PORT, PD_SINK_RST_PIN, PD_SINK_GPIO_POWER);
	hw_gpio_configure_pin(PD_SINK_RST_PORT, PD_SINK_RST_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);
	hw_gpio_configure_pin_power(PD_SINK_IRQ_N_PORT, PD_SINK_IRQ_N_PIN, PD_SINK_GPIO_POWER);
	hw_gpio_configure_pin(PD_SINK_IRQ_N_PORT, PD_SINK_IRQ_N_PIN, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, false);
	hw_gpio_configure_pin_power(PD_SINK_TXE_PORT, PD_SINK_TXE_PIN, PD_SINK_GPIO_POWER);
	hw_gpio_configure_pin(PD_SINK_TXE_PORT, PD_SINK_TXE_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);

	// source
	hw_gpio_configure_pin_power(PD_SRC_RST_PORT, PD_SRC_RST_PIN, PD_SRC_GPIO_POWER);
	hw_gpio_configure_pin(PD_SRC_RST_PORT, PD_SRC_RST_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);
	hw_gpio_configure_pin_power(PD_SRC_IRQ_N_PORT, PD_SRC_IRQ_N_PIN, PD_SRC_GPIO_POWER);
	hw_gpio_configure_pin(PD_SRC_IRQ_N_PORT, PD_SRC_IRQ_N_PIN, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, false);
	hw_gpio_configure_pin_power(PD_SRC_TXE_PORT, PD_SRC_TXE_PIN, PD_SRC_GPIO_POWER);
	hw_gpio_configure_pin(PD_SRC_TXE_PORT, PD_SRC_TXE_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);

	// reset source
	// note: only required in dead battery mode, in which case sink cannot be reset, will disconnect power
	usbpd_gpio_reset(USBPD_SOURCE);

	// init stack
	usbpd_init_stack();
}

void usbpd_enable(uint8_t port, bool enabled)
{
	// init wkup
	if (port == USBPD_SINK)
	{
		hw_wkup_gpio_configure_pin(PD_SINK_IRQ_N_PORT, PD_SINK_IRQ_N_PIN, enabled, HW_WKUP_PIN_STATE_LOW);
		hw_wkup_gpio_configure_pin(PD_SINK_CS_PORT, PD_SINK_CS_PIN, enabled, HW_WKUP_PIN_STATE_HIGH);
	}
	else
	{
		hw_wkup_gpio_configure_pin(PD_SRC_IRQ_N_PORT, PD_SRC_IRQ_N_PIN, enabled, HW_WKUP_PIN_STATE_LOW);
		hw_wkup_gpio_configure_pin(PD_SRC_CS_PORT, PD_SRC_CS_PIN, enabled, HW_WKUP_PIN_STATE_HIGH);
	}
	if (enabled)
	{
		printf("usbpd (%d): enabled\r\n", port);
	}
	else
	{
		printf("usbpd (%d): disabled\r\n", port);
	}
}

void usbpd_alert(uint8_t port)
{
	OS_TASK task = (port == USBPD_SINK) ? task_usbpd_snk : task_usbpd_src;
	if (task)
	{
		OS_TASK_NOTIFY_FROM_ISR(task, USBPD_NOTIF_IRQ, OS_NOTIFY_SET_BITS);
	}
}

void usbpd_start(void)
{
	// Start the Device Policy Manager
	if (USBPD_DPM_StartOS() != USBPD_OK)
	{
		printf("usbpd: err startos\r\n");
	}

	// start timer
	hw_timer_enable_clk(HW_TIMER4);
	hw_timer_enable(HW_TIMER4);

	// enable sink
	usbpd_enable(USBPD_SINK, true);
}

void usbpd_task(void *pvParameters)
{
	uint32_t port = (uint32_t)pvParameters;
	uint32_t val;

	do
	{
		// wait for action
		OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &val, OS_TASK_NOTIFY_FOREVER);

		// handle events
		if (val & USBPD_NOTIF_IRQ)
		{
			STUSB16xx_HW_IF_Alert_Check(port);
		}
		if (val & USBPD_NOTIF_SPI)
		{
			usbpd_spi_cs_event(port);
		}
		if (val & USBPD_NOTIF_SPI_TX)
		{
			usbpd_spi_tx_frame(port);
		}
		if (val & USBPD_NOTIF_SPI_TD)
		{
			usbpd_spi_tx_done(port);
		}

		// handle timer
		if ((val & USBPD_NOTIF_TICK) && (port == USBPD_SINK))
		{
			// run stack timers
			USBPD_DPM_TimerCounter();
		}
	}
	while (true);
}
