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


/**
 * SPI 1 configuration
 */
#define SPI1_DO_GPIO_PORT               ( HW_GPIO_PORT_0 )
#define SPI1_DO_GPIO_PIN                ( HW_GPIO_PIN_26 )

#define SPI1_DI_GPIO_PORT               ( HW_GPIO_PORT_0 )
#define SPI1_DI_GPIO_PIN                ( HW_GPIO_PIN_24 )

#define SPI1_CLK_GPIO_PORT              ( HW_GPIO_PORT_0 )
#define SPI1_CLK_GPIO_PIN               ( HW_GPIO_PIN_21 )


/* SPI chip-select pin(s) */
#define CUSTOM_DEVICE_CS_GPIO_PORT      ( HW_GPIO_PORT_0 )
#define CUSTOM_DEVICE_CS_GPIO_PIN       ( HW_GPIO_PIN_17 )


/**
 * SPI 2 configuration
 */


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
