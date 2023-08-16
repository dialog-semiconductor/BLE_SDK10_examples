/**
 ****************************************************************************************
 *
 * @file usbpd_spi.h
 *
 * @brief USB power delivery spi
 *
 ****************************************************************************************
 */
#ifndef USBPD_SPI_H
#define USBPD_SPI_H

void usbpd_spi_cs_event(uint8_t port);
void usbpd_spi_cs(uint8_t port);

void usbpd_spi_tx_done(uint8_t port);
void usbpd_spi_tx_frame(uint8_t port);
uint8_t usbpd_spi_tx_buffer(uint8_t port, uint8_t *buf, uint32_t bitcnt);
uint8_t usbpd_spi_tx_bist(uint8_t port);
void usbpd_spi_tx(uint8_t port);

void usbpd_spi_init(void);

#endif //USBPD_SPI_H
