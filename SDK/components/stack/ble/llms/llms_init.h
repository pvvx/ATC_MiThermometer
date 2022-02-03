/*
 * llms_init.h
 *
 *  Created on: 2019-5-25
 *      Author: Administrator
 */

#ifndef LLMS_INIT_H_
#define LLMS_INIT_H_








extern	int blms_create_connection;
extern	u32 blms_timeout_connectDevice;
extern	u32	blms_tick_connectDevice;



/************************************ User Interface  ******************************************************/




/*********************************** Stack Interface, user can not use!!! **********************************/
int  blt_llms_procInitPkt(u8 *raw_pkt);




#endif /* LLMS_INIT_H_ */
