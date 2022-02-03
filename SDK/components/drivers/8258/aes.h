/********************************************************************************************************
 * @file     aes.h 
 *
 * @brief    This is the header file for TLSR8258
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

#ifndef AES_H_
#define AES_H_
#include "compiler.h"
/**
 * @brief       This function servers to perform aes_128 encryption for 16-Byte input data
 *              with specific 16-Byte key
 * @param[in]   Key the pointer to the 16-Byte Key
 * @param[in]   Data the pointer to the 16-Byte plain text
 * @param[out]  Result the pointer to the encryption result cipher text
 * @return      0: success;
 *              not 0: failure
 */
extern int aes_encrypt(unsigned char *Key, unsigned char *Data, unsigned char *Result);

#define  AES_Encrypt  aes_encrypt

/**
 * @brief       This function servers to perform aes_128 decryption for 16-Byte input data
 *              with specific 16-Byte key
 * @param[in]   Key the pointer to the 16-Byte Key
 * @param[in]   Data the pointer to the 16-Byte cipher text
 * @param[out]  Result the pointer to the decryption result plain text
 * @return      0: success;
 *              not 0: failure
 */
extern int aes_decrypt(unsigned char *Key, unsigned char *Data, unsigned char *Result);

#define  AES_Decrypt  aes_decrypt

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
extern int  aes_dma_encrypt(unsigned char *Key,unsigned long *Data,unsigned short DataSize, unsigned long *Result,unsigned short ResultSize);
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
extern int  aes_dma_decrypt(unsigned char *Key,unsigned long *Data,unsigned short DataSize, unsigned long *Result,unsigned short ResultSize);


#endif /* AES_H_ */
