/********************************************************************************************************
 * @file     usbkb.c 
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

#include "tl_common.h"

#if(1)




#include "drivers.h"
#include "usbkb.h"
#include "usbmouse.h"
#include "../usbstd/usb.h"
#include "../usbstd/usbhw.h"
#include "../usbstd/usbhw_i.h"
#include "../usbstd/usbkeycode.h"

#include "../rf_frame.h"


u8 usb_fifo[USB_FIFO_NUM][USB_FIFO_SIZE];
u8 usb_ff_rptr = 0;
u8 usb_ff_wptr = 0;

int usbkb_hid_report_normal(u8 ctrl_key, u8 *keycode);
void usbkb_hid_report(kb_data_t *data);

static u8 vk_sys_map[VK_SYS_CNT] = {
	VK_POWER_V, VK_SLEEP_V, VK_WAKEUP_V
};

static vk_ext_t vk_media_map[VK_MEDIA_CNT] = {
    {VK_W_SRCH_V},
    {VK_HOME_V},
    {VK_W_BACK_V},
    {VK_W_FORWRD_V},
    {VK_W_STOP_V},
    {VK_W_REFRESH_V},
    {VK_W_FAV_V},
    {VK_MEDIA_V},
    {VK_MAIL_V},
    {VK_CAL_V},
    {VK_MY_COMP_V},
    {VK_NEXT_TRK_V},
    {VK_PREV_TRK_V},
    {VK_STOP_V},
    {VK_PLAY_PAUSE_V},
    {VK_W_MUTE_V},
    {VK_VOL_UP_V},
    {VK_VOL_DN_V},
};

enum{
    KB_NORMAL_RELEASE_MASK = BIT(0),
    KB_SYS_RELEASE_MASK = BIT(1),
    KB_MEDIA_RELEASE_MASK = BIT(2),
};



#define  USBKB_BUFF_DATA_NUM    8
//static
kb_data_t kb_dat_buff[USBKB_BUFF_DATA_NUM];

//static
u8  usbkb_wptr, usbkb_rptr;
static u32 usbkb_not_released;
static u32 usbkb_data_report_time;


void usbkb_add_frame (rf_packet_keyboard_t *packet_kb)
{
	u8 new_data_num = packet_kb->pno;  //根据pno 获得最新数据的个数
	for(u8 i=0;i<new_data_num;i++)
	{
			memcpy4((int*)(&kb_dat_buff[usbkb_wptr]), (int*)(&packet_kb->data[i*sizeof(kb_data_t)]), sizeof(kb_data_t));
			BOUND_INC_POW2(usbkb_wptr,USBKB_BUFF_DATA_NUM);
			if(usbkb_wptr == usbkb_wptr)
			{
					break;
			}
	}
}


void usbkb_report_frame(void)
{
	if(usbkb_wptr != usbkb_rptr){
		kb_data_t *data = (kb_data_t *)(&kb_dat_buff[usbkb_rptr]);
        usbkb_hid_report(data);
		BOUND_INC_POW2(usbkb_rptr,USBKB_BUFF_DATA_NUM);
	}
	return;
}

static void usbkb_release_normal_key(void){
	if(usbkb_not_released & KB_NORMAL_RELEASE_MASK){
		u8 normal_keycode[KEYBOARD_REPORT_KEY_MAX] = {0};
		if(usbkb_hid_report_normal(0, normal_keycode)){
			BM_CLR(usbkb_not_released, KB_NORMAL_RELEASE_MASK);
		}
	}
}

static void usbkb_release_sys_key(void){
	if(usbkb_not_released & KB_SYS_RELEASE_MASK){
		u32 release_data = 0;
		if(usbmouse_hid_report(USB_HID_KB_SYS, (u8*)(&release_data), 1)){
			BM_CLR(usbkb_not_released, KB_SYS_RELEASE_MASK);
		}
	}
}

static void usbkb_release_media_key(void){
	if(usbkb_not_released & KB_MEDIA_RELEASE_MASK){
		u8 ext_keycode[MOUSE_REPORT_DATA_LEN] = {0};
		if(usbmouse_hid_report(USB_HID_KB_MEDIA, ext_keycode, MEDIA_REPORT_DATA_LEN)){
			BM_CLR(usbkb_not_released, KB_MEDIA_RELEASE_MASK);
		}
	}
}

static void usbkb_release_keys(void){
	// 不要调换usbkb_release_sys_key 和usbkb_release_normal_key 的顺序，
	// 这是为了让 usbkb_release_sys_key,  usbkb_release_media_key 中间有一定延时
	usbkb_release_sys_key();
	usbkb_release_normal_key();
	usbkb_release_media_key();
}

void usbkb_release_check(){
	if(usbkb_not_released && clock_time_exceed(usbkb_data_report_time, USB_KEYBOARD_RELEASE_TIMEOUT)){
		usbkb_release_keys();	 //  release keys
	}

}

//  normal_keycode 里面返回普通按键
int usbkb_separate_key_types(u8 *keycode, u8 cnt, u8 *normal_key, u8 *ext_key){
    STATIC_ASSERT(KB_RETURN_KEY_MAX <= KEYBOARD_REPORT_KEY_MAX);
    assert(cnt <= KB_RETURN_KEY_MAX);

    int normal_cnt = 0;
    foreach(i, cnt){
        if(keycode[i] >= VK_EXT_START && keycode[i] < VK_EXT_END){
        	*ext_key = keycode[i];
        }else{
            normal_key[normal_cnt++] = keycode[i];
        }
    }
	return normal_cnt;
}

int usbkb_hid_report_normal(u8 ctrl_key, u8 *keycode){

	if(usbhw_is_ep_busy(USB_EDP_KEYBOARD_IN)){

		u8 *pData = (u8 *)&usb_fifo[usb_ff_wptr++ & (USB_FIFO_NUM - 1)];
		pData[0] = DAT_TYPE_KB;
		pData[1] = ctrl_key;
		memcpy(pData + 2, keycode, 6);

		int fifo_use = (usb_ff_wptr - usb_ff_rptr) & (USB_FIFO_NUM*2-1);
		if (fifo_use > USB_FIFO_NUM) {
			usb_ff_rptr++;
			//fifo overflow, overlap older data
		}

		return 0;
	}

	reg_usb_ep_ptr(USB_EDP_KEYBOARD_IN) = 0;




#if USB_SOFTWARE_CRC_CHECK

	unsigned char crc_in[KEYBOARD_REPORT_KEY_MAX+2];
	unsigned short crc;
	unsigned int crch;

	crc_in[0] =  ctrl_key;
	crc_in[1] =  0;
	foreach(i, KEYBOARD_REPORT_KEY_MAX){
		crc_in[i+2] = keycode[i];
	}
	crc = USB_CRC16 (crc_in, KEYBOARD_REPORT_KEY_MAX+2);
	crch = crc >> 8;

	if ((crch==0x06) || (crch==0x04) || (crch == 0x00))
	{
		unsigned int tmp = crc_in[2];
		crc_in[2] = crc_in[3];
		crc_in[3] = tmp;
	}
	// please refer to keyboard_report_desc
	foreach(i, (KEYBOARD_REPORT_KEY_MAX+2)){
	    reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = crc_in[i];
	}

#else

	// please refer to keyboard_report_desc
	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = ctrl_key;
	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = 0;//resv
	foreach(i, KEYBOARD_REPORT_KEY_MAX){
	    reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = keycode[i];
	}

#endif
//	reg_usb_ep_ctrl(USB_EDP_KEYBOARD_IN) = FLD_EP_DAT_ACK;		// ACK
	reg_usb_ep_ctrl(USB_EDP_KEYBOARD_IN) = FLD_EP_DAT_ACK | (edp_toggle[USB_EDP_KEYBOARD_IN] ? FLD_USB_EP_DAT1 : FLD_USB_EP_DAT0);  // ACK
	edp_toggle[USB_EDP_KEYBOARD_IN] ^= 1;

	return 1;
}

static inline void usbkb_report_normal_key(int ctrl_key, u8 *keycode, int cnt){
	if(cnt > 0 || ctrl_key){
		if(usbkb_hid_report_normal(ctrl_key, keycode)){
		    BM_SET(usbkb_not_released, KB_NORMAL_RELEASE_MASK);
		}
	}else{
		usbkb_release_normal_key();
	}
}

static inline void usbkb_report_sys_key(u8 ext_key){
	if(ext_key >= VK_SYS_START && ext_key < VK_SYS_END){
		int idx = ext_key - VK_SYS_START;
		if(usbmouse_hid_report(USB_HID_KB_SYS, (u8*)(&vk_sys_map[idx]), 1)){	// assert sys key len == 1, check descriptor
			BM_SET(usbkb_not_released, KB_SYS_RELEASE_MASK);
		}
	}else{
		usbkb_release_sys_key();
	}
}

static inline void usbkb_report_media_key(u8 ext_key){
	if(ext_key >= VK_MEDIA_START && ext_key < VK_MEDIA_END){
		STATIC_ASSERT(VK_EXT_LEN <= MOUSE_REPORT_DATA_LEN);

		u8 ext_keycode[MOUSE_REPORT_DATA_LEN] = {0};

		int idx = ext_key - VK_MEDIA_START;
		foreach(i, VK_EXT_LEN){
			ext_keycode[i] = vk_media_map[idx].val[i];
		}
		if(usbmouse_hid_report(USB_HID_KB_MEDIA, ext_keycode, MEDIA_REPORT_DATA_LEN)){
			BM_SET(usbkb_not_released, KB_MEDIA_RELEASE_MASK);
		}
	}else{
		usbkb_release_media_key();
	}
}


void usbkb_report_consumer_key(u16 consumer_key)
{
	if(consumer_key){

		u8 ext_keycode[MOUSE_REPORT_DATA_LEN] = {0};

		foreach(i, VK_EXT_LEN){
			ext_keycode[i] = consumer_key;
			consumer_key >>=8;
		}

		if(usbmouse_hid_report(USB_HID_KB_MEDIA, ext_keycode, MEDIA_REPORT_DATA_LEN)){
			BM_SET(usbkb_not_released, KB_MEDIA_RELEASE_MASK);
		}
	}else{
		usbkb_release_media_key();
	}


    usbkb_data_report_time = clock_time();

}


int kb_is_data_same(kb_data_t *a, kb_data_t *b){
	if(!a || !b){
		return 0;
	}
	if((a->cnt != b->cnt) || (a->ctrl_key != b->ctrl_key))
		return 0;
	foreach(i, a->cnt){
		if(a->keycode[i] != b->keycode[i]){
			return 0;
		}
	}
	return 1;
}

static inline int usbkb_check_repeat_and_save(kb_data_t *data){
    assert(data);
	static kb_data_t last_data;
	int same = kb_is_data_same(&last_data, data);
	if(!same){
		STATIC_ASSERT(sizeof(last_data) == 8);
	    ((u32*) (&last_data))[0] = ((u32*) (data))[0];
	    ((u32*) (&last_data))[1] = ((u32*) (data))[1];
	}
	return same;
}


void usbkb_hid_report(kb_data_t *data){
    assert(data);
    u8 ext_key = VK_EXT_END, normal_key_cnt = 0;
    u8 normal_keycode[KEYBOARD_REPORT_KEY_MAX] = {0};

	if(data->cnt > KB_RETURN_KEY_MAX){	// must,   in case bad packets
		return;
	}

	/*	http://msdn.microsoft.com/en-us/windows/hardware/gg462991.aspx
		It is also important to notice that any re-triggering of events should be done by software timers in the host system and not by hardware timers in the device itself.
		For example, if the user keeps pressing the Volume Increment button, the device should only generate one input report with this state information
	*/
	if((data->cnt > 0 || data->ctrl_key) && usbkb_check_repeat_and_save(data)){
		if(usbkb_not_released){
			usbkb_data_report_time = clock_time();
			return;
		}
	}


	if(data->cnt > 0){
		//  keycode分类处理
	    normal_key_cnt = usbkb_separate_key_types(data->keycode, data->cnt, normal_keycode, &ext_key);
	}

	usbkb_report_normal_key(data->ctrl_key, normal_keycode, normal_key_cnt);
	usbkb_report_sys_key(ext_key);
	usbkb_report_media_key(ext_key);

    usbkb_data_report_time = clock_time();
}


void usbkb_init(){
	//ev_on_poll(EV_POLL_KEYBOARD_RELEASE_CHECK, usbkb_release_check);
}


int usb_hid_report_fifo_proc(void)
{
	if(usb_ff_rptr == usb_ff_wptr){
		return 0;
	}

	u8 *pData = (u8 *)&usb_fifo[usb_ff_rptr & (USB_FIFO_NUM - 1)];

	if(pData[0] == DAT_TYPE_KB){
		if(usbhw_is_ep_busy(USB_EDP_KEYBOARD_IN)){
			return 0;
		}
		else{

			usbkb_hid_report_normal(pData[1], pData + 2);

			usb_ff_rptr ++;

			return 1;
		}
	}
	else if(pData[0] == DAT_TYPE_MOUSE){
		if(usbhw_is_ep_busy(USB_EDP_MOUSE)){
			return 0;
		}
		else{

			usbmouse_hid_report(pData[1], pData + 4, pData[2]);

			usb_ff_rptr ++;

			return 1;
		}
	}


	return 0;
}


#endif
