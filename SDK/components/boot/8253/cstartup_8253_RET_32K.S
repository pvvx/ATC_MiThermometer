/********************************************************************************************************
 * @file     cstartup_8253_RET_32K.S
 *
 * @brief    This is the boot file for TLSR8258
 *
 * @author   public@telink-semi.com;
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

#ifdef MCU_STARTUP_8253_RET_32K

#ifndef __LOAD_RAM_SIZE__
#define	__LOAD_RAM_SIZE__		0xc
#endif

	.code	16
@********************************************************************************************************
@                                           MACROS AND DEFINIITIONS
@********************************************************************************************************
@.include "version.in"

					@ Mode, correspords to bits 0-5 in CPSR
	.equ MODE_BITS,		0x1F	@ Bit mask for mode bits in CPSR
	.equ IRQ_MODE, 		0x12	@ Interrupt Request mode
	.equ SVC_MODE, 		0x13	@ Supervisor mode 

	.equ IRQ_STK_SIZE,	0x180
	.equ __LOAD_RAM, 	__LOAD_RAM_SIZE__
	
@********************************************************************************************************
@                                            TC32 EXCEPTION VECTORS
@********************************************************************************************************

	.section	.vectors,"ax"
	.global		__reset
	.global	 	__irq
	.global 	__start
	.global		__LOAD_RAM

__start:					@ MUST,  referenced by boot.link

	.extern irq_handler

	.extern  _ramcode_size_div_16_
	.extern  _ramcode_size_div_256_
	.extern  _ramcode_size_div_16_align_256_
	.extern  _ramcode_size_align_256_
	.extern  _ictag_start_
	.extern  _ictag_end_

	.org 0x0
	tj	__reset
@	.word	(BUILD_VERSION)
	.org 0x8
	.word	(0x544c4e4b)
	.word	(0x00880000 + _ram_use_size_div_16_)
@	.word	(0x00880000 + 0x800)

	.org 0x10
	tj		__irq
	.org 0x18
	.word	(_bin_size_)
@********************************************************************************************************
@                                   LOW-LEVEL INITIALIZATION
@********************************************************************************************************
	.extern  main



	.org 0x20
	.align 4
	.global start_suspend
	.thumb_func
	.type start_suspend, %function

start_suspend:
	tpush   {r2-r3}

    tmovs r2, #129    @0x81
    tloadr r3, __suspend_data      @0x80006f
    tstorerb r2, [r3, #0]  @*(volatile unsigned char *)0x80006f = 0x81

    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8
    tmov r8, r8

    tpop {r2-r3}
    tjex lr

__suspend_data:
	.word   (0x80006f)



__reset:

#if 0
	@ add debug, PB4 output 1
	tloadr     	r1, DEBUG_GPIO    @0x80058a  PB oen
	tmov		r0, #139      @0b 11101111
	tstorerb	r0, [r1, #0]

	tmov		r0, #16			@0b 00010000
	tstorerb	r0, [r1, #1]	@0x800583  PB output
#endif

SET_BOOT:
	tmov        r2, #4
	tloadrb     r1, [r2]		@read form core_840004
	tmov     	r0, #165    @A5
	tcmp        r0, r1
	tjne		SET_BOOT_END

	tmov        r2, #5
	tloadrb     r1, [r2]		@read form core_840005
	tloadr     	r0, BOOT_SEL_D
	tstorerb	r1, [r0, #0]
SET_BOOT_END:



@send flash cmd 0xab to wakeup flash;
FLASH_WAKEUP_BEGIN:
	tloadr      r0,FLASH_RECOVER + 0
	tmov		r1,#0
	tstorerb    r1,[r0,#1]
	tmov        r1,#171						@Flash deep cmd: 0xAB
	tstorerb    r1,[r0,#0]
	tmov		r2,#0
	tmov        r3,#6
TNOP:
	tadd        r2,#1
	tcmp        r2,r3
	tjle        TNOP
	tmov		r1,#1
	tstorerb    r1,[r0,#1]
FLASH_WAKEUP_END:




	tloadr	r0, FLL_D
	tloadr	r1, FLL_D+4
	tloadr	r2, FLL_D+8

FLL_STK:
	tcmp	r1, r2
	tjge	FLL_STK_END
	tstorer r0, [r1, #0]
	tadd    r1, #4
	tj		FLL_STK
FLL_STK_END:

	tloadr	r0, DAT0
	tmcsr	r0			
	tloadr	r0, DAT0 + 8
	tmov	r13, r0  

	tloadr	r0, DAT0 + 4
	tmcsr	r0	
	tloadr	r0, DAT0 + 12
	tmov	r13, r0  

	tmov	r0, #0
	tloadr	r1, DAT0 + 16
	tloadr	r2, DAT0 + 20

ZERO:
	tcmp	r1, r2
	tjge	ZERO_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO
ZERO_END:

	tloadr	r1, DAT0 + 28
	tloadr	r2, DAT0 + 32

ZERO_TAG:
	tcmp	r1, r2
	tjge	ZERO_TAG_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO_TAG
ZERO_TAG_END:

SETIC:
	tloadr     	r1, DAT0 + 24
	tloadr      r0, DAT0 + 36					@ IC tag start
	tstorerb	r0, [r1, #0]
	tadd    	r0, #1							@ IC tag end
	tstorerb	r0, [r1, #1]
	@tmov		r0, #0;
	@tstorerb	r0, [r1, #2]


	tloadr		r1, DATA_I
	tloadr		r2, DATA_I+4
	tloadr		r3, DATA_I+8
COPY_DATA:
	tcmp		r2, r3
	tjge		COPY_DATA_END
	tloadr		r0, [r1, #0]
	tstorer 	r0, [r2, #0]
	tadd    	r1, #4
	tadd		r2, #4
	tj			COPY_DATA
COPY_DATA_END:

#if 0
SETSPISPEED:
	tloadr     	r1, DAT0 + 36
	tmov		r0, #0xbb		@0x0b for fast read; 0xbb for dual dat/adr
	tstorerb	r0, [r1, #0]
	tmov		r0, #3			@3 for dual dat/adr
	tstorerb	r0, [r1, #1]
#endif

	tjl	main
END:	tj	END

	.balign	4
DAT0:
	.word	0x12			    @IRQ    @0
	.word	0x13			    @SVC    @4
	.word	(irq_stk + IRQ_STK_SIZE)
	.word	(0x84c000)		    @12  stack end
	.word	(_start_bss_)               @16
	.word	(_end_bss_)                 @20
	.word	(0x80060c)                  @24
	.word	(0x848000)                  @28
	.word	(0x848100)                  @32
	.word	(0x80)                      @36
@	.word	_ictag_start_               @28		@ IC tag start
@	.word	_ictag_end_	            	@32		@ IC tag end
@	.word	_ramcode_size_div_256_		@36
DATA_I:	
	.word	_dstored_					@0
	.word	_start_data_				@4
	.word	_end_data_					@8

FLL_D:
	.word	0xffffffff
	.word	(_start_data_)
	@.word	(0x84c000)
	.word	(_start_data_ + 32)
	.word   (_retention_data_start_)    @16
    .word   (_retention_data_end_)      @20
    .word   (_rstored_)                 @24
    .word	(_ram_use_size_div_16_)

DEBUG_GPIO:
	.word	(0x80058a)                  @  PBx oen


BOOT_SEL_D:
	.word	(0x80063e)

FLASH_RECOVER:
	.word	(0x80000c)                  @0

	.align 4
__irq:
	tpush    	{r14}
	tpush    	{r0-r7}
	tmrss    	r0
	
	tmov		r1, r8
	tmov		r2, r9
	tmov		r3, r10
	tmov		r4, r11
	tmov		r5, r12
	tpush		{r0-r5}
	
	tjl      	irq_handler

	tpop		{r0-r5}
	tmov		r8, r1
	tmov		r9, r2
	tmov		r10,r3
	tmov		r11,r4
	tmov		r12,r5

	tmssr    	r0
	tpop		{r0-r7}
	treti    	{r15}

ASMEND:

	.section .bss
	.align 4
	.lcomm irq_stk, IRQ_STK_SIZE
	.end

#endif
