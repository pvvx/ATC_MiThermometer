/********************************************************************************************************
 * @file     emi.c
 *
 * @brief    This is the source file for TLSR8258
 *
 * @author	 Driver Group
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
 * @par      History:
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/
#include "emi.h"
#include "clock.h"
#include "timer.h"


#define STATE0		0x1234
#define STATE1		0x5678
#define STATE2		0xabcd
#define STATE3		0xef01

static unsigned char  emi_rx_packet[64] __attribute__ ((aligned (4)));
static unsigned char  emi_zigbee_tx_packet[48]  __attribute__ ((aligned (4))) = {19,0,0,0,20,0,0};
static unsigned char  emi_ble_tx_packet [48]  __attribute__ ((aligned (4))) = {39, 0, 0, 0,0, 37};
static unsigned int   emi_rx_cnt=0,emi_rssibuf=0;
static signed  char   rssi=0;
static unsigned int   state0,state1;

/**
 * @brief   This function serves to set singletone power.
 * @param   level - the power level.
 * @return  none.
 */
void rf_set_power_level_index_singletone (RF_PowerTypeDef level)
{
	unsigned char value;

	if(level&BIT(7))
	{
		write_reg8(0x1225, (read_reg8(0x1225) & 0xbf) | ((0x01)<<6));//VANT
	}
	else
	{
		write_reg8(0x1225, (read_reg8(0x1225) & 0xbf));
	}
	write_reg8 (0xf02, 0x55);  //tx_en
	sleep_us(150);

	value = (unsigned char)level&0x3f;

	sub_wr(0x1378, 1, 6, 6);  //TX_PA_PWR_OW
	sub_wr(0x137c, value , 6, 1);  //TX_PA_PWR  0x3f18
}

/**
 * @brief   This function serves to set singletone power and channel.
 * @param   power_level - the power level.
 * @param   rf_chn - the channel.
 * @return  none.
 */
void rf_emi_single_tone(RF_PowerTypeDef power_level,signed char rf_chn)
{
	rf_drv_init(RF_MODE_ZIGBEE_250K);
	rf_set_channel_singletone(rf_chn);//set freq
	write_reg8 (0xf02, 0x45);  //tx_en
//	rf_set_power_level_index_singletone(power_level);
	rf_set_power_level_index_singletone(rf_power_Level_list[power_level]);
}


/**
 * @brief   This function serves to close RF
 * @param   none.
 * @return  none.
 */
void rf_emi_stop(void)
{
	write_reg8(0x1378, 0);
	write_reg8(0x137c, 0);//TX_PA_PWR
	rf_set_power_level_index (0);
	rf_set_tx_rx_off();
}

/**
 * @brief   This function serves to set rx mode and channel.
 * @param   mode - mode of RF
 * @param   rf_chn - the rx channel.
 * @return  none.
 */
void rf_emi_rx(RF_ModeTypeDef mode,signed char rf_chn)
{
	write_reg32 (0x408, 0x29417671);	//accesscode: 1001-0100 1000-0010 0110-1110 1000-1110   29 41 76 71
	write_reg8 (0x405, read_reg8(0x405)|0x80); //trig accesscode

	rf_rx_buffer_set(emi_rx_packet,64,0);
	rf_drv_init(mode);
	rf_set_channel(rf_chn,0);//set freq
	rf_set_tx_rx_off();
	rf_set_rxmode();
	sleep_us(150);
	rssi = 0;
	emi_rssibuf = 0;
	emi_rx_cnt = 0;
}

/**
 * @brief   This function serves is receiving service program
 * @param   none.
 * @return  none.
 */
void rf_emi_rx_loop(void)
{
	if(read_reg8(0xf20)&BIT(0))
	{
		if((read_reg8(0x44f)&0x0f)==0)
		{
			emi_rssibuf +=  (read_reg8(0x449));
			if(emi_rx_cnt)
				if(emi_rssibuf!=0)
				emi_rssibuf>>=1;
			rssi = emi_rssibuf-110;
			emi_rx_cnt++;
		}
		write_reg8(0xf20, 1);
		write_reg8(0xf00, 0x80);
	}
}

/**
 * @brief   This function serves to get the number of packets received
 * @param   none.
 * @return  the number of packets received.
 */
unsigned int rf_emi_get_rxpkt_cnt(void)
{
	return emi_rx_cnt;
}

/**
 * @brief   This function serves to get the RSSI of packets received
 * @param   none.
 * @return  the RSSI of packets received.
 */
char rf_emi_get_rssi_avg(void)
{
	return rssi;
}

/**
 * @brief   This function serves to get the address of the received packets
 * @param   none.
 * @return  the address of the received packets
 */
unsigned char *rf_emi_get_rxpkt(void)
{
	return emi_rx_packet;
}

/**
 * @brief   This function serves to generate random number.
 * @param   the old random number.
 * @return  the new random number.
 */
static unsigned int pnGen(unsigned int state)
{
	unsigned int feed = 0;
	feed = (state&0x4000) >> 1;
	state ^= feed;
	state <<= 1;
	state = (state&0xfffe) + ((state&0x8000)>>15);
	return state;
}

/**
 * @brief   This function serves to Set the CD mode correlation register.
 * @param   none.
 * @return  none.
 */
static void rf_continue_mode_setup(void)
{

	write_reg8(0x400,0x0a);
	write_reg8(0x408,0x00);

	write_reg8(0x401,0x80);//kick tx controller to wait data
	state0 = STATE0;
	state1 = STATE1;
}

/**
 * @brief   This function serves to continue to send CD mode.
 * @param   none.
 * @return  none.
 */
void rf_continue_mode_run(void)
{
	if(read_reg8(0x408) == 1){
		write_reg32(0x41c, 0x0f0f0f0f);
	}else if(read_reg8(0x408)==2){
		write_reg32(0x41c, 0x55555555);
	}else if(read_reg8(0x408)==3){
		write_reg32(0x41c, read_reg32(0x409));
	}else if(read_reg8(0x408)==4){
		write_reg32(0x41c, 0);
	}else if(read_reg8(0x408)==5){
		write_reg32(0x41c, 0xffffffff);
	}else{
		write_reg32(0x41c, (state0<<16)+state1);
		state0 = pnGen(state0);
		state1 = pnGen(state1);
	}

	while(read_reg8(0x41c) & 0x1){
	}
}




/**
 * @brief   This function serves to init the CD mode.
 * @param   rf_mode - mode of RF.
 * @param   power_level - power level of RF.
 * @param   rf_chn - channel of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_continue_setup(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type)
{
	rf_drv_init(rf_mode);//RF_MODE_BLE_1M
	rf_set_channel(rf_chn,0);
	write_reg8(0xf02, 0x45);  //tx_en
	rf_set_power_level_index_singletone (rf_power_Level_list[power_level]);
	rf_continue_mode_setup();
	write_reg8(0x408, pkt_type);//0:pbrs9 	1:0xf0	 2:0x55
}



/**
 * @brief   This function serves to generate random packets that need to be sent in burst mode
 * @param   *p - the address of random packets.
 * @param   n - the number of random packets.
 * @return  none.
 */
static void rf_phy_test_prbs9 (unsigned char *p, int n)
{
	//PRBS9: (x >> 1) | (((x<<4) ^ (x<<8)) & 0x100)
	unsigned short x = 0x1ff;
	int i;
	int j;
	for ( i=0; i<n; i++)
	{
		unsigned char d = 0;
		for (j=0; j<8; j++)
		{
			if (x & 1)
			{
				d |= BIT(j);
			}
			x = (x >> 1) | (((x<<4) ^ (x<<8)) & 0x100);
		}
		*p++ = d;
	}
}

/**
 * @brief   This function serves to init the burst mode.
 * @param   rf_mode - mode of RF.
 * @param   power_level - power level of RF.
 * @param   rf_chn - channel of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_burst_setup(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type)
{
//#if 0
	unsigned char i;
	unsigned char tx_data=0;

	write_reg32(0x408,0x29417671 );//access code  0xf8118ac9
	write_reg8 (0x405, read_reg8(0x405)|0x80);
	write_reg8(0x13c,0x10); // print buffer size set
	rf_set_channel(rf_chn,0);
	rf_drv_init(rf_mode);

	rf_set_power_level_index ((unsigned char)rf_power_Level_list[power_level&0x3f]);
	if(pkt_type==1)  tx_data = 0x0f;
	else if(pkt_type==2)  tx_data = 0x55;


	switch(rf_mode)
	{
		case RF_MODE_LR_S2_500K:
		case RF_MODE_LR_S8_125K:
		case RF_MODE_BLE_1M_NO_PN:
		case RF_MODE_BLE_2M:
			emi_ble_tx_packet[4] = pkt_type;//type
			for( i=0;i<37;i++)
			{
				emi_ble_tx_packet[6+i]=tx_data;
			}
			break;

		case RF_MODE_ZIGBEE_250K:
			emi_zigbee_tx_packet[5] = pkt_type;//type
			for( i=0;i<37;i++)
			{
				emi_zigbee_tx_packet[5+i]=tx_data;
			}
			break;

		default:
			break;
	}
}

/**
 * @brief   This function serves to init the burst mode with PA ramp up/down.
 * @param   rf_mode - mode of RF.
 * @param   power_level - power level of RF.
 * @param   rf_chn - channel of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_brust_setup_ramp(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type)
{
//#if 0
	unsigned char i;
	unsigned char tx_data=0;

	write_reg32(0x408,0x29417671 );//access code  0xf8118ac9
	write_reg8 (0x405, read_reg8(0x405)|0x80);
	write_reg8(0x13c,0x10); // print buffer size set
	rf_set_channel(rf_chn,0);
	rf_drv_init(rf_mode);
	rf_set_tx_rx_off();

	rf_set_power_level_index_singletone(rf_power_Level_list[power_level]);
	if(pkt_type==1)  tx_data = 0x0f;
	else if(pkt_type==2)  tx_data = 0x55;


	switch(rf_mode)
	{
		case RF_MODE_LR_S2_500K:
		case RF_MODE_LR_S8_125K:
		case RF_MODE_BLE_1M_NO_PN:
		case RF_MODE_BLE_2M:
			emi_ble_tx_packet[4] = pkt_type;//type
			for( i=0;i<37;i++)
			{
				emi_ble_tx_packet[6+i]=tx_data;
			}
			break;

		case RF_MODE_ZIGBEE_250K:
			emi_zigbee_tx_packet[5] = pkt_type;//type
			for( i=0;i<37;i++)
			{
				emi_zigbee_tx_packet[5+i]=tx_data;
			}
			break;

		default:
			break;
	}
}




/**
 * @brief   This function serves to send packets in the burst mode with PA ramp up/down.
 * @param   rf_mode - mode of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_burst_loop_ramp(RF_ModeTypeDef rf_mode,unsigned char pkt_type)
{
	write_reg8(0xf00, 0x80); // stop SM
	int power = (unsigned char)rf_power_Level_list[read_reg8(0x40008)];
	if((rf_mode==RF_MODE_BLE_1M_NO_PN)||(rf_mode==RF_MODE_BLE_2M)||(rf_mode==RF_MODE_LR_S8_125K)||(rf_mode==RF_MODE_LR_S2_500K))//ble
	{
		for(int i=0;i<=power;i++)
		{
			sub_wr(0x137c, i , 6, 1);
		}
		rf_tx_pkt(emi_ble_tx_packet);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();

		for(int i=power;i>=0;i--)
		{
			sub_wr(0x137c, i , 6, 1);
		}
		sleep_ms(2);
		if(pkt_type==0)
			rf_phy_test_prbs9(&emi_ble_tx_packet[6],37);
	}
	else if(rf_mode==RF_MODE_ZIGBEE_250K)//zigbee
	{
		for(int i=0;i<=power;i++)
			sub_wr(0x137c, i , 6, 1);

		rf_tx_pkt(emi_zigbee_tx_packet);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();
		for(int i=power;i>=0;i--)
			sub_wr(0x137c, i , 6, 1);
		sleep_ms(4);
		if(pkt_type==0)
			rf_phy_test_prbs9(&emi_zigbee_tx_packet[5],37);
	}
}

/**
 * @brief   This function serves to send packets in the burst mode
 * @param   rf_mode - mode of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_burst_loop(RF_ModeTypeDef rf_mode,unsigned char pkt_type)
{
	write_reg8(0xf00, 0x80); // stop SM

	if((rf_mode==RF_MODE_BLE_1M_NO_PN)||(rf_mode==RF_MODE_BLE_2M))//ble
	{

		rf_start_stx ((void *)emi_ble_tx_packet, read_reg32(0x740) + 10);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();

		sleep_ms(2);//
//		delay_us(625);//
		if(pkt_type==0)
			rf_phy_test_prbs9(&emi_ble_tx_packet[6],37);
	}
	else if(rf_mode==RF_MODE_LR_S8_125K)//ble
	{
		rf_start_stx ((void *)emi_ble_tx_packet, read_reg32(0x740) + 10);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();
		sleep_ms(2);//
		if(pkt_type==0)
			rf_phy_test_prbs9(&emi_ble_tx_packet[6],37);
	}
	else if(rf_mode==RF_MODE_LR_S2_500K)//ble
	{
		rf_start_stx ((void *)emi_ble_tx_packet, read_reg32(0x740) + 10);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();
		sleep_ms(2);//625*1.5
		if(pkt_type==0)
			rf_phy_test_prbs9(&emi_ble_tx_packet[6],37);
	}
	else if(rf_mode==RF_MODE_ZIGBEE_250K)//zigbee
	{

		rf_start_stx ((void *)emi_zigbee_tx_packet, read_reg32(0x740) + 10);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();
		sleep_us(625*2);//
		if(pkt_type==0)
			rf_phy_test_prbs9(&emi_zigbee_tx_packet[5],37);
	}
}

/**
 * @brief   This function serves to set the channel in singletone mode.
 * @param   chn - channel of RF.
 * @return  none.
 */
void rf_set_channel_singletone (signed char chn)//general
{
	unsigned short rf_chn =0;
	unsigned char ctrim;
	unsigned short chnl_freq;

	rf_chn = chn+2400;

	if (rf_chn >= 2550)
	    ctrim = 0;
	else if (rf_chn >= 2520)
	    ctrim = 1;
	else if (rf_chn >= 2495)
	    ctrim = 2;
	else if (rf_chn >= 2465)
	    ctrim = 3;
	else if (rf_chn >= 2435)
		ctrim = 4;
	else if (rf_chn >= 2405)
	    ctrim = 5;
	else if (rf_chn >= 2380)
	    ctrim = 6;
	else
	    ctrim = 7;

	chnl_freq = rf_chn * 2 +1;

	write_reg8(0x1244, ((chnl_freq & 0x7f)<<1) | 1  );   //CHNL_FREQ_DIRECT   CHNL_FREQ_L
	write_reg8(0x1245,  ((read_reg8(0x1245) & 0xc0)) | ((chnl_freq>>7)&0x3f) );  //CHNL_FREQ_H
	write_reg8(0x1229,  (read_reg8(0x1229) & 0xC3) | (ctrim<<2) );  //FE_CTRIM

}


