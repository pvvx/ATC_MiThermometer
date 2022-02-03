/********************************************************************************************************
 * @file     le_crypto.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2015
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#ifndef TN_DTLS_H
#define TN_DTLS_H

#define		biL			32

#define BITS_TO_LIMBS(i)  ( (i) / 32 + ( (i) % 32 != 0 ) )



#define TNDTLS_ERR_MPI_BAD_INPUT_DATA                    -0x0004  // Bad input parameters to function.
#define TNDTLS_ERR_MPI_INVALID_CHARACTER                 -0x0006  // There is an invalid character in the digit string.
#define TNDTLS_ERR_MPI_BUFFER_TOO_SMALL                  -0x0008  // The buffer is too small to write to.
#define TNDTLS_ERR_MPI_NEGATIVE_VALUE                    -0x000A  // The input arguments are negative or result in illegal output.
#define TNDTLS_ERR_MPI_DIVISION_BY_ZERO                  -0x000C  // The input argument for division is zero, which is not allowed.
#define TNDTLS_ERR_MPI_NOT_ACCEPTABLE                    -0x000E  // The input arguments are not acceptable.


typedef struct {
	int	s;
	int n;
	unsigned int p[8];
} tn_mpi;

typedef struct {
	int	s;
	int n;
	unsigned int p[16];
} tn_mpi16;

typedef struct
{
    tn_mpi X;
    tn_mpi Y;
    tn_mpi Z;
} tn_ecp_point;

typedef struct
{
	tn_ecp_point	X1X3;			// XsXc
	tn_ecp_point	X2X4;			//
	tn_ecp_point	GaGb;			// Peer's G
    tn_mpi			m2m4;			// private key
    tn_mpi			xcxs;			// key exchange
    tn_mpi			s;				// passphrase
} tn_ecjpake_context;

extern const unsigned int tn_p256_nq[9];

extern const unsigned int tn_p256_n[10];

extern const unsigned int tn_p256_pq[9];
 
extern const unsigned int tn_p256_pr[8];

extern const unsigned int tn_p256_p[10];

extern const tn_ecp_point	tn_t[16];

extern const tn_ecp_point tn_p256_ecp_g;

void tn_mpi_init( tn_mpi *X, int n);
void tn_mpi_free( tn_mpi *X );
int tn_mpi_copy( tn_mpi *X, const tn_mpi *Y );
int tn_mpi_cmp_mpi( const tn_mpi *X, const tn_mpi *Y );
void tn_mpi_modn_add (tn_mpi * px);
void tn_mpi_modp_add (tn_mpi * px);
void tn_mpi_modp_sub (tn_mpi * px);
int tn_mpi_mul_mpi_modp( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_mul_int_modp( tn_mpi *X, const tn_mpi *A,  int n );
int tn_mpi_add_mpi_modp( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_sub_mpi_modp( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_sub_mpi_modn( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_shift_l_modp( tn_mpi *X, int count );
int tn_mpi_add_mpi( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_sub_mpi( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_cmp_int( tn_mpi *X, int z );
int tn_mpi_lset( tn_mpi *X, int z );
int tn_mpi_bitlen( const tn_mpi *X );
int tn_mpi_get_bit( const tn_mpi *X, int pos );
int tn_mpi_modp( tn_mpi *R, const tn_mpi *A);
int tn_mpi_safe_cond_assign( tn_mpi *X, const tn_mpi *Y, unsigned char assign );
int tn_mpi_inv_mod( tn_mpi *X, const tn_mpi *A );
int tn_mpi_mul_mpi_modn( tn_mpi *X, const tn_mpi *A, const tn_mpi *B );
int tn_mpi_neg_modn( tn_mpi *X );

//////////////////////////////////////////////////////////////////////////////////
int tn_ecp_copy( tn_ecp_point *P, const tn_ecp_point *Q );
int tn_ecp_double_jac( tn_ecp_point *R, const tn_ecp_point *P );
int tn_ecp_add_mixed( tn_ecp_point *R, const tn_ecp_point *P, const tn_ecp_point *Q );
int tn_ecp_normalize_jac( tn_ecp_point *pt );
int tn_ecp_mul(tn_ecp_point *R, const tn_mpi *m, const tn_ecp_point *P);
int tn_ecp_muladd( tn_ecp_point *R, const tn_mpi *m, const tn_ecp_point *P, const tn_mpi *n, const tn_ecp_point *Q );

extern void tn_ecp_gen_keypair (tn_mpi *d, tn_ecp_point *Q);
extern void tn_p256_keypair (unsigned char *s, unsigned char *x, unsigned char *y);
extern void tn_p256_dhkey (unsigned char *r, unsigned char *s, unsigned char * x, unsigned char *y);

extern void tn_aes_128(unsigned char *key, unsigned char *plaintext, unsigned char *result);
extern void tn_aes_cmac ( unsigned char *key, unsigned char *input, int length,
                  unsigned char *mac );
extern int tn_crypto_f4 (unsigned char *r, unsigned char u[32], unsigned char v[32], unsigned char x[16], unsigned char z);
extern unsigned int tn_crypto_g2 (unsigned char u[32], unsigned char v[32], unsigned char x[16], unsigned char y[16]);
extern int tn_crypto_f5 (unsigned char *mac, unsigned char *ltk, unsigned char w[32], unsigned char n1[16], unsigned char n2[16],
				  unsigned char a1[7], unsigned char a2[7]);
extern int tn_crypto_f6 (unsigned char *e, unsigned char w[16], unsigned char n1[16], unsigned char n2[16],
				  unsigned char r[16], unsigned char iocap[3], unsigned char a1[7], unsigned char a2[7]);
extern int tn_crypto_h6 (unsigned char *r, unsigned char key[16], unsigned char id[4]);

extern int test_crypto_func ();
extern int test_dhkey ();

#endif 
