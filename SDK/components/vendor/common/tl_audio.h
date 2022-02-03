/********************************************************************************************************
 * @file     tl_audio.h 
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
#ifndef _TL_AUDIO_H_
#define _TL_AUDIO_H_

#include "tl_common.h"

#ifndef		TL_NOISE_SUPRESSION_ENABLE
#define		TL_NOISE_SUPRESSION_ENABLE			0
#endif

#ifndef		TL_MIC_PACKET_BUFFER_NUM
#define		TL_MIC_PACKET_BUFFER_NUM		4
#endif

#ifndef 	ADPCM_PACKET_LEN
#define 	ADPCM_PACKET_LEN		0
#endif

#ifndef 	TL_MIC_ADPCM_UNIT_SIZE
#define 	TL_MIC_ADPCM_UNIT_SIZE		0
#endif

#ifndef		TL_MIC_BUFFER_SIZE
#define		TL_MIC_BUFFER_SIZE		0
#endif

#ifndef		TL_SDM_BUFFER_SIZE
#define		TL_SDM_BUFFER_SIZE		0
#endif

#if TL_MIC_BUFFER_SIZE
s16		buffer_mic[TL_MIC_BUFFER_SIZE>>1];
#endif

#if TL_SDM_BUFFER_SIZE
s16		buffer_sdm[TL_SDM_BUFFER_SIZE>>1];
#endif

extern int md_long;
extern int md_short;
extern int md_im;
extern int md_noise;
extern int md_gain;

static inline int noise_supression (s16 md) {

	static int md_th =384;
	//static int md_long =0;
	//static int md_short =0;
	//static int md_im =0;
	//static int md_noise = 0;
	//static int md_gain = 256;
        md_long = ((md_long * 1023) + abs (md)) >> 10;
        md_short = ((md_short * 127) + abs (md)) >> 7;
        md_im = ((md_im * 15) + abs (md)) >> 4;

        //md_long = ((md_long * 127) + abs (md)) >> 7;
        //md_short = ((md_short * 31) + abs (md)) >> 5;
        //md_im = ((md_im * 3) + abs (md)) >> 2;

        if ((md_noise && (md_short > md_th)) || (md_im > (md_th<<1))) {
            md_noise = 0;
            md_long = md_short << 2;
        }
        else if (!md_noise && md_long < md_th) {
            md_noise = 1;
        }
        if (md_noise) {
            if (md_gain) { md_gain --; }
        }
        else {
            if (md_gain < 256) { md_gain ++;}
        }

        return (md * md_gain + 128) >> 8;
}

void pcm_to_adpcm (signed short *ps, int len, signed short *pd);
void mic_to_adpcm (signed short *ps, int len, signed short *pd);
void adpcm_to_pcm (signed short *ps, signed short *pd, int len);
void adpcm_to_sdm (signed short *ps, int len);
void silence_to_sdm (void);

void	proc_mic_encoder (void);
int	*	mic_encoder_data_buffer ();
void 	mic_encoder_data_read_ok (void);

void 	proc_sdm_decoder (void);
int  	sdm_decode_data (int *ps, int nbyte);
void 	sdm_decode_rate (int step, int adj);
int   	sdm_bytes_in_buffer ();

#endif
