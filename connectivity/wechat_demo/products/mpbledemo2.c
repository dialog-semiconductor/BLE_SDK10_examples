/*
*	WeChat mpbledemo2 
*
*	author by anqiren  2014/12/02  V1.0bat
*
**/
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "osal.h"
#include "hw_wkup.h"
#include "hw_gpio.h"

#include "epb_MmBp.h"
#include "ble_wechat_util.h"
#include "crc32.h"
#include "mpbledemo2.h"
#include "ble_wechat_config.h"

#include "crypto_hash_op.h"
#include "crypto_aes.h"

mpbledemo2_info         m_info          = {CMD_NULL, {NULL, 0}};

#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	uint8_t md5_type_and_id[MD5_TYPE_AND_ID_LENGTH];
#endif

#define Nb      4                                                // 以“字”（4字节）为单位的加解密数据块大小，固定为4

uint8_t challeange[CHALLENAGE_LENGTH]   = {0x11,0x22,0x33,0x44}; //为了方便这里填了一组确定的数组，使用过程中请自行生成随机数

mpbledemo2_state        mpbledemo2Sta   = {false, false, false, false, false, false,0,0,0};
const uint8_t           key[32]         = DEVICE_KEY
uint8_t                 session_key[32] = {0};
data_handler            *m_mpbledemo2_handler = NULL;

OS_TASK                 buttonEventHandle;

#define NOTIF_BUTTON_PRESSED_EVENT      (1 << 0)

// handler of the commands from wechat smartphone side
static void mpbledemo2_cmd_handler(uint16_t commandCode);
// API to send data to wechat smartphone sidee
static int32_t mpbledemo2_send_string(const char* ptrString, uint32_t lengthInBytes);

/* AES length must be multiple of 16bytes */
unsigned int my_AES_get_length(unsigned int length)
{
        if(length%16 == 0){
                return length;
        }
        else return ((length>>4) + 1)<<4;
}

/* For PKCS7 AES, proper padding is needed */
unsigned int AES_PKCS7_padding(unsigned char *pPlainText, unsigned int nDataLen)
{
        unsigned int length = nDataLen;
        nDataLen = ((nDataLen>>4) + 1)<<4;
        uint8_t p=(Nb<<2)-(length%(Nb<<2));
        unsigned char temp[16];
        memset(temp,p,16);
        if(length<nDataLen)
        {
                memcpy(pPlainText + length,temp,nDataLen - length);
        }
        return(nDataLen);
}

/**@brief   Function for the light initialization.
 *
 * @details Initializes all lights used by this application.
 */

int32_t mpbledemo2_get_md5(void)
{
        CRYPTO_HASH_RET hash_status = CRYPTO_HASH_RET_OK;
        crypto_hash_context_t hash_ctx;
#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	char device_type[] = DEVICE_TYPE;
	char device_id[] = DEVICE_ID;
	char argv[sizeof(DEVICE_TYPE) + sizeof(DEVICE_ID) - 1];
	memcpy(argv,device_type,sizeof(DEVICE_TYPE));
/*when add the DEVICE_ID to DEVICE_TYPE, the offset shuld -1 to overwrite '\0'  at the end of DEVICE_TYPE */
	memcpy(argv + sizeof(DEVICE_TYPE)-1,device_id,sizeof(DEVICE_ID));

	/* use HW crypto engine for MD5 operation */
        hash_status = crypto_hash_init_context(&hash_ctx, CRYPTO_HASH_MD5, (uint8_t *)argv, md5_type_and_id, sizeof(argv) - 1);
        hash_status = crypto_hash_compute(&hash_ctx, MD5_TYPE_AND_ID_LENGTH, 100);
#endif
	return hash_status;
}

void mpbledemo2_reset(void)
{
	mpbledemo2Sta.auth_send = false;
	mpbledemo2Sta.auth_state = false;
	mpbledemo2Sta.indication_state = false;
	mpbledemo2Sta.init_send = false;
	mpbledemo2Sta.init_state = false;
	mpbledemo2Sta.send_data_seq = 0;
	mpbledemo2Sta.push_data_seq = 0;
	mpbledemo2Sta.seq = 0;
}
    
int32_t mpbledemo2_button_handler(uint8_t pin_no);

static void button_event_task(void *params)
{
        for (;;) {
                BaseType_t ret;
                uint32_t notif;

                ret = OS_TASK_NOTIFY_WAIT(0, (uint32_t)-1, &notif, portMAX_DELAY);
                configASSERT(ret == pdTRUE);

                if (notif & NOTIF_BUTTON_PRESSED_EVENT) {
                        // check pending sensor data reading requests
                        mpbledemo2_button_handler(MPBLEDEMO2_BUTTON_1);
                }
        }
}

static void gpio_button_handler(void)
{
        hw_wkup_reset_interrupt();
        OS_TASK_NOTIFY_FROM_ISR(buttonEventHandle,
                        NOTIF_BUTTON_PRESSED_EVENT,
                        OS_NOTIFY_SET_BITS);
}

int32_t mpbledemo2_init(void)
{
	mpbledemo2_reset();

	OS_BASE_TYPE status __attribute__((unused));
        status = OS_TASK_CREATE("event handler",                /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                                button_event_task,              /* The System Initialization task. */
                                ( void * ) 0,             /* The parameter passed to the task. */
                                1024,                     /* The number of bytes to allocate to the
                                                             stack of the task. */
                                OS_TASK_PRIORITY_NORMAL, /* The priority assigned to the task. */
                                buttonEventHandle );                 /* The task handle */

        hw_wkup_init(NULL);
        hw_wkup_set_debounce_time(32);
        hw_wkup_configure_pin(CFG_KEY_TRIGGER_GPIO_PORT,
                                CFG_KEY_TRIGGER_GPIO_PIN, true, HW_WKUP_PIN_STATE_LOW);
        hw_wkup_register_key_interrupt(gpio_button_handler, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);

        hw_wkup_enable_irq();

	return (mpbledemo2_get_md5());
}

int32_t mpbledemo2_init_peripheral_func(void)
{
        return 0;
}    

void mpbledemo2_indication_state(bool isEnable)
{
        mpbledemo2Sta.indication_state = isEnable;
}

void light_on(void)
{

}

void light_off(void)
{

}

static void mpbledemo2_cmd_handler(uint16_t commandCode)
{
        switch (commandCode)
        {
                case openLightPush:
                {
                #ifdef CATCH_LOG
                        printf("\r\n light on!! ");
                #endif
                        light_on();
                        break;
                }
                case closeLightPush:
                {
                #ifdef CATCH_LOG
                        printf("\r\n light off!! ");
                #endif
                        light_off();
                        break;
                }
        }
}

static int32_t mpbledemo2_send_string(const char* ptrString, uint32_t lengthInBytes)
{
        uint8_t *data = NULL;
        uint32_t len = 0;
        ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_SENDDAT);
        ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.len, lengthInBytes);
        ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.str, ptrString);

        m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
        if (data == NULL)
        {
                return errorCodeProduce;
        }
        ble_wechat_indicate_data(data, len);
        return 0;
}


/**@brief Function for handling button pull events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
int32_t mpbledemo2_button_handler(uint8_t pin_no)
{
        if (m_mpbledemo2_handler == NULL)
        {
                m_mpbledemo2_handler = get_handler_by_type(PRODUCT_TYPE_MPBLEDEMO2);
        }

	switch (pin_no)
	{
                case MPBLEDEMO2_BUTTON_1:
                {
	        #ifdef CATCH_LOG
                        printf("\r\n button 1");
	        #endif
                        mpbledemo2_send_string(SEND_HELLO_WECHAT, sizeof(SEND_HELLO_WECHAT));
                }
                break;
                default:
                {
                        break;
                }
	}

	return 0;
}

int32_t device_auth(void)
{	
	if (m_mpbledemo2_handler == NULL)
	{
		m_mpbledemo2_handler = get_handler_by_type(PRODUCT_TYPE_MPBLEDEMO2);
	}

	uint8_t *data = NULL;
	uint32_t len = 0;
		
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_AUTH);
	m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	if (data == NULL)
	{
		return errorCodeProduce;
	}
        
	//sent data
		
	#ifdef CATCH_LOG
		printf("\r\n auth send! ");
	#endif

	#ifdef CATCH_LOG
		printf("\r\n##send data: ");
		uint8_t *d = data;
		for(uint8_t i=0;i<len;++i){
		printf(" %x",d[i]);}
		BpFixHead *fix_head = (BpFixHead *)data;
		printf("\r\n CMDID: %d", ntohs(fix_head->nCmdId));
		printf("\r\n len: %d", ntohs(fix_head->nLength ));
		printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
	#endif

	ble_wechat_indicate_data(data, len);
	return 0;
}
	
int32_t	device_init(void)
{
	uint8_t *data = NULL;
	uint32_t len = 0;
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_INIT);
	m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	if (data == NULL)
	{
		return errorCodeProduce;
	}
        
	//sent data
	ble_wechat_indicate_data(data, len);
	#ifdef CATCH_LOG
		printf("\r\n init send! ");
	#endif

	#ifdef CATCH_LOG
		printf("\r\n##send data: ");
		uint8_t *d = data;
		for(uint8_t i=0;i<len;++i){
		printf(" %x",d[i]);}
		BpFixHead *fix_head = (BpFixHead *)data;
		printf("\r\n CMDID: %d", ntohs(fix_head->nCmdId));
		printf("\r\n len: %d", ntohs(fix_head->nLength ));
		printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
	#endif
	return 0;
}

void mpbledemo2_main_process(void)
{
	if ((mpbledemo2Sta.indication_state) && (!mpbledemo2Sta.auth_state) && (!mpbledemo2Sta.auth_send) )
	{
		device_auth();
		mpbledemo2Sta.auth_send = true;	
	}
	if ((mpbledemo2Sta.auth_state) && (!mpbledemo2Sta.init_state) && (!mpbledemo2Sta.init_send))
	{
		device_init();
		mpbledemo2Sta.init_send = true;
	}
}

void mpbledemo2_data_free_func(uint8_t *data, uint32_t len)
{
	if (data)
	{
		OS_FREE(data);
		data = NULL;
	}
}

void mpbledemo2_data_produce_func(void *args, uint8_t **r_data, uint32_t *r_len)
{
		static uint16_t bleDemoHeadLen = sizeof(BlueDemoHead);
		mpbledemo2_info *info = (mpbledemo2_info *)args;
		BaseRequest basReq = {NULL};
		static uint8_t fix_head_len = sizeof(BpFixHead);
		BpFixHead fix_head = {0xFE, 1, 0, htons(ECI_req_auth), 0};
		mpbledemo2Sta.seq++;
		switch (info->cmd)
		{
		        case CMD_AUTH:
		        {
		        #if defined EAM_md5AndAesEnrypt
                                uint8_t deviceid[] = DEVICE_ID;
                                static uint32_t seq = 0x00000001;//
                                uint32_t ran = 0x11223344;//为了方便起见这里放了一个固定值做为随机数，在使用时请自行生成随机数。
                                ran = t_htonl(ran);
                                seq = t_htonl(seq);
                                uint8_t id_len = strlen(DEVICE_ID);
                                uint8_t* data = OS_MALLOC(id_len+8);
                                if(!data){printf("\r\nNot enough memory!");return;}
                                memcpy(data,deviceid,id_len);
                                memcpy(data+id_len,(uint8_t*)&ran,4);
                                memcpy(data+id_len+4,(uint8_t*)&seq,4);
                                uint32_t crc = crc32(0, data, id_len+8);
                                crc = t_htonl(crc);
                                memset(data,0x00,id_len+8);
                                memcpy(data,(uint8_t*)&ran,4);
                                memcpy(data+4,(uint8_t*)&seq,4);
                                memcpy(data+8,(uint8_t*)&crc,4);

                                /* Use HW engine for AES operation */
                                uint8_t CipherText[CIPHER_TEXT_LENGTH];
                                uint16_t AES_len = AES_PKCS7_padding(data, 12);
                                crypto_aes_context_t aes_ctx;
                                CRYPTO_AES_RET aes_status;
                                aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                        (uint8_t *)key, (uint8_t *)key, NULL, data, CipherText, AES_len);
                                aes_status =  crypto_aes_encrypt(&aes_ctx, AES_len, 500);
                                if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return;}

//                                AES_Init(key);
//                                AES_Encrypt_PKCS7 (data, CipherText, 12, key);

                                if(data){OS_FREE(data);data = NULL;}
                                AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, true ,{CipherText, CIPHER_TEXT_LENGTH}, false, {NULL, 0}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
                                seq++;
                        #endif
				
			#if defined EAM_macNoEncrypt
				static uint8_t mac_address[MAC_ADDRESS_LENGTH];
				get_mac_addr(mac_address);
				AuthRequest authReq = {&basReq, false,{NULL, 0}, PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false,{NULL, 0}, true, {mac_address, MAC_ADDRESS_LENGTH}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				
			#if defined EAM_md5AndNoEnrypt
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, PROTO_VERSION, (EmAuthMethod)AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false ,{NULL, 0}, false, {NULL, 0}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				*r_len = epb_auth_request_pack_size(&authReq) + fix_head_len;
				*r_data = (uint8_t *)OS_MALLOC(*r_len);
				if(!(*r_data)){printf("\r\nNot enough memory!");return;}
				if(epb_pack_auth_request(&authReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
					return;
				}
				fix_head.nCmdId = htons(ECI_req_auth);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				return ;
		        }

		        case CMD_INIT:
			{
				//has challenge
				InitRequest initReq = {&basReq,false, {NULL, 0},true, {challeange, CHALLENAGE_LENGTH}};
				*r_len = epb_init_request_pack_size(&initReq) + fix_head_len;
			#if defined EAM_md5AndAesEnrypt
				uint8_t length = *r_len;				
				uint8_t *p = OS_MALLOC(my_AES_get_length( *r_len-fix_head_len));
				if(!p){printf("\r\nNot enough memory!");return;}
				*r_len = my_AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
				//pack data
				*r_data = (uint8_t *)OS_MALLOC(*r_len);
				if(!(*r_data)){printf("\r\nNot enough memory!");return;}
				if(epb_pack_init_request(&initReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{*r_data = NULL;return;}
				//encrypt body
			#if defined EAM_md5AndAesEnrypt
				/* Use HW engine for AES operation */
				crypto_aes_context_t aes_ctx;
                                CRYPTO_AES_RET aes_status;
                                uint16_t AES_len = AES_PKCS7_padding(*r_data+fix_head_len, length-fix_head_len);
                                aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                        session_key, session_key, NULL, *r_data+fix_head_len, p, AES_len);
	                        aes_status =  crypto_aes_encrypt(&aes_ctx, AES_len, 500);
	                        if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return;}

				//AES_Init(session_key);
				//AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度

				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p)OS_FREE(p);
			#endif
				fix_head.nCmdId = htons(ECI_req_init);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				return  ;
			}

		        case CMD_SENDDAT:
			{
			#ifdef CATCH_LOG
				printf("\r\n msg to send : %s",(uint8_t*)info->send_msg.str);
			#endif
				BlueDemoHead  *bleDemoHead = (BlueDemoHead*)OS_MALLOC(bleDemoHeadLen+info->send_msg.len);
				if(!bleDemoHead){printf("\r\nNot enough memory!");return;}
				bleDemoHead->m_magicCode[0] = MPBLEDEMO2_MAGICCODE_H;
				bleDemoHead->m_magicCode[1] = MPBLEDEMO2_MAGICCODE_L;
				bleDemoHead->m_version = htons( MPBLEDEMO2_VERSION);
				bleDemoHead->m_totalLength = htons(bleDemoHeadLen + info->send_msg.len);
				bleDemoHead->m_cmdid = htons(sendTextReq);
				bleDemoHead->m_seq = htons(mpbledemo2Sta.seq);
				bleDemoHead->m_errorCode = 0;	
				/*connect body and head.*/
				/*turn to uint8_t* befort offset.*/
				memcpy((uint8_t*)bleDemoHead+bleDemoHeadLen, info->send_msg.str, info->send_msg.len);			
				SendDataRequest sendDatReq = {&basReq, {(uint8_t*) bleDemoHead, (bleDemoHeadLen + info->send_msg.len)}, false, (EmDeviceDataType)NULL};
				*r_len = epb_send_data_request_pack_size(&sendDatReq) + fix_head_len;
			#if defined EAM_md5AndAesEnrypt
				uint16_t length = *r_len;
				uint8_t *p = OS_MALLOC(my_AES_get_length( *r_len-fix_head_len));
				if(!p){printf("\r\nNot enough memory!");return;}
				*r_len = my_AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
				*r_data = (uint8_t *)OS_MALLOC(*r_len);
				if(!(*r_data)){printf("\r\nNot enough memory!");return;}
				if(epb_pack_send_data_request(&sendDatReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
				#if defined EAM_md5AndAesEnrypt
					if(p){OS_FREE(p);
					p = NULL;}
					#endif
					printf("\r\nepb_pack_send_data_request error!");
					return;
				}
			#if defined EAM_md5AndAesEnrypt
				//encrypt body
                                /* Use HW engine for AES operation */
                                crypto_aes_context_t aes_ctx;
                                CRYPTO_AES_RET aes_status;
                                uint16_t AES_len = AES_PKCS7_padding(*r_data+fix_head_len, length-fix_head_len);
                                aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                        session_key, session_key, NULL, *r_data+fix_head_len, p, AES_len);
                                aes_status =  crypto_aes_encrypt(&aes_ctx, AES_len, 500);
                                if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return;}

				//AES_Init(session_key);
				//AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度

				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p){OS_FREE(p); p = NULL;}
			#endif
				fix_head.nCmdId = htons(ECI_req_sendData);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				if(bleDemoHead){OS_FREE(bleDemoHead);bleDemoHead = NULL;}
			#ifdef CATCH_LOG
				printf("\r\n##send data: ");
				uint8_t *d = *r_data;
				for(uint8_t i=0;i<*r_len;++i){
				printf(" %x",d[i]);}
				BpFixHead *fix_head = (BpFixHead *)*r_data;
				printf("\r\n CMDID: %d",ntohs(fix_head->nCmdId));
				printf("\r\n len: %d", ntohs(fix_head->nLength ));
				printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
			#endif
			mpbledemo2Sta.send_data_seq++;
				
				return ;
		}
	}	
}

int32_t mpbledemo2_data_consume_func(uint8_t *data, uint32_t len)
{
		BpFixHead *fix_head = (BpFixHead *)data;
		uint8_t fix_head_len = sizeof(BpFixHead);
	#ifdef CATCH_LOG
		printf("\r\n##Received data: ");
		uint8_t *d = data;
		for(uint8_t i=0;i<len;++i){
		printf(" %x",d[i]);}
		printf("\r\n CMDID: %d", ntohs(fix_head->nCmdId));
		printf("\r\n len: %d", ntohs(fix_head->nLength));
		printf("\r\n Seq: %d",ntohs(fix_head->nSeq));
	#endif
		switch(ntohs(fix_head->nCmdId))
		{
			case ECI_none:
			{
			}
			break;
			case ECI_resp_auth:
			{
					AuthResponse* authResp;
					authResp = epb_unpack_auth_response(data+fix_head_len,len-fix_head_len);
				#ifdef CATCH_LOG
					printf("\r\n@@Received 'authResp'\r\n");
				#endif
					if(!authResp){return errorCodeUnpackAuthResp;}
				#ifdef CATCH_LOG
					printf("\r\n unpack 'authResp' success!\r\n");
				#endif
					if(authResp->base_response)
					{
						if(authResp->base_response->err_code == 0)
						{
							mpbledemo2Sta.auth_state = true;

						}
						else
						{
						#ifdef CATCH_LOG
							printf("\r\n error code:%ld",authResp->base_response->err_code);
						#endif
							#ifdef CATCH_LOG
							if(authResp->base_response->has_err_msg)
							{
								printf("\r\n base_response error msg:%s",authResp->base_response->err_msg.str);	
							}
							#endif
							epb_unpack_auth_response_free(authResp);
							return authResp->base_response->err_code;
						}
					}
				#if defined EAM_md5AndAesEnrypt// get sessionkey
					if(authResp->aes_session_key.len)
					{
					#ifdef CATCH_LOG
						printf("\r\nsession_key:");
					#endif
		                                /* Use HW engine for AES operation */
		                                crypto_aes_context_t aes_ctx;
		                                CRYPTO_AES_RET aes_status;
		                                aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
		                                        (uint8_t *)key,  (uint8_t *)key, NULL, session_key, (uint8_t *)authResp->aes_session_key.data, authResp->aes_session_key.len);
		                                aes_status =  crypto_aes_decrypt(&aes_ctx, authResp->aes_session_key.len, 500);
		                                if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return aes_status;}

		                                //AES_Init(key);
						//AES_Decrypt(session_key,authResp->aes_session_key.data,authResp->aes_session_key.len,key);

						#ifdef CATCH_LOG
						for(uint8_t i = 0;i<16;i++)
						{
							printf(" 0x%02x",session_key[i]);	
						}
						#endif
					}
				#endif
					epb_unpack_auth_response_free(authResp);
			}
			break;
			case ECI_resp_sendData:
			{
				
			#ifdef CATCH_LOG
				printf("\r\n@@Received 'sendDataResp'\r\n");
			#endif
			#if defined EAM_md5AndAesEnrypt		
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = OS_MALLOC (length);
					if(!p){printf("\r\nNot enough memory!"); if(data)OS_FREE(data);data = NULL; return 0;}
                                        /* Use HW engine for AES operation */
                                        crypto_aes_context_t aes_ctx;
                                        CRYPTO_AES_RET aes_status;
                                        uint16_t AES_len = my_AES_get_length(len- fix_head_len);
                                        aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                                session_key, session_key, NULL, p, data+fix_head_len, AES_len);
                                        aes_status =  crypto_aes_decrypt(&aes_ctx, AES_len, 500);
                                        if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return aes_status;}

                                        //AES_Init(session_key);
					//AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){OS_FREE(p);p = NULL;}
			#endif	
				SendDataResponse *sendDataResp;
				sendDataResp = epb_unpack_send_data_response(data+fix_head_len,len-fix_head_len);
				if(!sendDataResp)
					{
						return errorCodeUnpackSendDataResp;
					}
			#ifdef CATCH_LOG
				BlueDemoHead *bledemohead = (BlueDemoHead*)sendDataResp->data.data;
				if(ntohs(bledemohead->m_cmdid) == sendTextResp)
					{
						printf("\r\n received msg: %s\r\n",sendDataResp->data.data+sizeof(BlueDemoHead));
					}
			#endif
					if(sendDataResp->base_response->err_code)
					{
						epb_unpack_send_data_response_free(sendDataResp);
						return sendDataResp->base_response->err_code;
					}
					epb_unpack_send_data_response_free(sendDataResp);
			}
			break;
			case ECI_resp_init:
			{
			#ifdef CATCH_LOG
				printf("\r\n@@Received 'initResp'\r\n");
			#endif
			#if defined EAM_md5AndAesEnrypt		
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = OS_MALLOC (length);
					if(!p){printf("\r\nNot enough memory!");if(data)OS_FREE(data);data = NULL; return 0;}
                                        /* Use HW engine for AES operation */
                                        crypto_aes_context_t aes_ctx;
                                        CRYPTO_AES_RET aes_status;
					uint16_t AES_len = my_AES_get_length(len- fix_head_len);
                                        aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                                session_key, session_key, NULL, p, data+fix_head_len, AES_len);
                                        aes_status =  crypto_aes_decrypt(&aes_ctx, AES_len, 500);
                                        if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return aes_status;}

                                        //AES_Init(session_key);
					//AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){OS_FREE(p);p = NULL;}
			#endif		
				InitResponse *initResp = epb_unpack_init_response(data+fix_head_len, len-fix_head_len);
				if(!initResp)
				{
					return errorCodeUnpackInitResp;
				}
				#ifdef CATCH_LOG
					printf("\r\n unpack 'initResp' success!");
				#endif	
					if(initResp->base_response)
					{
						if(initResp->base_response->err_code == 0)
						{
							if(initResp->has_challeange_answer)
							{
								if(crc32(0,challeange,CHALLENAGE_LENGTH) == initResp->challeange_answer)
								{
									mpbledemo2Sta.init_state = true;
								}
							}
							else mpbledemo2Sta.init_state = true;
							mpbledemo2Sta.wechats_switch_state = true;
						}
						else
						{
						#ifdef CATCH_LOG
							printf("\r\n error code:%ld",initResp->base_response->err_code);
						#endif	
							if(initResp->base_response->has_err_msg)
							{
							#ifdef CATCH_LOG
								printf("\r\n base_response error msg:%s",initResp->base_response->err_msg.str);
							#endif	
							}
							epb_unpack_init_response_free(initResp);
							return initResp->base_response->err_code;
						}
					}
				epb_unpack_init_response_free(initResp);
			}
				break;
			case ECI_push_recvData:
				{
				#if defined EAM_md5AndAesEnrypt
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = OS_MALLOC (length);
					if(!p){printf("\r\nNot enough memory!");if(data)OS_FREE(data); data =NULL; return 0;}
                                        /* Use HW engine for AES operation */
                                        crypto_aes_context_t aes_ctx;
                                        CRYPTO_AES_RET aes_status;
                                        uint16_t AES_len = my_AES_get_length(len- fix_head_len);
                                        aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                                session_key, session_key, NULL, p, data+fix_head_len, AES_len);
                                        aes_status =  crypto_aes_decrypt(&aes_ctx, AES_len, 500);
                                        if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return aes_status;}

                                        //AES_Init(session_key);
					//AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){OS_FREE(p);p = NULL;}
				#endif
			RecvDataPush *recvDatPush;
			recvDatPush = epb_unpack_recv_data_push(data+fix_head_len, len-fix_head_len);
		#ifdef CATCH_LOG
			printf("\r\n@@Received 'recvDataPush'\r\n");
		#endif
			if(!recvDatPush)
			{
				return errorCodeUnpackRecvDataPush;
			}
		#ifdef CATCH_LOG
			printf("\r\n unpack the 'recvDataPush' successfully! \r\n");
			if(recvDatPush->base_push == NULL)
			{
				printf("\r\n recvDatPush->base_push is NULL! \r\n");
			}
			else 
			{
				printf("\r\n recvDatPush->base_push is not NULL! \r\n");
			}
			printf("\r\n recvDatPush->data.len: %x \r\n",recvDatPush->data.len);
			printf("\r\n recvDatPush->data.data:  \r\n");
			const uint8_t *d = recvDatPush->data.data;
			for(uint8_t i=0;i<recvDatPush->data.len;++i){
			printf(" %x",d[i]);}
			if(recvDatPush->has_type)
			{
				printf("\r\n recvDatPush has type! \r\n");
				printf("\r\n type: %d\r\n",recvDatPush->type);
			}
		#endif	
			BlueDemoHead *bledemohead = (BlueDemoHead*)recvDatPush->data.data;
		#ifdef CATCH_LOG
			printf("\r\n magicCode: %x",bledemohead->m_magicCode[0]);
			printf(" %x",bledemohead->m_magicCode[1]);
			printf("\r\n version: %x",ntohs(bledemohead->m_version));
			printf("\r\n totalLength: %x",ntohs(bledemohead->m_totalLength));
			printf("\r\n cmdid: %x",ntohs(bledemohead->m_cmdid ));
			printf("\r\n errorCode: %x",ntohs(bledemohead->m_errorCode));
		#endif	
			// command handler
			mpbledemo2_cmd_handler(ntohs(bledemohead->m_cmdid));
			epb_unpack_recv_data_push_free(recvDatPush);
			mpbledemo2Sta.push_data_seq++;
		}
				break;
			case ECI_push_switchView:
				{
			
					mpbledemo2Sta.wechats_switch_state = !mpbledemo2Sta.wechats_switch_state;
			#ifdef CATCH_LOG
				printf("\r\n@@Received 'switchViewPush'\r\n");
			#endif
			#if defined EAM_md5AndAesEnrypt		
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = OS_MALLOC (length);
					if(!p){printf("\r\nNot enough memory!");if(data)OS_FREE(data);data = NULL; return 0;}
                                        /* Use HW engine for AES operation */
                                        crypto_aes_context_t aes_ctx;
                                        CRYPTO_AES_RET aes_status;
                                        uint16_t AES_len = my_AES_get_length(len- fix_head_len);
                                        aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                                session_key, session_key, NULL, p, data+fix_head_len, AES_len);
                                        aes_status =  crypto_aes_decrypt(&aes_ctx, AES_len, 500);
                                        if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return aes_status;}

					//AES_Init(session_key);
					//AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){OS_FREE(p);p = NULL;}
			#endif		
				SwitchViewPush *swichViewPush;
				swichViewPush = epb_unpack_switch_view_push(data+fix_head_len,len-fix_head_len);
				if(!swichViewPush)
				{
					return errorCodeUnpackSwitchViewPush;
				}
				epb_unpack_switch_view_push_free(swichViewPush);
			}
				break;
			case ECI_push_switchBackgroud:
				{
			#ifdef CATCH_LOG
				printf("\r\n@@Received 'switchBackgroudPush'\r\n");
			#endif
			#if defined EAM_md5AndAesEnrypt
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = OS_MALLOC (length);
					if(!p){printf("\r\nNot enough memory!");if(data)OS_FREE(data);data = NULL;  return 0;}
                                        /* Use HW engine for AES operation */
                                        crypto_aes_context_t aes_ctx;
                                        CRYPTO_AES_RET aes_status;
                                        uint16_t AES_len = my_AES_get_length(len- fix_head_len);
                                        aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CBC, HW_AES_128,
                                                session_key, session_key, NULL, p, data+fix_head_len, AES_len);
                                        aes_status =  crypto_aes_decrypt(&aes_ctx, AES_len, 500);
                                        if(aes_status != CRYPTO_AES_RET_OK) {printf("\r\nAES error!");return aes_status;}

                                        //AES_Init(session_key);
					//AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);

					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(data){OS_FREE(p);p = NULL;}
			#endif
				SwitchBackgroudPush *switchBackgroundPush = epb_unpack_switch_backgroud_push(data+fix_head_len,len-fix_head_len);
				if(! switchBackgroundPush)
				{
					return errorCodeUnpackSwitchBackgroundPush;
				}	
				epb_unpack_switch_backgroud_push_free(switchBackgroundPush);
			}
				break;
			case ECI_err_decode:
				break;
			default:
				{
		#ifdef CATCH_LOG
			printf("\r\n !!ERROR CMDID:%d",ntohs(fix_head->nCmdId));
		#endif
			}
				break;
		}
		return 0;
}

void mpbledemo2_data_error_func(int32_t error_code)
{
	if(error_code)
	{
		#ifdef CATCH_LOG
			printf("\r\n! error: mpbledemo2 reseted");
		#endif
        
		NVIC_SystemReset();
	}
}

data_handler mpbledemo2_data_handler = {
		.m_product_type                 = PRODUCT_TYPE_MPBLEDEMO2,
		.m_data_produce_func            = &mpbledemo2_data_produce_func,
		.m_data_free_func               = &mpbledemo2_data_free_func,
		.m_data_consume_func            = &mpbledemo2_data_consume_func,
		.m_data_error_func              = &mpbledemo2_data_error_func,
		.m_data_init_peripheral_func    = &mpbledemo2_init_peripheral_func,
		.m_data_init_func               = &mpbledemo2_init,
		.m_data_main_process_func       = &mpbledemo2_main_process,
		.m_data_button_handler_func     = &mpbledemo2_button_handler,
		.m_data_produce_args            = &m_info,
		.next                           = NULL
};
