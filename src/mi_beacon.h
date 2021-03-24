/*
 * mi_beacon.h
 *
 *  Created on: 17.02.2021
 *      Author: pvvx
 */

#ifndef MI_BEACON_H_
#define MI_BEACON_H_

extern uint8_t bindkey[16];
extern uint8_t *pbindkey;

void mi_beacon_summ(void); // averaging measurements
void mi_encrypt_beacon(uint32_t cnt);
void mi_beacon_init(void);

#endif /* MI_BEACON_H_ */
