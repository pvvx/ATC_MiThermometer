/*
 * llms_scan.h
 *
 *  Created on: 2019-5-25
 *      Author: Administrator
 */

#ifndef LLMS_SCAN_H_
#define LLMS_SCAN_H_



/************************************ User Interface  ******************************************************/
void 	blc_llms_initScanning_module(void);



/*********************************** Stack Interface, user can not use!!! **********************************/

void 	blt_llms_switchScanChannel (int set_chn);
int  	blt_llms_procScanPkt(u8 *raw_pkt, u8 *new_pkt, u32 tick_now);
int 	blt_llms_procScanData(u8 *raw_pkt);

#endif /* LLMS_SCAN_H_ */
