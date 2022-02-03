/*
 * blt_fw_signature.c
 *
 *  Created on: 2019-5-5
 *      Author: Administrator
 */


#include "tl_common.h"
#include "drivers.h"
#include "blt_fw_sign.h"
#include "stack/ble/blt_config.h"
#include "proj_lib/firmware_encrypt.h"

void blt_firmware_signature_check(void)
{
		unsigned int flash_mid;
		unsigned char flash_uid[16];
		unsigned char signature_enc_key[16];
		int flag = flash_read_mid_uid_with_check(&flash_mid, flash_uid);

		if(flag==0){  //reading flash UID error
			while(1);
		}

		firmware_encrypt_based_on_uid (flash_uid, signature_enc_key);

		if(memcmp(signature_enc_key, (u8*)CUST_FIRMWARE_SIGNKEY_ADDR, 16)){  //signature not match
			while(1);   //user can change the code here to stop firmware running
		}
}
