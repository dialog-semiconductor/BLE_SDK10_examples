/**
 ****************************************************************************************
 *
 * @file usbpd.h
 *
 * @brief USB power delivery driver
 *
 ****************************************************************************************
 */
#ifndef USBPD_H
#define USBPD_H

#include <stdbool.h>

#define USBPD_NOTIF_IRQ		(1 << 0)
#define USBPD_NOTIF_SPI		(1 << 1)
#define USBPD_NOTIF_SPI_TX	(1 << 2)
#define USBPD_NOTIF_SPI_TD	(1 << 3)
#define USBPD_NOTIF_TICK	(1 << 4)

typedef enum
{
	USBPD_SINK = 0,
	USBPD_SOURCE
} usbpd_dev_t;

// gpio
void usbpd_gpio_reset_set(uint8_t port, bool state);
void usbpd_gpio_reset(uint8_t port);
bool usbpd_gpio_get_alert(uint8_t port);
bool usbpd_gpio_get_cs(uint8_t port);
void usbpd_gpio_set_txen(uint8_t port, bool active);

// timers
void usbpd_timer_start(uint8_t id, uint16_t us_time);
uint8_t usbpd_timer_get_expired(uint8_t id);

// callbacks
void usbpd_set_link(uint8_t port, bool state);
uint8_t usbpd_set_voltage(uint8_t port, uint32_t vbus_mv);
uint8_t usbpd_set_currentlimit(uint8_t port, uint32_t ibus_ma);
uint16_t usbpd_get_vbus(uint8_t port);
int16_t usbpd_get_ivbus(uint8_t port);

// generic
void usbpd_init(void);
void usbpd_enable(uint8_t port, bool enable);
void usbpd_alert(uint8_t port);
void usbpd_start(void);
void usbpd_task(void *pvParameters);

#endif //USBPD_H
