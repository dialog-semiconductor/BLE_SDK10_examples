/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2022 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#include "ad_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configUART_ADAPTER == 1)

#include "ad_uart.h"

/**************************** UART1 configuration start ****************************/

const ad_uart_io_conf_t uart1_io_conf = {
        /* UART RX pin configuration */
        .rx = {
                /* RX pin configuration selection */
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_8,
                /* RX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART_RX,
                        .high = true,
                },
                /* RX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART_RX,
                        .high = true,
                },
        },

        /* UART TX pin configuration */
        .tx = {
                /* TX pin configuration selection */
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_9,
                /* TX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART_TX,
                        .high = true,
                },
                /* TX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART_TX,
                        .high = true,
                },
        },

        /* Select the Voltage Level for the UART GPIOs */
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_uart_driver_conf_t uart1_uart_driver_conf = {
        /* UART controller operation parameters */
        {
                .baud_rate = HW_UART_BAUDRATE_115200,   /* Select the baud rate */
                .data = HW_UART_DATABITS_8,             /* Select the data bits */
                .parity = HW_UART_PARITY_NONE,          /* select the Parity */
                .stop = HW_UART_STOPBITS_1,             /* Select the number of Stop Bits */
                .auto_flow_control = 0,                 /* Enable/Disable the HW flow control.
                                                         * 0 - Disable the use of RTS/CTS flow control.
                                                         * 1 - Enable the use of RTS/CTS flow control.
                                                         *
                                                         *     NOTE: UART1 does not have RTS/CTS capabilities
                                                         *           RTS/CTS can be used only with UART2/UART3 */
                .use_fifo = 1,                          /* Enable/Disable the use of the UART HW FIFO */
                .use_dma = 1,                           /* Enable/Disable the use of DMA for UART transfers. */
                .tx_dma_channel = HW_DMA_CHANNEL_1,     /* If DMA is used, select the DMA channels to use for RX/TX */
                .rx_dma_channel = HW_DMA_CHANNEL_0,     /* The DMA is activated only for transfers >=2 bytes */
                .tx_fifo_tr_lvl = 0,                    /* Set the TX FIFO threshold level for generating the THRE interrupts */
                .rx_fifo_tr_lvl = 0,                    /* Select the FIFO threshold trigger level in the RX FIFO at which the Received Data Available
                                                         * Interrupt is generated.
                                                         * It also determines when the dma_rx_req_n signal is asserted when DMA Mode (FCR[3]) = 1.
                                                         * See DA1469x datasheet for details */
        }
};

const ad_uart_controller_conf_t uart1_uart_conf = {
        .id = HW_UART1,                                 /* Select the HW UART block to configure */
        .io = &uart1_io_conf,                           /* Select the GPIOs settings to use */
        .drv = &uart1_uart_driver_conf,                 /* Select the UART controller operation parameters to use */
};

/**************************** UART1 configuration end ****************************/

/**************************** UART2 configuration start ****************************/
const ad_uart_io_conf_t uart2_io_conf = {
        /* UART RX pin configuration */
        .rx = {
                /* RX pin configuration selection */
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_16,
                /* RX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART2_RX,
                        .high = true,
                },
                /* RX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART2_RX,
                        .high = true,
                },
        },
        /* UART TX pin configuration */
        .tx = {
                /* TX pin configuration selection */
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_17,
                /* TX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART2_TX,
                        .high = true,
                },
                /* TX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART2_TX,
                        .high = true,
                },
        },
        /* RTSN */
        /* is needed only when .auto_flow_control = 1
         * do not include it in case CTS/RTS is not used to save some GPIOS
         * for other purpose */
        .rtsn = {
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_18,
                /* RSTN pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART2_RTSN,
                        .high = true,
                },
                /* RSTN pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART2_RTSN,
                        .high = true,
                },
        },
        /* CTSN */
        /* is needed only when .auto_flow_control = 1
         * do not include it in case CTS/RTS is not used to save some GPIOS
         * for other purpose */
        .ctsn = {
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_19,
                /* CSTN pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART2_CTSN,
                        .high = true,
                },
                /* CSTN pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART2_CTSN,
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
                .auto_flow_control = 1,                 /* Enable/Disable the HW flow control.
                                                         * 0 - Disable the use of RTS/CTS flow control.
                                                         * 1 - Enable the use of RTS/CTS flow control.
                                                         *
                                                         *     NOTE: UART1 does not have RTS/CTS capabilities
                                                         *           RTS/CTS can be used only with UART2/UART3 */
                .use_fifo = 1,                          /* Enable/Disable the use of the UART HW FIFO */
                .use_dma = 1,                           /* Enable/Disable the use of DMA for UART transfers. */
                .tx_dma_channel = HW_DMA_CHANNEL_3,     /* If DMA is used, select the DMA channels to use for RX/TX */
                .rx_dma_channel = HW_DMA_CHANNEL_2,     /* The DMA is activated only for transfers >=2 bytes */
                .tx_fifo_tr_lvl = 0,                    /* Set the TX FIFO threshold level for generating the THRE interrupts */
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
                .pin = HW_GPIO_PIN_26,
                /* RX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART3_RX,
                        .high = true,
                },
                /* RX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART3_RX,
                        .high = true,
                },
        },
        /* UART TX pin configuration */
        .tx = {
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_27,
                /* TX pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART3_TX,
                        .high = true,
                },
                /* TX pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART3_TX,
                        .high = true,
                },
        },
        /* RTSN */
        /* is needed only when .auto_flow_control = 1
         * do not include it in case CTS/RTS is not used to save some GPIOS
         * for other purpose */
        .rtsn = {
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_28,
                /* RSTN pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART3_RTSN,
                        .high = true,
                },
                /* RSTN pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_OUTPUT,
                        .function = HW_GPIO_FUNC_UART3_RTSN,
                        .high = true,
                },
        },
        /* CTSN */
        /* is needed only when .auto_flow_control = 1
         * do not include it in case CTS/RTS is not used to save some GPIOS
         * for other purpose */
        .ctsn = {
                .port = HW_GPIO_PORT_0,
                .pin = HW_GPIO_PIN_29,
                /* CSTN pin configuration when device is active ('On') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART3_CTSN,
                        .high = true,
                },
                /* CSTN pin configuration when device is in sleep ('Off') */
                {
                        .mode = HW_GPIO_MODE_INPUT_PULLUP,
                        .function = HW_GPIO_FUNC_UART3_CTSN,
                        .high = true,
                },
        },
        /* Select the Voltage Level for the UART GPIOs */
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_uart_driver_conf_t uart3_uart_driver_conf = {
        {
                .baud_rate = HW_UART_BAUDRATE_115200,   /* Select the baud rate */
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
                .tx_fifo_tr_lvl = 0,                    /* Set the TX FIFO threshold level for generating the THRE interrupts */
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
