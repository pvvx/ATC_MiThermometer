/********************************************************************************************************
 * @file     selection_sort.c 
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

void selection_sort_char(unsigned char * arr, int size){
	//sorting
	int i, j;
	for(i = 0; i < size; ++i)
	{
		for( j = i + 1; j < size; ++j)
		{
			if (arr[j] < arr[i])
			{
				unsigned char t = arr[i];
				arr[i] = arr[j];
				arr[j] = t;
			}
		}
	}
}

void selection_sort_int(unsigned int * arr, int size){
	//sorting
	int i, j;
	for(i = 0; i < size; ++i)
	{
		for( j = i + 1; j < size; ++j)
		{
			if (arr[j] < arr[i])
			{
				unsigned int t = arr[i];
				arr[i] = arr[j];
				arr[j] = t;
			}
		}
	}
}


