/**
 ****************************************************************************************
 *
 * @file peripheral_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef PERIPHERAL_SETUP_H_
#define PERIPHERAL_SETUP_H_


#define I2C1_SCL_PORT                   ( HW_GPIO_PORT_0 )
#define I2C1_SCL_PIN                    ( HW_GPIO_PIN_28 )

#define I2C1_SDA_PORT                   ( HW_GPIO_PORT_0 )
#define I2C1_SDA_PIN                    ( HW_GPIO_PIN_29 )


/* The address in which the external I2C device listens to */
#define I2C_CUSTOM_DEVICE_ADDRESS      (0x50)

/*
 * External interrupt trigger source
 **/
#define WKUP_TRIGGER_PORT               ( HW_GPIO_PORT_0 )
#define WKUP_TRIGGER_PIN                ( HW_GPIO_PIN_6  )

/*
 * TIMER1 capture trigger source
 **/
#define TIMER1_CAPTURE_PIN              ( HW_TIMER_GPIO_PIN_6      )
#define TIMER1_CAPTURE_POL              ( HW_TIMER_TRIGGER_FALLING )

#endif /* PERIPHERAL_SETUP_H_ */
