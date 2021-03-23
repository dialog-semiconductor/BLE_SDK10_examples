/**
 ****************************************************************************************
 *
 * @file ble_wechat_util.c
 *
 * @brief Wechat application utilities
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *
 ****************************************************************************************
 */

#include "ble_wechat_util.h"
#include "epb_MmBp.h"
#include "stdio.h"

data_handler first_handler = {PRODUCT_TYPE_UNKNOWN, NULL, NULL, NULL, NULL};

//function for getting handler by type
//find the type in the list structure of data handler then return the node pointer
data_handler* get_handler_by_type(int type)
{
		data_handler* handler = &first_handler;
		while(handler->next != NULL) {
				handler = handler->next;
				if (handler->m_product_type == type) {
						return handler;
				}
		}
		return NULL;
}
#define BigLittleSwap16(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                            (((uint16_t)(A) & 0x00ff) << 8))

 
#define BigLittleSwap32(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                            (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                            (((uint32_t)(A) & 0x0000ff00) << 8) | \
                            (((uint32_t)(A) & 0x000000ff) << 24))

int checkCPUendian()
{
       union{
              unsigned long i;
              uint8_t s[4];
       }c;
 
       c.i = 0x12345678;
       return (0x12 == c.s[0]);
}

unsigned long t_htonl(unsigned long h)
{
       return checkCPUendian() ? h : BigLittleSwap32(h);
}
 
unsigned long t_ntohl(unsigned long n)
{

       return checkCPUendian() ? n : BigLittleSwap32(n);
}

unsigned short htons(unsigned short h)
{
       return checkCPUendian() ? h : BigLittleSwap16(h);
}
 
unsigned short ntohs(unsigned short n)
{
       return checkCPUendian() ? n : BigLittleSwap16(n);
}


/**@brief Function for error handling, which is called when an error has occurred. 
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] p_data_handler where Error come from
 */
void wechat_error_chack(data_handler *p_data_handler, int error_code)
{
	switch(error_code)
	{
		case EEC_system:
		{
		#ifdef CATCH_LOG
			printf("\r\n! error: system error");
		#endif
		}
			break ;
		case EEC_needAuth:
		#ifdef CATCH_LOG
			printf("\r\n! error: needAuth");
		#endif
			break ;
		case EEC_sessionTimeout:
		#ifdef CATCH_LOG
			printf("\r\n! error: sessionTimeout");
		#endif
			break ;
		case EEC_decode:
		#ifdef CATCH_LOG
			printf("\r\n! error: decode");
		#endif
			break ;
		case EEC_deviceIsBlock:
		#ifdef CATCH_LOG
			printf("\r\n! error: deviceIsBlock");
		#endif
			break ;
		case EEC_serviceUnAvalibleInBackground:
		#ifdef CATCH_LOG
			printf("\r\n! error: serviceUnAvalibleInBackground");
		#endif
			break ;
		case EEC_deviceProtoVersionNeedUpdate:
		#ifdef CATCH_LOG
			printf("\r\n! error: deviceProtoVersionNeedUpdate");
		#endif
			break ;
		case EEC_phoneProtoVersionNeedUpdate:
		#ifdef CATCH_LOG
			printf("\r\n! error: phoneProtoVersionNeedUpdate");
		#endif
			break ;
		case EEC_maxReqInQueue:
		#ifdef CATCH_LOG
			printf("\r\n! error: maxReqInQueue");
		#endif
			break ;
		case EEC_userExitWxAccount:
		#ifdef CATCH_LOG
			printf("\r\n! error: userExitWxAccount");
		#endif
			break ;
		default:
			break ;
	}
	p_data_handler->m_data_error_func(error_code);
}


void data_handler_init(data_handler** p_data_handler, uint8_t product_type)
{
	if (*p_data_handler == NULL) 
		{
			*p_data_handler = get_handler_by_type(product_type);
		}
}


/*turn an unsigned short value to big-endian value					*/
/*for example 0x1234 in the memory of X86 is 0x34 and 0x12	*/
/*then turn it to Network Byte Order is 0x12 and 0x34				*/

