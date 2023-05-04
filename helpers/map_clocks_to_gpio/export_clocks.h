/**
 ****************************************************************************************
 *
 * @file export_clocks.h
 *
 * @brief Exporting clocks header file
 *
 * Copyright (C) 2015-2023 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef EXPORT_CLOCKS_H_
#define EXPORT_CLOCKS_H_

#ifndef CLOCK_MODE_DEFAULT
#define CLOCK_MODE_DEFAULT     CLOCK_MODE_MANUAL
#endif

#ifndef CLOCK_SOURCE_DEFAULT
#define CLOCK_SOURCE_DEFAULT   CLOCK_SOURCE_XTAL32K
#endif

#ifndef CLOCK_PORT_DEFAULT
#define CLOCK_PORT_DEFAULT     HW_GPIO_PORT_1
#endif

#ifndef CLOCK_PIN_DEFAULT
#define CLOCK_PIN_DEFAULT      HW_GPIO_PIN_11
#endif

#ifndef CLOCK_STATUS_DEFAULT
#define CLOCK_STATUS_DEFAULT   true
#endif

/*****************************************************************************************
 * MANUAL or AUTO mode configuration
 */
typedef enum {
        CLOCK_MODE_MANUAL  = 0x0,
        CLOCK_MODE_AUTO         ,
        CLOCK_MODE_INVALID
} CLOCK_MODE;

/*****************************************************************************************
 * Clock Source configuration.
 */
typedef enum {
        CLOCK_SOURCE_XTAL32K = 0x0  ,
        CLOCK_SOURCE_RC32K          ,
        CLOCK_SOURCE_RCX            ,
        CLOCK_SOURCE_XTAL32M        ,
        CLOCK_SOURCE_RC32M          ,
        CLOCK_SOURCE_DIVN           ,
        CLOCK_SOURCE_INVALID
} CLOCK_SOURCE;

/*****************************************************************************************
 * Selected GPIO status
 */
typedef enum {
        PORT_STATUS_AVAILABLE    = 0x0,
        PORT_STATUS_UNAVAILABLE       ,
        PORT_STATUS_WARNING           ,
        PORT_STATUS_INVALID
} PORT_STATUS;

/*****************************************************************************************
 * System Initialization
 */
void export_clocks_init(void);

#endif /* EXPORT_CLOCKS_ */
