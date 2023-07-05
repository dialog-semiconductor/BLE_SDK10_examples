/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "ad_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configUART_ADAPTER == 1)

#include "ad_uart.h"

/**************************** UART2 configuration start ****************************/
const ad_uart_io_conf_t uart2_io_conf = {
        /* UART RX pin configuration */
        .rx = {
                /* RX pin configuration selection */
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_6,
                /* RX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_UART2_RX,
                        .high = false,
                },
                /* RX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_GPIO,
                        .high = true,
                },
        },
        /* UART TX pin configuration */
        .tx = {
                /* TX pin configuration selection */
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_7,
                /* TX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART2_TX,
                        .high = false,
                },
                /* TX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_GPIO,
                        .high = true,
                },
        },
        /* Select the Voltage Level for the UART GPIOs */
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_uart_driver_conf_t uart2_uart_driver_conf = {
        {
                .baud_rate = HW_UART_BAUDRATE_115200,   /* Select the baud rate */
                .data = HW_UART_DATABITS_8,             /* Select the data bits  */
                .parity = HW_UART_PARITY_NONE,          /* select the Parity    */
                .stop = HW_UART_STOPBITS_1,             /* Select the number of Stop Bits */
                .auto_flow_control = 0,                 /* Enable/Disable the HW flow control.
                                                         * 0 - Disable the use of RTS/CTS flow control.
                                                         * 1 - Enable the use of RTS/CTS flow control.
                                                         *
                                                         *     NOTE: UART1 does not have RTS/CTS capabilities
                                                         *           RTS/CTS can be used only with UART2/UART3 */
                .use_fifo = 1,                          /* Enable/Disable the use of the UART HW FIFO */
                .use_dma = 1,                           /* Enable/Disable the use of DMA for UART transfers. */
                .tx_dma_channel = HW_DMA_CHANNEL_3,     /* If DMA is used, select the DMA channels to use for RX/TX */
                .rx_dma_channel = HW_DMA_CHANNEL_2,     /* The DMA is activated only for transfers >=2 bytes */
                .tx_fifo_tr_lvl = 0,                    /* Set the TX FIFO threshold level for generating the threshold interrupts */
                .rx_fifo_tr_lvl = 0,                    /* Select the FIFO threshold trigger level in the RX FIFO at which the Received Data Available
                                                         * Interrupt is generated.
                                                         * It also determines when the dma_rx_req_n signal is asserted when DMA Mode (FCR[3]) = 1.
                                                         * See DA1469x datasheet for details */
        }
};

const ad_uart_controller_conf_t uart2_uart_conf = {
.id = HW_UART2,                                         /* Select the HW UART block to configure */
        .io = &uart2_io_conf,                           /* Select the GPIOs settings to use */
        .drv = &uart2_uart_driver_conf,                 /* Select the UART controller operation parameters to use */
};

/**************************** UART2 configuration end ****************************/

/**************************** UART3 configuration start ****************************/
const ad_uart_io_conf_t uart3_io_conf = {
        /* UART RX pin configuration */
        .rx = {
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_8,
                /* RX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_UART3_RX,
                        .high = false,
                },
                /* RX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_GPIO,
                        .high = true,
                },
        },
        /* UART TX pin configuration */
        .tx = {
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_9,
                /* TX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART3_TX,
                        .high = false,
                },
                /* TX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_GPIO,
                        .high = true,
                },
        },
        /* RTSN */
        /* is needed only when .auto_flow_control = 1
         * do not include it in case CTS/RTS is not used to save some GPIOS
         * for other purpose */
        .rtsn = {
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_0,
                /* RSTN pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART3_RTSN,
                        .high = false,
                },
                /* RSTN pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_GPIO,
                        .high = true,
                },
        },
        /* CTSN */
        /* is needed only when .auto_flow_control = 1
         * do not include it in case CTS/RTS is not used to save some GPIOS
         * for other purpose */
        .ctsn = {
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_7,
                /* CSTN pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_UART3_CTSN,
                        .high = false,
                },
                /* CSTN pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_GPIO,
                        .high = true,
                },
        },
        /* Select the Voltage Level for the UART GPIOs */
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_uart_driver_conf_t uart3_uart_driver_conf = {
        {
                .baud_rate = HW_UART_BAUDRATE_1000000,   /* Select the baud rate */
                .data = HW_UART_DATABITS_8,             /* Select the data bits  */
                .parity = HW_UART_PARITY_NONE,          /* select the Parity    */
                .stop = HW_UART_STOPBITS_1,             /* Select the number of Stop Bits */
                .auto_flow_control = 1,                 /* Enable/Disable the HW flow control.
                                                         * 0 - Disable the use of RTS/CTS flow control.
                                                         * 1 - Enable the use of RTS/CTS flow control.
                                                         *
                                                         *     NOTE: UART1 does not have RTS/CTS capabilities
                                                         *           RTS/CTS can be used only with UART2/UART3 */
                .use_fifo = 1,                          /* Enable/Disable the use of the UART HW FIFO */
                .use_dma = 1,                           /* Enable/Disable the use of DMA for UART transfers. */
                .tx_dma_channel = HW_DMA_CHANNEL_5,     /* If DMA is used, select the DMA channels to use for RX/TX */
                .rx_dma_channel = HW_DMA_CHANNEL_4,     /* The DMA is activated only for transfers >=2 bytes */
                .tx_fifo_tr_lvl = 0,                    /* Set the TX FIFO threshold level for generating the threshold interrupts */
                .rx_fifo_tr_lvl = 0,                    /* Select the FIFO threshold trigger level in the RX FIFO at which the Received Data Available
                                                         * Interrupt is generated.
                                                         * It also determines when the dma_rx_req_n signal is asserted when DMA Mode (FCR[3]) = 1.
                                                         * See DA1469x datasheet for details */
        }
};

const ad_uart_controller_conf_t uart3_uart_conf = {
        .id = HW_UART3,                                 /* Select the HW UART block to configure */
        .io = &uart3_io_conf,                           /* Select the GPIOs settings to use */
        .drv = &uart3_uart_driver_conf,                 /* Select the UART controller operation parameters to use */
};

/**************************** UART3 configuration end ****************************/

#endif


#ifdef __cplusplus
}
#endif
