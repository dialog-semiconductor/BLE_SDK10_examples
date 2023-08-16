/**
 ****************************************************************************************
 *
 * @file spi.h
 *
 * @brief SPI header
 *
 ****************************************************************************************
 */
#ifndef SPI_H
#define SPI_H

#include "hw_spi.h"

void spi_init(void);
void spi_deinit(void);

void spi_read_start(HW_SPI_ID dev, uint8_t *buffer, uint16_t len_max);
uint16_t spi_read_stop(HW_SPI_ID dev);

void spi_write(HW_SPI_ID dev, uint8_t *buffer, uint16_t len);
void spi_write_blocking(HW_SPI_ID dev, uint8_t *buffer, uint16_t len, uint8_t spare);

#endif //SPI_H
