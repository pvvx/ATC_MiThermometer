/********************************************************************************************************
 * @file     macro_trick.h 
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

/////////////////  variadic macro  ////////////////////////

#if 1
// a little more complex version that works with GCC and visual studio

///   http://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion
#define COUNT_ARGS_IMPL2(_1, _2, _3, _4, _5, _6, _7, _8 , _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N 
#define COUNT_ARGS_IMPL(args)   COUNT_ARGS_IMPL2 args 
#define COUNT_ARGS(...)    		COUNT_ARGS_IMPL((__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define MACRO_CHOOSE_HELPER2(base, count) 	base##count 
#define MACRO_CHOOSE_HELPER1(base, count) 	MACRO_CHOOSE_HELPER2(base, count) 
#define MACRO_CHOOSE_HELPER(base, count) 	MACRO_CHOOSE_HELPER1(base, count) 

#define MACRO_GLUE(x, y) x y 
#define VARARG(base, ...)					MACRO_GLUE(MACRO_CHOOSE_HELPER(base, COUNT_ARGS(__VA_ARGS__)),(__VA_ARGS__)) 
// usage
/*
	#define  fun1(a)		xxxx
	#define  fun2(a, b)		xxxx
	#define  fun3(a, b, c)	xxxx

	#define fun(...)		VARARG(fun,  __VA_ARGS__) 

	int main(){
		fun(1);			// calls fun1(1)
		fun(1, 2);			// calls fun2(1,2)
		fun(1, 2, 3);		// calls fun3(1,2,3)
	}

*/


#else
// a concise version that only works with GCC

///   http://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8 , _9, _10, N, ...) N 
#define VA_NARGS(...) VA_NARGS_IMPL(X,##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0) 
#define VARARG_IMPL2(base, count, ...) base##count(__VA_ARGS__) 
#define VARARG_IMPL(base, count, ...) VARARG_IMPL2(base, count, __VA_ARGS__)  
#define VARARG(base, ...) VARARG_IMPL(base, VA_NARGS(__VA_ARGS__), __VA_ARGS__)  

#endif
// #define NUMARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

/////////////////  end of variadic macro  //////////////////////

