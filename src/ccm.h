/**
 * \file ccm.h
 *
 * \brief Counter with CBC-MAC (CCM) for 128-bit block ciphers
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
#ifndef MBEDTLS_CCM_H
#define MBEDTLS_CCM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           CCM buffer encryption
 *
 * \param key       key must be 16 bytes
 * \param length    length of the input data in bytes
 * \param iv        nonce (initialization vector)
 * \param iv_len    length of IV in bytes
 *                  must be 2, 3, 4, 5, 6, 7 or 8
 * \param add       additional data
 * \param add_len   length of additional data in bytes
 *                  must be less than 2^16 - 2^8
 * \param input     buffer holding the input data
 * \param output    buffer for holding the output data
 *                  must be at least 'length' bytes wide
 * \param tag       buffer for holding the tag
 * \param tag_len   length of the tag to generate in bytes
 *                  must be 4, 6, 8, 10, 14 or 16
 *
 * \note            The tag is written to a separate buffer. To get the tag
 *                  concatenated with the output as in the CCM spec, use
 *                  tag = output + length and make sure the output buffer is
 *                  at least length + tag_len wide.
 *
 * \return          0 if successful
 */
int aes_ccm_encrypt_and_tag( const unsigned char *key, 
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *add, size_t add_len,
                         const unsigned char *input, size_t length,
                         unsigned char *output,
                         unsigned char *tag, size_t tag_len );

/**
 * \brief           CCM buffer authenticated decryption
 *
 * \param key       key must be 16 bytes
 * \param length    length of the input data
 * \param iv        initialization vector
 * \param iv_len    length of IV
 * \param add       additional data
 * \param add_len   length of additional data
 * \param input     buffer holding the input data
 * \param output    buffer for holding the output data
 * \param tag       buffer holding the tag
 * \param tag_len   length of the tag
 *
 * \return         0 if successful and authenticated,
 *                 MBEDTLS_ERR_CCM_AUTH_FAILED if tag does not match
 */
int aes_ccm_auth_decrypt( const unsigned char *key, 
                      const unsigned char *iv, size_t iv_len,
                      const unsigned char *add, size_t add_len,
                      const unsigned char *input, size_t length,
                      unsigned char *output,
                      const unsigned char *tag, size_t tag_len );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CCM_H */
