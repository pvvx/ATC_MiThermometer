/*
 *  NIST SP800-38C compliant CCM implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * Definition of CCM:
 * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
 * RFC 3610 "Counter with CBC-MAC (CCM)"
 *
 * Related:
 * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
 */
//#include "ccm.h"
#include <stdint.h>
#include "tl_common.h"
#if USE_MIHOME_BEACON
#include "stack/ble/crypt/aes/aes_att.h"

#define CCM_ENCRYPT 0
#define CCM_DECRYPT 1

/*
 * Macros for common operations.
 * Results in smaller compiled code than static inline functions.
 */

/*
 * Update the CBC-MAC state in y using a block in b
 * (Always using b as the source helps the compiler optimise a bit better.)
 */
#define UPDATE_CBC_MAC          \
    for( i = 0; i < 16; i++ )  	\
        y[i] ^= b[i];           \
    tn_aes_128((u8*)key, y, y);

/*
 * Encrypt or decrypt a partial block with CTR
 * Warning: using b for temporary storage! src and dst must not be b!
 * This avoids allocating one more 16 bytes buffer while allowing src == dst.
 */
#define CTR_CRYPT(dst, src, len)	\
    tn_aes_128((u8*) key, ctr, b);	\
    for( i = 0; i < len; i++ )		\
        dst[i] = src[i] ^ b[i];

/*
 * Authenticated encryption or decryption
 */
static int ccm_auth_crypt( int mode, const unsigned char *key,
                           const unsigned char *iv, size_t iv_len,
                           const unsigned char *add, size_t add_len,
                           const unsigned char *input, size_t length,
                           unsigned char *output,
                           unsigned char *tag, size_t tag_len )
{
    unsigned char i;
    unsigned char q;
    size_t len_left;
    unsigned char b[16];
    unsigned char y[16];
    unsigned char ctr[16];
    const unsigned char *src;
    unsigned char *dst;
#if 0
    /*
     * Check length requirements: SP800-38C A.1
     * Additional requirement: a < 2^16 - 2^8 to simplify the code.
     * 'length' checked later (when writing it to the first block)
     */
    if( tag_len < 4 || tag_len > 16 || tag_len % 2 != 0 )
        return(-1);

    /* Also implies q is within bounds */
    if( iv_len < 7 || iv_len > 13 )
        return(-1);

    if( add_len > 0xFF00 )
        return(-1);
#endif
    q = 16 - 1 - (unsigned char) iv_len;
    /*
     * First block B_0:
     * 0        .. 0        flags
     * 1        .. iv_len   nonce (aka iv)
     * iv_len+1 .. 15       length
     *
     * With flags as (bits):
     * 7        0
     * 6        add present?
     * 5 .. 3   (t - 2) / 2
     * 2 .. 0   q - 1
     */
    b[0] = 0;
    b[0] |= ( add_len > 0 ) << 6;
    b[0] |= ( ( tag_len - 2 ) / 2 ) << 3;
    b[0] |= q - 1;
    memcpy( b + 1, iv, iv_len );
    for( i = 0, len_left = length; i < q; i++, len_left >>= 8 )
        b[15-i] = (unsigned char)( len_left & 0xFF );
    if( len_left > 0 )
        return(-1);
    /* Start CBC-MAC with first block */
    memset( y, 0, 16 );
    UPDATE_CBC_MAC;
    /*
     * If there is additional data, update CBC-MAC with
     * add_len, add, 0 (padding to a block boundary)
     */
    if( add_len > 0 )
    {
        size_t use_len;
        len_left = add_len;
        src = add;
        memset( b, 0, 16 );
        b[0] = (unsigned char)( ( add_len >> 8 ) & 0xFF );
        b[1] = (unsigned char)( ( add_len      ) & 0xFF );
        use_len = len_left < 16 - 2 ? len_left : 16 - 2;
        memcpy( b + 2, src, use_len );
        len_left -= use_len;
        src += use_len;
        UPDATE_CBC_MAC;
        while( len_left > 0 )
        {
            use_len = len_left > 16 ? 16 : len_left;
            memset( b, 0, 16 );
            memcpy( b, src, use_len );
            UPDATE_CBC_MAC;
            len_left -= use_len;
            src += use_len;
        }
    }
    /*
     * Prepare counter block for encryption:
     * 0        .. 0        flags
     * 1        .. iv_len   nonce (aka iv)
     * iv_len+1 .. 15       counter (initially 1)
     *
     * With flags as (bits):
     * 7 .. 3   0
     * 2 .. 0   q - 1
     */
    ctr[0] = q - 1;
    memcpy( ctr + 1, iv, iv_len );
    memset( ctr + 1 + iv_len, 0, q );
    ctr[15] = 1;
    /*
     * Authenticate and {en,de}crypt the message.
     *
     * The only difference between encryption and decryption is
     * the respective order of authentication and {en,de}cryption.
     */
    len_left = length;
    src = input;
    dst = output;
    while( len_left > 0 )
    {
        size_t use_len = len_left > 16 ? 16 : len_left;
        if( mode == CCM_ENCRYPT )
        {
            memset( b, 0, 16 );
            memcpy( b, src, use_len );
            UPDATE_CBC_MAC;
        }
        CTR_CRYPT( dst, src, use_len );
        if( mode == CCM_DECRYPT )
        {
            memset( b, 0, 16 );
            memcpy( b, dst, use_len );
            UPDATE_CBC_MAC;
        }
        dst += use_len;
        src += use_len;
        len_left -= use_len;
        /*
         * Increment counter.
         * No need to check for overflow thanks to the length check above.
         */
        for( i = 0; i < q; i++ )
            if( ++ctr[15-i] != 0 )
                break;
    }
    /*
     * Authentication: reset counter and crypt/mask internal tag
     */
    for( i = 0; i < q; i++ )
        ctr[15-i] = 0;
    CTR_CRYPT( y, y, 16 );
    memcpy( tag, y, tag_len );
    return( 0 );
}

/*
 * Authenticated encryption
 */
int aes_ccm_encrypt_and_tag( const unsigned char *key, 
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *add, size_t add_len,
                         const unsigned char *input, size_t length,
                         unsigned char *output,
                         unsigned char *tag, size_t tag_len )
{
    return( ccm_auth_crypt( CCM_ENCRYPT, key, iv, iv_len,
                            add, add_len, input, length, output, tag, tag_len ) );
}

/* Implementation that should never be optimized out by the compiler */
inline void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}


/*
 * Authenticated decryption
 */
int aes_ccm_auth_decrypt( const unsigned char *key, 
                      const unsigned char *iv, size_t iv_len,
                      const unsigned char *add, size_t add_len,
                      const unsigned char *input, size_t length,
                      unsigned char *output,
                      const unsigned char *tag, size_t tag_len )
{
    int ret;
    unsigned char check_tag[16];
    unsigned char i;
    int diff;
    if( ( ret = ccm_auth_crypt( CCM_DECRYPT, key,
                                iv, iv_len, add, add_len,
                                input, length, output, check_tag, tag_len ) ) != 0 )
    {
        return(ret);
    }

    /* Check tag in "constant-time" */
    for( diff = 0, i = 0; i < tag_len; i++ )
        diff |= tag[i] ^ check_tag[i];

    if( diff != 0 )
    {
        volatile unsigned char *p = output; while(length-- ) *p++ = 0;
        return(-1);
    }

    return(0);
}

#ifdef SET_AUTOTEST
static uint8_t k[16] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
static uint8_t nonce[13] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c};
static uint8_t p[64] = "HELLOWORLD!@#$%^helloworld123456";
static uint8_t c[64] = {0};
static uint8_t d[64] = {0};
static uint8_t mic[4] = {0};
static uint8_t astr[4] = {1,2,3,4};

#define LEN 32
void aes_ccm_test2(void)
{

    aes_ccm_encrypt_and_tag(k, nonce, 12, astr, sizeof(astr), p, LEN, c, mic, 4);
    aes_ccm_auth_decrypt(k, nonce, 12, astr, sizeof(astr), c, LEN, d, mic, 4);

    MI_LOG_INFO("Cipher : \n");
    MI_LOG_HEXDUMP(c, LEN);
    MI_LOG_INFO("MIC : \n");
    MI_LOG_HEXDUMP(mic, sizeof(mic));

    MI_LOG_DEBUG("AES128-CCM TEST: ");
    if(memcmp(d, p, LEN) == 0) {
        MI_LOG_PRINTF(" PASS \n");
    }
    else {
        MI_LOG_PRINTF(" FAIL \n");
    }
    
}
#endif

#endif // USE_MIHOME_BEACON
