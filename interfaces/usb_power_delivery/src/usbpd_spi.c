/**
 ****************************************************************************************
 *
 * @file usbpd_spi.c
 *
 * @brief USB power delivery spi
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include "osal.h"
#include "usbpd_def.h"
#include "usbpd_stusb_dpm_if.h"
#include "periph_setup.h"
#include "spi.h"
#include "usbpd.h"
#include "usbpd_spi.h"

extern OS_TASK task_usbpd_snk;
extern OS_TASK task_usbpd_src;
extern STUSB16xx_PORT_HandleTypeDef Ports[USBPD_PORT_COUNT];

/****************************************************************************************************************
 * Defines
 ****************************************************************************************************************/

//#define USBPD_DEBUG_SPI_RX
//#define USBPD_DEBUG_SPI_TX

//#define USBPD_SPI_TX_LOOPBACK
//#define USBPD_SPI_TX_DMA

/****************************************************************************************************************
 * Types
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Constants
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Local Data
 ****************************************************************************************************************/

uint8_t usbpd_buf_sink_rx[TXRX_BUFFER_SIZE];
uint8_t usbpd_buf_sink_tx[TXRX_BUFFER_SIZE];
#if (USBPD_PORT_COUNT == 2)
uint8_t usbpd_buf_source_rx[TXRX_BUFFER_SIZE];
uint8_t usbpd_buf_source_tx[TXRX_BUFFER_SIZE];
#endif

static uint32_t usbpd_tx_bitcnt[2];

bool usbpd_spi_rx_debug[2];
bool usbpd_spi_tx_debug[2];

/****************************************************************************************************************
 * Local functions
 ****************************************************************************************************************/

static inline uint8_t usbpd_spi_reverse(uint8_t b)
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

static void usbpd_spi_rx_frame(uint8_t port, uint16_t frame_len)
{
	uint8_t r, prev_bit, curr_bit;
	uint32_t temp_data = 0;
	UnwrapData_TypeDef* ud = &(Ports[port].unwrapdata);
	uint8_t* frame_buf = Ports[port].pRxBuffPtr;

	// init decoding variables, discard first two bytes
	ud->exed_flag = 0;
	ud->preamble = 0;
	ud->dataindex = 0;
	ud->dataoffset = 0;
	ud->index = 2;
	ud->offset = 0;

	// reverse bit order
	for (r = 0; r < frame_len; r++)
	{
		frame_buf[r] = usbpd_spi_reverse(frame_buf[r]);
	}

	// loop all bytes (and one more, assume array is large enough)
	while ((ud->exed_flag == 0) && ((ud->index + 2) <= frame_len))
	{
		// end of preamble not identified yet
		if (ud->preamble < 2)
		{
			// search end of preamble
			r = frame_buf[ud->index] ^ 0xAA;

			if (r == 0x00 || r == 0xFF)
			{
				// preamble
				ud->preamble = 1;
				ud->index++;
			}
			else if (ud->preamble)
			{
				// received data contains the end of preamble
				prev_bit = (frame_buf[ud->index-1] >> 7) & 0x01;
				while (ud->offset < 8)
				{
					curr_bit = (frame_buf[ud->index] >> ud->offset) & 0x01;
					if (prev_bit == curr_bit)
					{
						// preamble identified, index and offset identify the byte and the bit position of data after preamble
						if (curr_bit == 0)
						{
							if (ud->offset == 0)
							{
								ud->offset = 7;
								ud->index--;
							}
							else
							{
								ud->offset--;
							}
						}
						ud->preamble = 2;
						break;
					}
					prev_bit = curr_bit;
					ud->offset++;
				}
			}
			else
			{
				// invalid data before preamble
				ud->index++;
			}
		}
		else
		{
			// extract 10-bits from data flow (structured in bytes)
			// memcpy(&temp_data, &frame_buf[ud->index], 3);
			temp_data = ( (uint32_t)frame_buf[ud->index] | ((uint32_t)frame_buf[ud->index + 1 ] << 8 )) | ((uint32_t)frame_buf[ud->index + 2 ] << 16 );
			temp_data = (temp_data >> ud->offset) & 0x3FF;
			ud->index += ud->offset <= 5 ? 1 : 2;
			ud->offset = (ud->offset + 2) & 7;

			// callback core phy accumulate
			if (Ports[port].cbs.USBPD_HW_IF_RX_Accumulate != NULL)
			{
				Ports[port].cbs.USBPD_HW_IF_RX_Accumulate(port, temp_data);
			}

			// EOP detecting
			if ((temp_data & 0x1F) == 0x0D)
			{
				ud->exed_flag = 2;
			}
		}
	}

	// rx completed callback
	if (Ports[port].cbs.USBPD_HW_IF_RX_Completed != NULL)
	{
		USBPD_PHY_RX_Status_TypeDef res = Ports[port].cbs.USBPD_HW_IF_RX_Completed(port);
		if ((res != USBPD_PHY_RX_STATUS_OK) && (res != USBPD_PHY_RX_STATUS_INVALID))
		{
			printf("usbpd (%d): rx error %d\r\n", port, res);
			res = USBPD_PHY_RX_STATUS_ERROR;
		}
		if ((ud->index + 4) < frame_len)
		{
			printf("usbpd (%d): rx leftover bytes %lu\r\n", port, frame_len - ud->index);
			res = USBPD_PHY_RX_STATUS_ERROR;
		}

		#ifdef USBPD_DEBUG_SPI_RX
		if (usbpd_spi_rx_debug[port])
		#else
		if (res == USBPD_PHY_RX_STATUS_ERROR)
		#endif
		{
			printf("usbpd (%d): rx raw ", port);
			for (int cnt = 0; cnt < frame_len; cnt++)
			{
				printf("%02x ", usbpd_spi_reverse(frame_buf[cnt]));
			}
			printf("\r\n");
			usbpd_spi_rx_debug[port] = false;
		}
	}
}

/****************************************************************************************************************
 * External functions
 ****************************************************************************************************************/

void usbpd_spi_cs_event(uint8_t port)
{
        HW_SPI_ID id = (port == USBPD_SINK) ? HW_SPI1 : HW_SPI2;
	uint16_t len;

	if (Ports[port].State == HAL_USBPD_PORT_STATE_BUSY_RX)
	{
		// stop read
		len = spi_read_stop(id);

		// set the state of the port
		Ports[port].State = HAL_USBPD_PORT_STATE_READY;

		// handle received frame, may update state to waiting
		if (len > 0)
		{
			usbpd_spi_rx_frame(port, len);
		}
	}
	else if (Ports[port].State == HAL_USBPD_PORT_STATE_BUSY_TX)
	{
		// set the state of the port
		Ports[port].State = HAL_USBPD_PORT_STATE_READY;
		OS_TASK_NOTIFY((port == USBPD_SINK) ? task_usbpd_snk : task_usbpd_src, USBPD_NOTIF_SPI_TD, OS_NOTIFY_SET_BITS);
	}
	else if (Ports[port].State != HAL_USBPD_PORT_STATE_READY)
	{
		// state is ready on boot
		printf("usbpd (%d): unexpected cs event in state %d\r\n", port, Ports[port].State);
	}

	if (Ports[port].State == HAL_USBPD_PORT_STATE_READY)
	{
		// callback core phy reset
		if (Ports[port].cbs.USBPD_HW_IF_RX_Reset != NULL)
		{
			Ports[port].cbs.USBPD_HW_IF_RX_Reset(port);
		}

		// clean buffer
		memset(Ports[port].pRxBuffPtr, 0x00, TXRX_BUFFER_SIZE);

		// set the state of the port
		Ports[port].State = HAL_USBPD_PORT_STATE_BUSY_RX;

		// start receiving next frame
		spi_read_start(id, Ports[port].pRxBuffPtr, TXRX_BUFFER_SIZE);
	}
}

void usbpd_spi_cs(uint8_t port)
{
	OS_TASK task = (port == USBPD_SINK) ? task_usbpd_snk : task_usbpd_src;
	if (task)
	{
		OS_TASK_NOTIFY_FROM_ISR(task, USBPD_NOTIF_SPI, OS_NOTIFY_SET_BITS);
	}
}

void usbpd_spi_tx_done(uint8_t port)
{
	// handle sent frame, may update state to waiting
	if (Ports[port].cbs.USBPD_HW_IF_TxCompleted != NULL)
	{
		Ports[port].cbs.USBPD_HW_IF_TxCompleted(port, 0);
	}
}

void usbpd_spi_tx_frame(uint8_t port)
{
        HW_SPI_ID id = (port == USBPD_SINK) ? HW_SPI1 : HW_SPI2;
	uint8_t* frame_buf = Ports[port].pTxBuffPtr;
	uint32_t bitcnt = usbpd_tx_bitcnt[port];
	uint16_t frame_len = DIV_ROUND_UP(bitcnt, 8) + TX_PREAMBLE_SIZE;
	uint8_t i;

	// check for expected state: waiting for receive event, or expecting tx event
	if ((Ports[port].State != HAL_USBPD_PORT_STATE_BUSY_RX) && (Ports[port].State != HAL_USBPD_PORT_STATE_WAITING))
	{
		printf("usbpd (%d): tx while busy in state %d\r\n", port, Ports[port].State);
		return;
	}
	if (!usbpd_gpio_get_cs(port))// || (Ports[port].CCx == CCNONE))
	{
		printf("usbpd (%d): tx while receiving (cc %lu)\r\n", port, Ports[port].CCx);
		return;
	}

	// reverse bit order
	for (i = 0; i < frame_len; i++)
	{
		frame_buf[i] = usbpd_spi_reverse(frame_buf[i]);
	}

	// add symbol 1 after eop, as in received frames
	frame_buf[frame_len++] = 0x40;

	#ifdef USBPD_SPI_TX_LOOPBACK
	// receive sent frame
	Ports[port].cbs.USBPD_HW_IF_RX_Reset(port);
	usbpd_spi_rx_frame(port, frame_len);
	return USBPD_OK;
	#endif

	#ifdef USBPD_DEBUG_SPI_TX
	if (usbpd_spi_tx_debug[port])
	{
		printf("usbpd (%d): tx raw ", port);
		for (i = 0; i < frame_len; i++)
		{
			printf("%02x ", frame_buf[i]);
		}
		printf("\r\n");
		usbpd_spi_tx_debug[port] = false;
	}
	#endif

	// stop read
	if (Ports[port].State == HAL_USBPD_PORT_STATE_BUSY_RX)
	{
		spi_read_stop(id);
	}

	// spare clock cycles at the end of transmission are calculated
	Ports[port].TxSpareBits = (bitcnt % 8);
	Ports[port].State = HAL_USBPD_PORT_STATE_BUSY_TX;

	#ifdef USBPD_SPI_TX_DMA
	// start write
	spi_write(id, frame_buf, frame_len);

	// tx enable, cs will be asserted by chip
	usbpd_gpio_set_txen(port, true);
	#else
	// tx enable, cs will be asserted by chip
	usbpd_gpio_set_txen(port, true);

	// start write
	spi_write_blocking(id, frame_buf, frame_len, Ports[port].TxSpareBits);

	// tx disable, cs will be released by chip, raising a cs event
	usbpd_gpio_set_txen(port, false);
	#endif
}

uint8_t usbpd_spi_tx_buffer(uint8_t port, uint8_t *frame_buf, uint32_t bitcnt)
{
	// if called from rx handler, prevent starting new rx
	if (Ports[port].State == HAL_USBPD_PORT_STATE_READY)
	{
		Ports[port].State = HAL_USBPD_PORT_STATE_WAITING;
	}

	usbpd_tx_bitcnt[port] = bitcnt;
	OS_TASK_NOTIFY((port == USBPD_SINK) ? task_usbpd_snk : task_usbpd_src, USBPD_NOTIF_SPI_TX, OS_NOTIFY_SET_BITS);
	return USBPD_OK;
}

uint8_t usbpd_spi_tx_bist(uint8_t port)
{
	printf("usbpd (%d): tx bist not implemented\r\n", port);
	return USBPD_OK;
}

#ifdef USBPD_SPI_TX_DMA
void usbpd_spi_tx(uint8_t port)
{
	// tx disable, cs will be released by chip, raising a cs event
	usbpd_gpio_set_txen(port, false);
}
#endif

void usbpd_spi_init(void)
{
        for (int p = 0; p < 2; p++)
        {
        	usbpd_tx_bitcnt[p] = 0;
        	usbpd_spi_rx_debug[p] = false;
        	usbpd_spi_tx_debug[p] = false;
        }
}
