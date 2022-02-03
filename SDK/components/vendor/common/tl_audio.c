/********************************************************************************************************
 * @file     tl_audio.c 
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
#include "drivers.h"
#include "tl_audio.h"
//#include "drivers.h"
#include <stack/ble/trace.h>



int md_long =0;
int md_short =0;
int md_im =0;
int md_noise = 0;
int md_gain = 256;

static const signed char idxtbl[] = { -1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8};
static const unsigned short steptbl[] = {
 7,  8,  9,  10,  11,  12,  13,  14,  16,  17,
 19,  21,  23,  25,  28,  31,  34,  37,  41,  45,
 50,  55,  60,  66,  73,  80,  88,  97,  107, 118,
 130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
 337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767   };

//////////////////////////////////////////////////////////
//	for 8266: input 128-word, output 80-byte
//////////////////////////////////////////////////////////
void pcm_to_adpcm (signed short *ps, int len, signed short *pd)
{
	int i, j;
	unsigned short code=0;
	unsigned short code16=0;
	int predict_idx = 1;
	code = 0;

	for (i=0; i<8; i++) {
		*pd++ = ps[i];   //copy first 8 samples
	}
	int predict = ps[0];
	for (i=1; i<len; i++) {

		s16 di = ps[i];
		int step = steptbl[predict_idx];
		int diff = di - predict;

		if (diff >=0 ) {
			code = 0;
		}
		else {
			diff = -diff;
			code = 8;
		}

		int diffq = step >> 3;

		for (j=4; j>0; j=j>>1) {
			if( diff >= step) {
				diff = diff - step;
				diffq = diffq + step;
				code = code + j;
			}
			step = step >> 1;
		}

		code16 = (code16 >> 4) | (code << 12);
		if ( (i&3) == 3) {
			*pd++ = code16;
		}

		if(code >= 8) {
			predict = predict - diffq;
		}
		else {
			predict = predict + diffq;
		}

		if (predict > 32767) {
			predict = 32767;
		}
		else if (predict < -32767) {
			predict = -32767;
		}

		predict_idx = predict_idx + idxtbl[code];
		if(predict_idx < 0) {
			predict_idx = 0;
		}
		else if(predict_idx > 88) {
			predict_idx = 88;
		}
	}
}

#define				NUM_OF_ORIG_SAMPLE				2

void mic_to_adpcm (signed short *ps, int len, signed short *pd)
{
	int i, j;
	unsigned short code=0;
	unsigned short code16=0;
	int predict_idx = 1;
	code = 0;

	for (i=0; i<NUM_OF_ORIG_SAMPLE; i++) {
		*pd++ = ps[i];   //copy first 5 samples
	}
	int predict = ps[0];
	for (i=1; i<len; i++) {

		s16 di = ps[i];
		int step = steptbl[predict_idx];
		int diff = di - predict;

		if (diff >=0 ) {
			code = 0;
		}
		else {
			diff = -diff;
			code = 8;
		}

		int diffq = step >> 3;

		for (j=4; j>0; j=j>>1) {
			if( diff >= step) {
				diff = diff - step;
				diffq = diffq + step;
				code = code + j;
			}
			step = step >> 1;
		}

		code16 = (code16 >> 4) | (code << 12);
		if ( (i&3) == 3) {
			*pd++ = code16;
		}

		if(code >= 8) {
			predict = predict - diffq;
		}
		else {
			predict = predict + diffq;
		}

		if (predict > 32767) {
			predict = 32767;
		}
		else if (predict < -32767) {
			predict = -32767;
		}

		predict_idx = predict_idx + idxtbl[code];
		if(predict_idx < 0) {
			predict_idx = 0;
		}
		else if(predict_idx > 88) {
			predict_idx = 88;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////
//	256-samples split into 2
/////////////////////////////////////////////////////////////////////////////////
void mic_to_adpcm_split (signed short *ps, int len, signed short *pds, int start)
{
	int i, j;
	unsigned short code=0;
	unsigned short code16=0;
	static int predict_idx = 1;
	code = 0;
	static signed short *pd;
	static int predict;

	//byte2,byte1: predict;  byte3: predict_idx; byte4:adpcm data len
	if (start)
	{
		pd = pds;
		*pd++ = predict;
		* (((signed char *)pds) + 2)= predict_idx;
		* (((unsigned char *)pds) + 3)= (ADPCM_PACKET_LEN - 4);
		pd++;
	}

	//byte5- byte128: 124 byte(62 sample) adpcm data
	for (i=0; i<len; i++) {

		s16 di = ps[i];
		int step = steptbl[predict_idx];
		int diff = di - predict;

		if (diff >=0 ) {
			code = 0;
		}
		else {
			diff = -diff;
			code = 8;
		}

		int diffq = step >> 3;

		for (j=4; j>0; j=j>>1) {
			if( diff >= step) {
				diff = diff - step;
				diffq = diffq + step;
				code = code + j;
			}
			step = step >> 1;
		}

		code16 = (code16 >> 4) | (code << 12);
		if ( (i&3) == 3) {
			*pd++ = code16;
		}

		if(code >= 8) {
			predict = predict - diffq;
		}
		else {
			predict = predict + diffq;
		}

		if (predict > 32767) {
			predict = 32767;
		}
		else if (predict < -32767) {
			predict = -32767;
		}

		predict_idx = predict_idx + idxtbl[code];
		if(predict_idx < 0) {
			predict_idx = 0;
		}
		else if(predict_idx > 88) {
			predict_idx = 88;
		}
	}
}

////////////////////////////////////////////////////////////////////
/*  name ADPCM to pcm
    signed short *ps -> pointer to the adpcm source buffer
    signed short *pd -> pointer to the pcm destination buffer
    int len          -> decorded size
*/

#if (UI_AUDIO_ENABLE && CLOCK_SYS_CLOCK_HZ == 24000000)  //only for ble master 24m system clock
_attribute_ram_code_
#endif
void adpcm_to_pcm (signed short *ps, signed short *pd, int len){
	int i;

	//byte2,byte1: predict;  byte3: predict_idx; byte4:adpcm data len
	int predict = ps[0];
	int predict_idx = ps[1] & 0xff;
//	int adpcm_len = (ps[1]>>8) & 0xff;

	unsigned char *pcode = (unsigned char *) (ps + NUM_OF_ORIG_SAMPLE);

	unsigned char code;
	code = *pcode ++;

	//byte5- byte128: 124 byte(62 sample) adpcm data
	for (i=0; i<len; i++) {

		if (1) {
			int step = steptbl[predict_idx];

			int diffq = step >> 3;

			if (code & 4) {
				diffq = diffq + step;
			}
			step = step >> 1;
			if (code & 2) {
				diffq = diffq + step;
			}
			step = step >> 1;
			if (code & 1) {
				diffq = diffq + step;
			}

			if (code & 8) {
				predict = predict - diffq;
			}
			else {
				predict = predict + diffq;
			}

			if (predict > 32767) {
				predict = 32767;
			}
			else if (predict < -32767) {
				predict = -32767;
			}

			predict_idx = predict_idx + idxtbl[code & 15];

			if(predict_idx < 0) {
				predict_idx = 0;
			}
			else if(predict_idx > 88) {
				predict_idx = 88;
			}

			if (i&1) {
				code = *pcode ++;
			}
			else {
				code = code >> 4;
			}
		}

		if (0 && i < NUM_OF_ORIG_SAMPLE) {
			*pd++ = ps[i];
		}
		else {
			*pd++ = predict;
		}
	}
}

#ifndef		ADPCM_PACKET_LEN
#define		ADPCM_PACKET_LEN					128
#endif

#if		TL_MIC_BUFFER_SIZE

#define	BUFFER_PACKET_SIZE		((ADPCM_PACKET_LEN >> 2) * TL_MIC_PACKET_BUFFER_NUM)

int		buffer_mic_enc[BUFFER_PACKET_SIZE];
u8		buffer_mic_pkt_wptr;
u8		buffer_mic_pkt_rptr;

u32		adb_t2;


#define TL_NOISE_SUPRESSION_ENABLE 0 // TODO : too much calculation can have packet drop
#if 	IIR_FILTER_ENABLE
int c1[5] = {5751, 895, 1010, 253, -187};//filter all 
int c2[5] = {4294, -6695, 3220, 1674, -855};//filter 1.2khz 
int c3[5] = {4739, -2293, 1254, 573, -474};//filter 4khz
int filter_1[10];
int filter_2[10];
int filter_3[10];
u8  filter1_shift;
u8  filter2_shift;
u8  filter3_shift;

void voice_iir (signed short * ps, signed short *pd, int* coef, int nsample,u8 shift)
{
      int i = 0;
      int s = 0;
      for (i=0; i<nsample; i++)
      {
            s = (*ps * coef[0])>>shift;                  //input 16-bit
            s += coef[5] * coef[1];
            s += coef[6] * coef[2];       //coef 0,1,2: 12-bit
            s += coef[7] * coef[3];
            s += coef[8] * coef[4];      //coef 4 & 5: 10-bit; coef 7 & 8: 18-bit
            s = s >> 10;                        //18-bit
            if (s >= (1<<18))
                  s = (1<<18) - 1;
            else if (s < -(1<<18))
                  s = - (1<<18);
            coef[6] = coef[5];                  //16-bit
            coef[5] = *ps++;              //16-bit
            coef[8] = coef[7];                  //18-bit
            coef[7] = s;
            *pd++ = s >> 2;
      }
}
#endif 
void	proc_mic_encoder (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = get_mic_wr_ptr();
	u16 l = (mic_wptr >= buffer_mic_rptr) ? (mic_wptr - buffer_mic_rptr) : 0xffff;

	if (l >=(TL_MIC_BUFFER_SIZE>>2)) {
		log_task_begin (TR_T_adpcm);

		s16 *ps = buffer_mic + buffer_mic_rptr;
#if 	TL_NOISE_SUPRESSION_ENABLE
        // for FIR adc sample data, only half part data are effective
		for (int i=0; i<TL_MIC_ADPCM_UNIT_SIZE*2; i++) {
			ps[i] = noise_supression (ps[i]);
        }
#endif
#if 	IIR_FILTER_ENABLE
		extern u8 mic_start_flag;
		if(mic_start_flag){
			mic_start_flag =0;
			memset(filter_1,0,sizeof(filter_1));
			memset(filter_2,0,sizeof(filter_2));
			memset(filter_3,0,sizeof(filter_3));
		}
		memcpy(filter_1,c1,sizeof(c1));
		memcpy(filter_2,c2,sizeof(c2));
		memcpy(filter_3,c3,sizeof(c3));
		#if 1
		voice_iir(ps,ps,filter_2,(TL_MIC_BUFFER_SIZE>>2),filter2_shift);
		voice_iir(ps,ps,filter_3,(TL_MIC_BUFFER_SIZE>>2),filter3_shift);
		voice_iir(ps,ps,filter_1,(TL_MIC_BUFFER_SIZE>>2),filter1_shift);
		#endif
#endif 
		mic_to_adpcm_split (	ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr++;
		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
			log_event (TR_T_adpcm_enc_overflow);
		}

		log_task_end (TR_T_adpcm);
	}
}

int	*	mic_encoder_data_buffer ()
{
	if (buffer_mic_pkt_rptr == buffer_mic_pkt_wptr) {
			return 0;
	}

	int *ps = buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
			(buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1));


	return ps;
}

void mic_encoder_data_read_ok (void)
{
	buffer_mic_pkt_rptr++;
}


#if 0
void	proc_mic_encoder (void)
{
	u32 t = clock_time ();
	static u16	buffer_mic_rptr;
	u16 mic_wptr = reg_audio_wr_ptr;
	u16 l = ((mic_wptr<<1) - buffer_mic_rptr) & ((TL_MIC_BUFFER_SIZE>>1) - 1);
	if (l >= 128) {
		log_task_begin (TR_T_adpcm);
		s16 *ps = buffer_mic + buffer_mic_rptr;

#if 	TL_NOISE_SUPRESSION_ENABLE
		for (int i=0; i<128; i++) {
			ps[i] = noise_supression (ps[i] >> 16) << 16;
		}
#endif
		mic_to_adpcm_split (	ps,	128,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						((buffer_mic_pkt_wptr>>1) & (TL_MIC_PACKET_BUFFER_NUM - 1))), !(buffer_mic_pkt_wptr&1));

		buffer_mic_rptr = (buffer_mic_rptr + 128) & ((TL_MIC_BUFFER_SIZE>>1) - 1);
		buffer_mic_pkt_wptr++;
		int pkts = ((buffer_mic_pkt_wptr>>1) - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
			log_event (TR_T_adpcm_enc_overflow);
		}
		adb_t2 = clock_time() - t;
		log_task_end (TR_T_adpcm);
	}
}


int	*	mic_encoder_data_buffer ()
{
	if ((buffer_mic_pkt_rptr & 0x7f) == (buffer_mic_pkt_wptr >> 1)) {
			return 0;
	}

	int *ps = buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
			(buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1));

	buffer_mic_pkt_rptr++;

	return ps;
}
#endif

#endif

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
//	hardware dependent
/////////////////////////////////////////////////////////////
#if TL_SDM_BUFFER_SIZE

int		buffer_sdm_wptr;
int		buffer_sdm_dec[ADPCM_PACKET_LEN];

void adpcm_to_sdm (signed short *ps, int len){
	int i;
	int predict_idx = 1;
	int predict=0;

	unsigned char *pcode = (unsigned char *) (ps + 12);
	unsigned char code=0;

	for (i=0; i<len; i++) {

		if (i) {
			int step = steptbl[predict_idx];

			int diffq = step >> 3;

			if (code & 4) {
				diffq = diffq + step;
			}
			step = step >> 1;
			if (code & 2) {
				diffq = diffq + step;
			}
			step = step >> 1;
			if (code & 1) {
				diffq = diffq + step;
			}

			if (code & 8) {
				predict = predict - diffq;
			}
			else {
				predict = predict + diffq;
			}

			if (predict > 32767) {
				predict = 32767;
			}
			else if (predict < -32767) {
				predict = -32767;
			}

			predict_idx = predict_idx + idxtbl[code & 15];

			if(predict_idx < 0) {
				predict_idx = 0;
			}
			else if(predict_idx > 88) {
				predict_idx = 88;
			}

			if (i&1) {
				code = *pcode ++;
			}
			else {
				code = code >> 4;
			}
		}
		else {
			code = *pcode++ >> 4;
			predict = ps[0];
		}

		int t2;
		if (i < 8) {
			t2 = ps[i];
		}
		else {
			t2 = predict;
		}
		//* ((s16 *) (buffer_sdm + buffer_sdm_wptr)) = t2;
		buffer_sdm[buffer_sdm_wptr] = (t2<<0);
		buffer_sdm_wptr = (buffer_sdm_wptr + 1) & ((TL_SDM_BUFFER_SIZE>>1) - 1);
	}
}

void pcm_to_sdm (signed short *ps, int len){
	for (int i=0; i<len; i++) {
		buffer_sdm[buffer_sdm_wptr] = ps[i];
		buffer_sdm_wptr = (buffer_sdm_wptr + 1) & ((TL_SDM_BUFFER_SIZE>>1) - 1);
	}
}

void silence_to_sdm (void){
	for (int i=0; i<TL_SDM_BUFFER_SIZE>>1; i++) {
		* ((s16 *) (buffer_sdm + i)) = 0;
	}
}

int  sdm_decode_ready (int nbyte_to_decode)
{
	u16 sdm_rptr = reg_aud_rptr; //get_sdm_rd_ptr ();
	u16 num = ((buffer_sdm_wptr>>1) - sdm_rptr) & ((TL_SDM_BUFFER_SIZE>>2) - 1);
	return ((nbyte_to_decode>>2) + num) < (TL_SDM_BUFFER_SIZE >> 2);
}

int   sdm_word_in_buffer ()
{
	u16 num = ((buffer_sdm_wptr>>1) - reg_aud_rptr) & ((TL_SDM_BUFFER_SIZE>>2) - 1);
	return num;
}

void  sdm_decode_rate (int step, int adj)
{
	u16 sdm_rptr = reg_aud_rptr; //get_sdm_rd_ptr ();
	u16 num = ((buffer_sdm_wptr>>1) - sdm_rptr) & ((TL_SDM_BUFFER_SIZE>>2) - 1);

	if (num > (TL_SDM_BUFFER_SIZE*3>>5)) {
		reg_ascl_step = step + adj;
	}
	else if (num < (TL_SDM_BUFFER_SIZE>>4)) {
		reg_ascl_step = step - adj;
	}
}


void proc_sdm_decoder (void)
{

}

int  sdm_decode_data (int *ps, int nbyte)
{
	return 0;
}

#endif

