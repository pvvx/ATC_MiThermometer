/********************************************************************************************************
 * @file     aes.c 
 *
 * @brief    This is the source file for TLSR8258
 *
 * @author	 Driver Group
 * @date     May 8, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 * @par      History:
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/

#include "aes.h"
#include "bsp.h"
#include "clock.h"
#include "register.h"
#include "timer.h"
/**
 * @brief       This function servers to perform aes_128 encryption for 16-Byte input data
 *              with specific 16-Byte key
 * @param[in]   Key the pointer to the 16-Byte Key
 * @param[in]   Data the pointer to the 16-Byte plain text
 * @param[out]  Result the pointer to the encryption result cipher text
 * @return      0: success;
 *              not 0: failure
 */
int aes_encrypt(unsigned char *Key, unsigned char *Data, unsigned char *Result)
{
    unsigned int tmp = 0;
    unsigned char *p = Data;
    unsigned char i = 0;

    //trig encrypt operation
    reg_aes_ctrl &= (~FLD_AES_CTRL_CODEC_TRIG);

    //set the key
    for (i = 0; i < 16; i++) {
    	reg_aes_key(i) = Key[i];
    }

    //feed the data
    while (reg_aes_ctrl & FLD_AES_CTRL_DATA_FEED) {
        tmp = p[0] + (p[1]<<8) + (p[2]<<16) + (p[3]<<24);
        reg_aes_data = tmp;
        p += 4;
    }

    //wait for aes ready
    while ((reg_aes_ctrl & FLD_AES_CTRL_CODEC_FINISHED) == 0);

    //read out the result
    p = Result;
    for (i = 0; i < 4; i++) {
        tmp = reg_aes_data;
        *p++ = tmp & 0xff;
        *p++ = (tmp>>8) & 0xff;
        *p++ = (tmp>>16) & 0xff;
        *p++ = (tmp>>24) & 0xff;
    }
    return 0;
}


/**
 * @brief       This function servers to perform aes_128 decryption for 16-Byte input data
 *              with specific 16-Byte key
 * @param[in]   Key the pointer to the 16-Byte Key
 * @param[in]   Data the pointer to the 16-Byte cipher text
 * @param[out]  Result the pointer to the decryption result plain text
 * @return      0: success;
 *              not 0: failure
 */
int aes_decrypt(unsigned char *Key, unsigned char *Data, unsigned char *Result)
{
    unsigned int tmp = 0;
    unsigned char *p = Data;
    unsigned char i = 0;

    //trig decrypt operation
    reg_aes_ctrl |= FLD_AES_CTRL_CODEC_TRIG;

    //set the key
    for (i = 0; i < 16; i++) {
    	reg_aes_key(i) = Key[i];
    }

    //feed the data
    while (reg_aes_ctrl & FLD_AES_CTRL_DATA_FEED) {
        tmp = p[0] + (p[1]<<8) + (p[2]<<16) + (p[3]<<24);
        reg_aes_data = tmp;
        p += 4;
    }

    //wait for aes ready
    while ((reg_aes_ctrl & FLD_AES_CTRL_CODEC_FINISHED) == 0);

    //read out the result
    p = Result;
    for (i = 0; i < 4; i++) {
        tmp = reg_aes_data;
        *p++ = tmp & 0xff;
        *p++ = (tmp>>8) & 0xff;
        *p++ = (tmp>>16) & 0xff;
        *p++ = (tmp>>24) & 0xff;
    }
    return 0;
}

/**
 * @brief       This function servers to perform aes_128 encryption
 *              with specific 16-Byte key in DMA MODE
 * @param[in]   Key :the pointer to the 16-Byte Key
 * @param[in]   Data :the pointer to the 16-Byte plain text
 * @param[in]   DataSize: the length of Data
 * @param[out]  ResultSize: the length of Result
 * @param[out]  Result: the pointer to the encryption cipher text
 * @return      0: success;
 *              not 0: failure
 */
int  aes_dma_encrypt(unsigned char *Key,unsigned long *Data,unsigned short DataSize, unsigned long *Result,unsigned short ResultSize)
{
	write_reg8(0xc10,((unsigned long) Result)&0xff);//set memory address low byte
	write_reg8(0xc11, (((unsigned long)Result)>>8)&0xff);//set memory address high byte
	reg_dma4_addrHi=0x04;
	reg_dma4_size =  ResultSize/16;//set memory size
	reg_dma4_mode = 0x01;

	write_reg8(0xc14,((unsigned long)Data)&0xff);//set memory address low byte
	write_reg8(0xc15, (((unsigned long)Data)>>8)&0xff);//set memory address high byte
	reg_dma5_addrHi=0x04;
	reg_dma5_size =DataSize/ 16;//set memory size
	reg_dma5_mode = 0x00;

	reg_aes_ctrl &= (~FLD_AES_CTRL_CODEC_TRIG);//trig encrypt operation

	for (unsigned char i = 0; i < 16; i++) {
		reg_aes_key(i) = Key[i];  //set the key
	}

	reg_dma_chn_en|=0x30;//enable aes dma channel
	reg_dma_tx_rdy0|=(FLD_DMA_CHN_AES_CODE|FLD_DMA_CHN_AES_DECO);

	while (( reg_aes_ctrl & BIT(2)) == 0);
	sleep_ms(2000);
	return 0;
}

/**
 * @brief       This function servers to perform aes_128  decryption
 *              with specific 16-Byte key in DMA MODE
 * @param[in]   Key :the pointer to the 16-Byte Key
 * @param[in]   Data :the pointer to the 16-Byte cipher text
 * @param[in]   DataSize: the length of Data
 * @param[out]  ResultSize: the length of Result
 * @param[out]  Result: the pointer to the decryption result plain text
 * @return      0: success;
 *              not 0: failure
 */
int aes_dma_decrypt(unsigned char *Key,unsigned long *Data,unsigned short DataSize, unsigned long *Result,unsigned short ResultSize)
{
	write_reg8(0xc10,((unsigned long) Result)&0xff);//set memory address low byte
	write_reg8(0xc11, (((unsigned long)Result)>>8)&0xff);//set memory address high byte
	reg_dma4_addrHi=0x04;
	reg_dma4_size =  ResultSize/16;//set memory size
	reg_dma4_mode = 0x01;

	write_reg8(0xc14,((unsigned long)Data)&0xff);//set memory address low byte
	write_reg8(0xc15, (((unsigned long)Data)>>8)&0xff);//set memory address high byte
	reg_dma5_addrHi=0x04;
	reg_dma5_size =DataSize/ 16;//set memory size
	reg_dma5_mode = 0x00;

	reg_aes_ctrl |=FLD_AES_CTRL_CODEC_TRIG;

	for (unsigned char i = 0; i < 16; i++) {
		reg_aes_key(i) = Key[i];  //set the key
	}

	reg_dma_chn_en|=0x30;//enable aes dma channel
	reg_dma_tx_rdy0|=(FLD_DMA_CHN_AES_CODE|FLD_DMA_CHN_AES_DECO);

	while (( reg_aes_ctrl & BIT(2)) == 0);
	sleep_us(2000000);
	return 0;
}

