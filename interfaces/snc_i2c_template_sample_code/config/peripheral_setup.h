/**
 ****************************************************************************************
 *
 * @file peripheral_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
