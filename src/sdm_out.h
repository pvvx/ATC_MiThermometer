/*
 * sdm_out.h
 *
 *  Created on: 17 апр. 2025 г.
 *      Author: pvvx
 */

#ifndef SDM_OUT_H_
#define SDM_OUT_H_
typedef struct {
	s16 in_min;
	s16 in_max;
	u16 out_min;
	u16 out_max;
} cfg_dac_t;

typedef struct {
	cfg_dac_t cfg;
	s32 k;
} cfg_sdmdac_t;

extern cfg_sdmdac_t sdmdac;

void set_dac(void);
void init_dac(void);
//void sdm_out(signed short value_dac0, signed short value_dac1);


#endif /* SDM_OUT_H_ */
