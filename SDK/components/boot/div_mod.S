/********************************************************************************************************
 * @file     div_mod.S
 *
 * @brief    This is the assembly file for TLSR8258
 *
 * @author	 author@telink-semi.com;
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
 *
 *******************************************************************************************************/

#define 	UDIV		#0
#define 	SDIV		#1
#define 	UMOD		#2
#define 	SMOD		#3

#define 	MUL2_STEP		8

	.code	16
	.text

	.section	.ram_code,"ax" //in ram code
	.align	2
	.global	__modsi3
	.code 16
	.thumb_func
	.type	__modsi3, %function
__modsi3:
	tmov	r2, SMOD
	tj	div
	.size	__modsi3, .-__modsi3

	.section	.ram_code,"ax" //in ram code
	.align	2
	.global	__divsi3
	.code 16
	.thumb_func
	.type	__divsi3, %function
__divsi3:
	tmov	r2, SDIV
	tj	div
	.size	__divsi3, .-__divsi3

	.section	.ram_code,"ax" //in ram code
	.align	2
	.global	__umodsi3
	.code 16
	.thumb_func
	.type	__umodsi3, %function
__umodsi3:
	tmov	r2, UMOD
	tj	div
	.size	__umodsi3, .-__umodsi3

	.section	.ram_code,"ax" //in ram code
	.align	2
	.global	__udivsi3
	.code 16
	.thumb_func
	.type	__udivsi3, %function
__udivsi3:
	tmov	r2, UDIV
	tj	div
	.size	__udivsi3, .-__udivsi3

	.section	.ram_code,"ax" //in ram code
	.align	2
	.global	div
	.code 16
	.thumb_func
	.type	div, %function
div:
	tmrcs	r3
	tpush	{r3, r4}
	tmov	r4, #0x80
	tor	r3, r4
	tmcsr	r3
	
	tloadr	r3, .L11
	tstorer	r0, [r3]
	tadd	r3, r3, #4
	tstorer	r1, [r3]
	tsub	r3, r3, #8
	tstorerb	r2, [r3]

.L2:
	tloadrb	r0, [r3]
	tcmp	r0, #0
	tjne	.L2
	tcmp	r2, #1
	tjls	.L4
	tadd	r3, r3, #8
	tloadr	r0, [r3]
	tj	.L6
.L4:
	tadd	r3, r3, #4
	tloadr	r0, [r3]

.L6:
	tpop	{r3, r4}
	tmcsr	r3
	tjex	lr

	.align	4
.L11:
	.word(0x800664)
	.word(0x800660)
	.word(0x800668)
	.size	div, .-div

//removed
#if 0
	//.section	.ram_code,"ax" //in ram code
	.align	4
	.global	mul32x32_64
	.thumb_func
	.type	mul32x32_64, %function
mul32x32_64:
    tmul	r0, r1
    tloadr	r1, [pc, #4]
    tloadr	r1, [r1, #0]
    tjex	lr
    .word(0x008006fc)
#endif

#if 0
	//.section	.ram_code,"ax" //in ram code
	.align	4
	.global	mz_mul1
	.thumb_func
	.type	mz_mul1, %function
mz_mul1:
	tpush 	{r4, r5, r6, r7}
	tmov	r4, r8
	tpush	{r4}
	tmov	r4, #1
	tmov	r8, r4				//r8 = 1
	tloadr	r6, [pc, #4]     	//r6 REG_ADDR32(0x6fc)
	tmovs	r5, #0				//clear carry
	tj	MZ_MUL1_END
    .word(0x008006fc)
MZ_MUL1_START:
	tmul	r4, r3				// l0 = a0 * b
	tloadr	r7, [r0, #0]		// y0
	tadd	r4, r5				// l0 + c => c0
	tsubc	r5, r5				// c0 - 1
	tadd	r4, r7				// l0 + c + y0 => c1
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r5,	r8				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r4}			// store y0
	taddc	r5, r7				// cn = c0 + h1 + c1
MZ_MUL1_END:
	tloadm	r1!, {r4}			// load *a
	tsub	r2, #1				// r2--
	tcmp	r2, #0
	tjge		MZ_MUL1_START	// carry set
	tstorem	r0!, {r5}

	tpop	{r4}
	tmov	r8, r4
	tpop 	{r4, r5, r6, r7}
    tjex	lr

#endif

	//.section	.ram_code,"ax" //in ram code
	.align	4
	.global	mz_mul2
	.thumb_func
	.type	mz_mul2, %function
mz_mul2:
	tpush 	{r4, r5, r6, r7}
	tmov	r4, r8
	tmov	r5, r9
	tmov	r6, r10

	tmov	r7, r11
	tpush	{r4, r5, r6, r7}
	tmov	r8, r2				//r8 = n, loop number
	tmov	r2,	#1

	tmov	r10, r2				// r10 = 1
	tsub	r2, #(MUL2_STEP + 1)
	tmov	r9, r2				//r9 = -MUL2_STEP
	tmov	r2, #0

	tmov	r2, #0
	tloadr	r6, [pc, #4]     	 //r6 REG_ADDR32(0x6fc)
	tmov	r11,r2				//r11 = 0
	tj	MZ_MUL2_LOOP
	//tj	MZ_MUL2_LOOP2
	.word(0x008006fc)

MZ_MUL2_START:
	//a0
	tmul	r4, r3				// l0 = a0 * b
	tloadr	r7, [r0, #0]		// y0
	tadd	r4, r2				// l0 + c => c0
	tsubc	r2, r2				// c0 - 1
	tadd	r4, r7				// l0 + c + y0 => c1
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r4}			// store y0 y1
	taddc	r2, r7				// cn = c0 + h1 + c1

	tmul	r5, r3				// l1 = a1 * b
	tloadr	r7, [r0, #0]		// y1
	tadd	r5, r2				// l1 + cn => c2
	tsubc	r2, r2				// c2 - 1
	tadd	r5, r7				// l1 + c + y1 => c3
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r5}			// store y0 y1
	tloadm	r1!, {r4, r5}		// load *a
	taddc	r2, r7				// cn2 = c2 + h1 + c3

	//a0
	tmul	r4, r3				// l0 = a0 * b
	tloadr	r7, [r0, #0]		// y0
	tadd	r4, r2				// l0 + c => c0
	tsubc	r2, r2				// c0 - 1
	tadd	r4, r7				// l0 + c + y0 => c1
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r4}			// store y0 y1
	taddc	r2, r7				// cn = c0 + h1 + c1

	tmul	r5, r3				// l1 = a1 * b
	tloadr	r7, [r0, #0]		// y1
	tadd	r5, r2				// l1 + cn => c2
	tsubc	r2, r2				// c2 - 1
	tadd	r5, r7				// l1 + c + y1 => c3
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r5}			// store y0 y1
	tloadm	r1!, {r4, r5}		// load *a
	taddc	r2, r7				// cn2 = c2 + h1 + c3

	//a0
	tmul	r4, r3				// l0 = a0 * b
	tloadr	r7, [r0, #0]		// y0
	tadd	r4, r2				// l0 + c => c0
	tsubc	r2, r2				// c0 - 1
	tadd	r4, r7				// l0 + c + y0 => c1
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r4}			// store y0 y1
	taddc	r2, r7				// cn = c0 + h1 + c1

	tmul	r5, r3				// l1 = a1 * b
	tloadr	r7, [r0, #0]		// y1
	tadd	r5, r2				// l1 + cn => c2
	tsubc	r2, r2				// c2 - 1
	tadd	r5, r7				// l1 + c + y1 => c3
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r5}			// store y0 y1
	tloadm	r1!, {r4, r5}		// load *a
	taddc	r2, r7				// cn2 = c2 + h1 + c3

///// next 2
	tmul	r4, r3				// l0 = a0 * b
	tloadr	r7, [r0, #0]		// y0
	tadd	r4, r2				// l0 + c => c0
	tsubc	r2, r2				// c0 - 1
	tadd	r4, r7				// l0 + c + y0 => c1
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r4}			// store y0 y1
	taddc	r2, r7				// cn = c0 + h1 + c1

	tmul	r5, r3				// l1 = a1 * b
	tloadr	r7, [r0, #0]		// y1
	tadd	r5, r2				// l1 + cn => c2
	tsubc	r2, r2				// c2 - 1
	tadd	r5, r7				// l1 + c + y1 => c3
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r5}			// store y0 y1
	taddc	r2, r7				// cn2 = c2 + h1 + c3

MZ_MUL2_LOOP:
	tloadm	r1!, {r4, r5}		// load *a
	tadd	r8, r9				// r8 -= MUL2_STEP
	tcmp	r8, r11				// const 0
	tjge		MZ_MUL2_START	// carry set

	tmov	r5,	r8
	tadd	r5, #MUL2_STEP
	tsub	r1, #8
	tj	MZ_MUL2_LOOP2

MZ_MUL2_START2:
	tmul	r4, r3				// l0 = a0 * b
	tloadr	r7, [r0, #0]		// y0
	tadd	r4, r2				// l0 + c => c0
	tsubc	r2, r2				// c0 - 1
	tadd	r4, r7				// l0 + c + y0 => c1
	tloadr	r7, [r6, #0]		// r7 = h0
	tadd	r2,	r10				// c0 - 1 + 1 = c0 (nc)
	tstorem	r0!, {r4}			// store y0
	taddc	r2, r7				// cn = c0 + h1 + c1

MZ_MUL2_LOOP2:
	tloadm	r1!, {r4}		// load *a
	tsub	r5, #1				// r7--
	tcmp	r5, #0
	tjge		MZ_MUL2_START2	// carry set

MZ_MUL2_END:
	//tmov	r2, #13
	tstorem	r0!, {r2}

	tpop 	{r4, r5, r6, r7}
	tmov	r8, r4
	tmov	r9, r5
	tmov	r10, r6
	tmov	r11, r7
	tpop 	{r4, r5, r6, r7}
    tjex	lr
    tnop


///////// asm crc24 function 2
	.section	.ram_code,"ax" //in ram code
	.align	2
	.global	blt_packet_crc24_opt
	.code 16
	.thumb_func
	.type	blt_packet_crc24_opt, %function
blt_packet_crc24_opt:
	tpush	{r3, r4, r5, r6, r7, lr}
	tmov	r5, r8
	tpush	{r5}
	tmov	r5, r1
	tneg	r1, r0
	tmov	r4, #3
	tand	r1, r4			//number of byte CRC of pre_process to align CRC to word boundary
	tsub	r5, r1
	tjge	CRC24_SAVE_WORD_NUM
	tadd	r1, r5
	tmov	r5, #0
CRC24_SAVE_WORD_NUM:
	tmov	r8, r5			//save to r8
	//tloadr	r3, CRC24_DAT
	tadd	r4, r0, #0
	tmov	r0, #0
	tmov	r7, #60			//r7 = 15 * 4
CRC24_BYTE_LOOP:			//r4: src; r6: dat; r2: crc; r5: tmp
	tcmp	r0, r1
	tjeq	CRC24_BYTE_END
	tloadrb	r6, [r4, r0]	//r6 = dat[r0]
	txor	r6, r2			//r6 = crc ^ dat
	tshftl	r5, r6, #2		//r5 = r6 << 2
	tand	r5, r7			//r2 = r2 & 60
	tloadr	r5, [r5, r3]	//load table
	tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
	tshftr	r6, r6, #2		// r6 = r6 >> 2
	txor	r2, r5			//r2 = r5 ^  r2
	tand	r6, r7			//r6 = r6 & 60

	tloadr	r6, [r6, r3]
	tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
	tadd	r0, #1
	txor	r2, r6			//r2 = r6 ^  r2
	tjne	CRC24_BYTE_LOOP
CRC24_BYTE_END:
	tmov	r1, r8
	tcmp	r1, #0
	tjeq	CRC24_END
	tmov	r5, #0
	tmov	r8, r5
	tadd	r4, r0
	tmov	r0, #0
CRC24_WORD_LOOP:
		tsub	r1, #4
		tjlt	CRC24_WORD_END
		tloadr	r0, [r4, #0]	//r0 = dat[r0]
		tadd	r4, #4
		tshftr	r6, r0, #0		// r6 = r0 >> 0
	CRC24_WORD_nib0:
		txor	r6, r2			//r6 = crc ^ dat
		tshftl	r5, r6, #2		//r5 = r6 << 2
		tand	r5, r7			//r2 = r2 & 60
		tloadr	r5, [r5, r3]	//load table
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r6, r6, #2		// r6 = r6 >> 2
		tand	r6, r7			//r6 = r6 & 60
		tloadr	r6, [r6, r3]
		txor	r2, r5			//r2 = r5 ^  r2
	CRC24_WORD_nib1:
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r5, r0, #8		//dat >> 8
		txor	r2, r6			//r2 = r6 ^  r2
	CRC24_WORD_nib2:
		txor	r5, r2			//r6 = crc ^ dat
		tshftl	r6, r5, #2		//r5 << 2
		tand	r6, r7
		tloadr	r6, [r6, r3]
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r5, r5, #2
		tand	r5, r7			//r6 = r6 & 60
		tloadr	r5, [r5, r3]
		txor	r2, r6			//r2 = r6 ^  r2
	CRC24_WORD_nib3:
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r6, r0, #16		//dat >> 8
		txor	r2, r5			//r2 = r6 ^  r2
	CRC24_WORD_nib4:
		txor	r6, r2			//r6 = crc ^ dat
		tshftl	r5, r6, #2		//r5 = r6 << 2
		tand	r5, r7			//r2 = r2 & 60
		tloadr	r5, [r5, r3]	//load table
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r6, r6, #2		// r6 = r6 >> 2
		tand	r6, r7			//r6 = r6 & 60
		tloadr	r6, [r6, r3]
		txor	r2, r5			//r2 = r5 ^  r2
	CRC24_WORD_nib5:
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r5, r0, #24		//dat >> 8
		txor	r2, r6			//r2 = r6 ^  r2
	CRC24_WORD_nib6:
		txor	r5, r2			//r6 = crc ^ dat
		tshftl	r6, r5, #2		//r5 << 2
		tand	r6, r7
		tloadr	r6, [r6, r3]
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tshftr	r5, r5, #2
		tand	r5, r7			//r5 = r5 & 60
		tloadr	r5, [r5, r3]
		txor	r2, r6			//r2 = r6 ^  r2
	CRC24_WORD_nib7:
		tasr	r2, r2, #4		//r2 >> 4 (crc >> 4)
		tmov	r0, #0
		txor	r2, r5			//r2 = r6 ^  r2
	tj		CRC24_WORD_LOOP
	CRC24_WORD_END:
		tadd	r1, #4
		tj		CRC24_BYTE_LOOP
CRC24_END:
	tadd	r0, r2, #0
	tpop	{r5}
	tmov	r8, r5
	tpop	{r3, r4, r5, r6, r7, pc}
	tnop

//		static int Crc24Lookup[16] = {
//			0x0000000,0x01b4c00,0x0369800,0x02dd400,
//			0x06d3000,0x0767c00,0x05ba800,0x040e400,
//			0x0da6000,0x0c12c00,0x0ecf800,0x0f7b400,
//			0x0b75000,0x0ac1c00,0x081c800,0x09a8400,
//		};
//		//usage
//		//crc = blt_packet_crc24_opt (dat, length, crc_init, Crc24Lookup);
