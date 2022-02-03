/********************************************************************************************************
 * @file     static_assert.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 30, 2010
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

#pragma once

// static assertion. evaluate at compile time. It is very useful like,  STATIC_ASSERT(sizeof(a) == 5);

// #define STATIC_ASSERT(expr)   			{ char static_assertion[(expr) ? 1 : -1]; ((void) static_assertion); }	// (void) array;  to remove compiler unused variable warning

// more complicated version canbe used anywhere in the source
#define STATIC_ASSERT_M(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1] 
// token pasting madness: 
#define STATIC_ASSERT3(X,L) 	STATIC_ASSERT_M(X,static_assertion_at_line_##L) 
#define STATIC_ASSERT2(X,L) 	STATIC_ASSERT3(X,L) 

#define STATIC_ASSERT(X)    	STATIC_ASSERT2(X,__LINE__) 

#define STATIC_ASSERT_POW2(expr)		STATIC_ASSERT(!((expr) & ((expr)-1)))					//  assert  expr  is  2**N
#define STATIC_ASSERT_EVEN(expr)		STATIC_ASSERT(!((expr) & 1))
#define STATIC_ASSERT_ODD(expr)			STATIC_ASSERT(((expr) & 1))
#define STATIC_ASSERT_INT_DIV(a, b) 	STATIC_ASSERT((a) / (b) * (b) == (a))

