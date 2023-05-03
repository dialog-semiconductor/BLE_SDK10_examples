/**
 ****************************************************************************************
 *
 * @file dsps_port_usbd.h
 *
 * @brief DSPS port for USB CDC devices
 *
 * Copyright (C) 2023. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef DSPS_PORT_USBD_H_
#define DSPS_PORT_USBD_H_

/**
 * Application-defined routine for the USB CDC initialization
 *
 * \note All the necessary USB initialization actions are performed upon USB attach in the USB system service.
 *
 */
#define _SERIAL_PORT_OPEN(_dev)

/**
 * Application-defined routine for the USB CDC de-initialization
 *
 * \note All the necessary USB initialization actions are performed upon USB attach in the USB system service.
 *
 */
#define _SERIAL_PORT_CLOSE(_dev)

/**
 * Application-defined routine for the USB CDC read operations
 *
 * \param[in] _dev       Handle of a valid CDC instance. Should be retrieved via \sa cdc_usbd_get_handle()
 * \param[in] _data      Pointer to a buffer where the received data will be stored
 * \param[in] _len       Number of bytes to read
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been read the given timeout
 *
 */
#define _SERIAL_PORT_READ_DATA(_dev, _data, _len, _timeout)   read_from_cdc_usbd(_dev, _data, _len, _timeout)

/**
 * Application-defined routine for the USB CDC read operations
 *
 * \param[in] _dev       Handle of a valid CDC instance. Should be retrieved via \sa cdc_usbd_get_handle()
 * \param[in] _data      Pointer to data that should be sent to the host
 * \param[in] _len       Number of bytes to be written
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been written before a timeout occurs
 *
 */
#define _SERIAL_PORT_WRITE_DATA(_dev, _data, _len, _timeout)  write_to_cdc_usbd(_dev, _data, _len, _timeout)

/**
 * Application-defined routine for the USB flow control activation
 *
 * \note No USB flow control is supported on application level. All the basic flow control mechanisms
 *       are provided by the USB controller intrinsically and as defined by the USB protocol.
 */
#define _SERIAL_PORT_SET_FLOW_ON(_dev)

/**
 * Application-defined routine for the USB flow control de-activation
 *
 * \note No USB flow control is supported on application level. All the basic flow control mechanisms
 *       are provided by the USB controller intrinsically and as defined by the USB protocol.
 */
#define _SERIAL_PORT_SET_FLOW_OFF(_dev)

#endif /* DSPS_PORT_USBD_H_ */
