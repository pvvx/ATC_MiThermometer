/********************************************************************************************************
 * @file     keyboard.c 
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
#include "drivers.h"
#include "keyboard.h"
#include "../usbstd/usbkeycode.h"


#if (defined(KB_DRIVE_PINS) && defined(KB_SCAN_PINS))

u32 drive_pins[] = KB_DRIVE_PINS;
u32 scan_pins[] = KB_SCAN_PINS;

#if (STUCK_KEY_PROCESS_ENABLE)
unsigned char stuckKeyPress[ARRAY_SIZE(drive_pins)];
#endif

kb_data_t	kb_event;
kb_data_t	kb_event_cache;
unsigned char  deepback_key_state;
u32 deepback_key_tick;

#ifndef		SCAN_PIN_50K_PULLUP_ENABLE
#define		SCAN_PIN_50K_PULLUP_ENABLE		0
#endif

#ifndef		KB_MAP_DEFAULT
#define		KB_MAP_DEFAULT		1
#endif

#ifndef		KB_LINE_MODE
#define		KB_LINE_MODE		0
#endif

#ifndef		KB_LINE_HIGH_VALID
#define		KB_LINE_HIGH_VALID		1
#endif

#ifndef		KB_KEY_FLASH_PIN_MULTI_USE
#define		KB_KEY_FLASH_PIN_MULTI_USE		0
#endif

#ifndef		KB_HAS_CTRL_KEYS
#define		KB_HAS_CTRL_KEYS		1
#endif

#ifndef		KB_RM_GHOST_KEY_EN
#define		KB_RM_GHOST_KEY_EN		0
#endif

#ifndef		KB_HAS_FN_KEY
#define		KB_HAS_FN_KEY		1
#endif

#ifndef		KB_DRV_DELAY_TIME
#define		KB_DRV_DELAY_TIME		10
#endif


#if  KB_REPEAT_KEY_ENABLE

#ifndef		KB_REPEAT_KEY_INTERVAL_MS
#define		KB_REPEAT_KEY_INTERVAL_MS		200
#endif
#ifndef		KB_REPEAT_KEY_NUM
#define		KB_REPEAT_KEY_NUM				4
#endif
static const unsigned char kb_map_repeat[KB_REPEAT_KEY_NUM] = KB_MAP_REPEAT;

repeatKey_t repeat_key = {
	0,
	0,
	0,
	0,
	U32_MAX,
};

#endif

typedef unsigned char kb_k_mp_t[ARRAY_SIZE(drive_pins)]; //typedef unsigned char kb_k_mp_t[8]

#if		KB_MAP_DEFAULT

#ifndef			KB_MAP_NORMAL
static const unsigned char kb_map_normal[ARRAY_SIZE(scan_pins)][ARRAY_SIZE(drive_pins)] = {
	{VK_PAUSE,	 VK_POWER,	  VK_EURO,		VK_SLEEP,	 	VK_RCTRL,	  VK_WAKEUP,	VK_CTRL,	    VK_F5},
	{VK_Q,		 VK_TAB,	  VK_A,		    VK_ESC,		    VK_Z,		  VK_NCHG,	  	VK_TILDE,	    VK_1},
	{VK_W,		 VK_CAPITAL,  VK_S,	        VK_K45,	    	VK_X,		  VK_CHG,	  	VK_F1,			VK_2},
	{VK_E,		 VK_F3,		  VK_D,		    VK_F4,		    VK_C,		  VK_ROMA,	  	VK_F2,			VK_3},
	{VK_R,		 VK_T,		  VK_F,		    VK_G,			VK_V,		  VK_B,		  	VK_5,			VK_4},
	{VK_U,		 VK_Y,		  VK_J,		    VK_H,			VK_M,		  VK_N,		 	VK_6,			VK_7},
	{VK_I,		 VK_RBRACE,	  VK_K,		    VK_F6,		    VK_COMMA,	  VK_K56,	  	VK_EQUAL,		VK_8},
	{VK_O,	     VK_F7,  	  VK_L,   	    VK_RMB,	 		VK_PERIOD,    VK_APP, 	 	VK_F8,			VK_9},
	{VK_P,		 VK_LBRACE,	  VK_SEMICOLON, VK_QUOTE,		VK_BACKSLASH, VK_SLASH,	  	VK_MINUS,		VK_0},
	{VK_SCR_LOCK,VK_C9R1,	  VK_FN,	    VK_ALT,		    VK_MMODE,	  VK_RALT,	  	VK_C9R6,		VK_PRINTSCREEN},
	{VK_K14,	 VK_BACKSPACE,VK_BACKSLASH,	VK_F11,		    VK_ENTER,	  VK_F12,	  	VK_F9,			VK_F10},
	{VK_HOME,	 VK_LEFT,	  VK_END,	    VK_SPACE,		VK_NUM_LOCK,  VK_DOWN,	  	VK_DELETE,		VK_POWER},
	{VK_UP,	 	 VK_NONE,	  VK_DOWN,	    VK_INSERT,		VKPAD_SLASH,  VK_RIGHT,	  	VK_INSERT,		VK_SLEEP},
	{VK_PAGE_UP, VK_RIGHT,	  VK_PAGE_DOWN,	VKPAD_PERIOD,	VKPAD_ASTERIX,VKPAD_MINUS,	VK_PAGE_UP,		VK_PAGE_DOWN},
	{VKPAD_PLUS, VK_K107,	  VKPAD_ENTER,  VK_UP,		    VK_PLAY_PAUSE,VK_LEFT,	  	VK_HOME,	    VK_END},
	{VK_WAKEUP,	 VK_SHIFT,	  VK_RSHIFT,	VK_VOL_DN,	    VK_VOL_UP,	  VK_NEXT_TRK,	VK_PREV_TRK,	VK_MEDIA},
	{VK_MAIL,	 VK_WIN,	  VK_W_FORWRD,	VK_W_STOP,		VK_W_BACK,	  VK_W_REFRESH,	VK_W_MUTE,    	VK_W_SRCH},
	{VK_KCL,	 VK_W_FAV,	  VK_RWIN,		VK_MY_COMP,		VK_STOP,	  VK_CAL,	  	VK_WEB,	    	VK_KCR},
};
#else
static const unsigned char kb_map_normal[ARRAY_SIZE(scan_pins)][ARRAY_SIZE(drive_pins)] = KB_MAP_NORMAL;
#endif

#ifndef			KB_MAP_NUM
static const unsigned char kb_map_num[ARRAY_SIZE(scan_pins)][ARRAY_SIZE(drive_pins)] = {
	{VK_PAUSE,	 VK_POWER,	  VK_EURO,		VK_SLEEP,	 	VK_RCTRL,	  VK_WAKEUP,	VK_CTRL,	    VK_F5},
	{VK_Q,		 VK_TAB,	  VK_A,		    VK_ESC,		    VK_Z,		  VK_NCHG,	  	VK_TILDE,	    VK_1},
	{VK_W,		 VK_CAPITAL,  VK_S,	        VK_K45,	    	VK_X,		  VK_CHG,	  	VK_F1,			VK_2},
	{VK_E,		 VK_F3,		  VK_D,		    VK_F4,		    VK_C,		  VK_ROMA,	  	VK_F2,			VK_3},
	{VK_R,		 VK_T,		  VK_F,		    VK_G,			VK_V,		  VK_B,		  	VK_5,			VK_4},
	{VK_U,		 VK_Y,		  VK_J,		    VK_H,			VK_M,		  VK_N,		 	VK_6,			VK_7},
	{VK_I,		 VK_RBRACE,	  VK_K,		    VK_F6,		    VK_COMMA,	  VK_K56,	  	VK_EQUAL,		VK_8},
	{VK_O,	     VK_F7,  	  VK_L,   	    VK_RMB,	 		VK_PERIOD,    VK_APP, 	 	VK_F8,			VK_9},
	{VK_P,		 VK_LBRACE,	  VK_SEMICOLON, VK_QUOTE,		VK_BACKSLASH, VK_SLASH,	  	VK_MINUS,		VK_0},
	{VK_SCR_LOCK,VK_C9R1,	  VK_FN,	    VK_ALT,		    VK_MMODE,	  VK_RALT,	  	VK_C9R6,		VK_PRINTSCREEN},
	{VK_K14,	 VK_BACKSPACE,VK_BACKSLASH,	VK_F11,		    VK_ENTER,	  VK_F12,	  	VK_F9,			VK_F10},
	{VKPAD_7,	 VKPAD_4,	  VKPAD_1,	    VK_SPACE,		VK_NUM_LOCK,  VK_DOWN,	  	VK_DELETE,		VK_POWER},
	{VKPAD_8,	 VKPAD_5,	  VKPAD_2,	    VKPAD_0,		VKPAD_SLASH,  VK_RIGHT,	  	VK_INSERT,		VK_SLEEP},
	{VKPAD_9,	 VKPAD_6,	  VKPAD_3,	    VKPAD_PERIOD,	VKPAD_ASTERIX,VKPAD_MINUS,	VK_PAGE_UP,		VK_PAGE_DOWN},
	{VKPAD_PLUS, VK_K107,	  VKPAD_ENTER,  VK_UP,		    VK_PLAY_PAUSE,VK_LEFT,	  	VK_HOME,	    VK_END},
	{VK_WAKEUP,	 VK_SHIFT,	  VK_RSHIFT,	VK_VOL_DN,	    VK_VOL_UP,	  VK_NEXT_TRK,	VK_PREV_TRK,	VK_MEDIA},
	{VK_MAIL,	 VK_WIN,	  VK_W_FORWRD,	VK_W_STOP,		VK_W_BACK,	  VK_W_REFRESH,	VK_W_MUTE,    	VK_W_SRCH},
	{VK_KCL,	 VK_W_FAV,	  VK_RWIN,		VK_MY_COMP,		VK_STOP,	  VK_CAL,	  	VK_WEB,	    	VK_KCR},
};
#else
static const unsigned char kb_map_num[ARRAY_SIZE(scan_pins)][ARRAY_SIZE(drive_pins)] = KB_MAP_NUM;
#endif

#ifndef			KB_MAP_FN
static const unsigned char kb_map_fn[ARRAY_SIZE(scan_pins)][ARRAY_SIZE(drive_pins)] = {
	{VK_PAUSE,	 VK_POWER,	  VK_EURO,		VK_SLEEP,	 	VK_RCTRL,	  VK_WAKEUP,	VK_CTRL,	    VK_F5},
	{VK_Q,		 VK_TAB,	  VK_A,		    VK_ESC,		    VK_Z,		  VK_NCHG,	  	VK_TILDE,	    VK_1},
	{VK_W,		 VK_CAPITAL,  VK_S,	        VK_K45,	    	VK_X,		  VK_CHG,	  	VK_F1,			VK_2},
	{VK_E,		 VK_F3,		  VK_D,		    VK_F4,		    VK_C,		  VK_ROMA,	  	VK_F2,			VK_3},
	{VK_R,		 VK_T,		  VK_F,		    VK_G,			VK_V,		  VK_B,		  	VK_5,			VK_4},
	{VK_U,		 VK_Y,		  VK_J,		    VK_H,			VK_M,		  VK_N,		 	VK_6,			VK_7},
	{VK_I,		 VK_RBRACE,	  VK_K,		    VK_F6,		    VK_COMMA,	  VK_K56,	  	VK_EQUAL,		VK_8},
	{VK_O,	     VK_F7,  	  VK_L,   	    VK_RMB,	 		VK_PERIOD,    VK_APP, 	 	VK_F8,			VK_9},
	{VK_P,		 VK_LBRACE,	  VK_SEMICOLON, VK_QUOTE,		VK_BACKSLASH, VK_SLASH,	  	VK_MINUS,		VK_0},
	{VK_SCR_LOCK,VK_C9R1,	  VK_FN,	    VK_ALT,		    VK_MMODE,	  VK_RALT,	  	VK_C9R6,		VK_PRINTSCREEN},
	{VK_K14,	 VK_BACKSPACE,VK_BACKSLASH,	VK_F11,		    VK_ENTER,	  VK_F12,	  	VK_F9,			VK_F10},
	{VKPAD_7,	 VKPAD_4,	  VKPAD_1,	    VK_SPACE,		VK_NUM_LOCK,  VK_DOWN,	  	VK_DELETE,		VK_POWER},
	{VKPAD_8,	 VKPAD_5,	  VKPAD_2,	    VKPAD_0,		VKPAD_SLASH,  VK_RIGHT,	  	VK_INSERT,		VK_SLEEP},
	{VKPAD_9,	 VKPAD_6,	  VKPAD_3,	    VKPAD_PERIOD,	VKPAD_ASTERIX,VKPAD_MINUS,	VK_PAGE_UP,		VK_PAGE_DOWN},
	{VKPAD_PLUS, VK_K107,	  VKPAD_ENTER,  VK_UP,		    VK_PLAY_PAUSE,VK_LEFT,	  	VK_HOME,	    VK_END},
	{VK_WAKEUP,	 VK_SHIFT,	  VK_RSHIFT,	VK_VOL_DN,	    VK_VOL_UP,	  VK_NEXT_TRK,	VK_PREV_TRK,	VK_MEDIA},
	{VK_MAIL,	 VK_WIN,	  VK_W_FORWRD,	VK_W_STOP,		VK_W_BACK,	  VK_W_REFRESH,	VK_W_MUTE,    	VK_W_SRCH},
	{VK_KCL,	 VK_W_FAV,	  VK_RWIN,		VK_MY_COMP,		VK_STOP,	  VK_CAL,	  	VK_WEB,	    	VK_KCR},

};
#else
static const unsigned char kb_map_fn[ARRAY_SIZE(scan_pins)][ARRAY_SIZE(drive_pins)] = KB_MAP_FN;
#endif

kb_k_mp_t *	kb_p_map[4] = {
		kb_map_normal,
		kb_map_num,
		kb_map_fn,
		kb_map_fn,
};


///////////////////////////////////////////////////////////////////////
//////////// load configuration from flash/OTP ////////////////////////
///////////////////////////////////////////////////////////////////////
#else //line 62

#endif

u32	scan_pin_need;

static unsigned char 	kb_is_fn_pressed = 0;

kb_k_mp_t * kb_k_mp;

void kb_rmv_ghost_key(u32 * pressed_matrix){
	u32 mix_final = 0;
	foreach_arr(i, drive_pins){
		for(int j = (i+1); j < ARRAY_SIZE(drive_pins); ++j){
			u32 mix = (pressed_matrix[i] & pressed_matrix[j]);
			// >=2 根线重合,  那就是 ghost key
			//four or three key at "#" is pressed at the same time, should remove ghost key
			if( mix && (!BIT_IS_POW2(mix) || (pressed_matrix[i] ^ pressed_matrix[j])) ){
				// remove ghost keys
				//pressed_matrix[i] &= ~mix;
				//pressed_matrix[j] &= ~mix;
				mix_final |= mix;
			}
		}
		pressed_matrix[i] &= ~mix_final;
	}
}

#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
int key_matrix_same_as_last_cnt = 0;  //record key matrix no change cnt
#endif

unsigned int key_debounce_filter( u32 mtrx_cur[], u32 filt_en ){
    u32 kc = 0;
#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
    unsigned char matrix_differ = 0;
#endif
    static u32 mtrx_pre[ARRAY_SIZE(drive_pins)];
    static u32 mtrx_last[ARRAY_SIZE(drive_pins)];
    foreach_arr(i, drive_pins){
        u32 mtrx_tmp = mtrx_cur[i];
#if (STUCK_KEY_PROCESS_ENABLE)
        stuckKeyPress[i] = mtrx_tmp ? 1 : 0;
#endif
        if( filt_en ){
            //mtrx_cur[i] = (mtrx_last[i] ^ mtrx_tmp) ^ (mtrx_last[i] | mtrx_tmp);  //key_matrix_pressed is valid when current and last value is the same
            mtrx_cur[i] = ( ~mtrx_last[i] & (mtrx_pre[i] & mtrx_tmp) ) | ( mtrx_last[i] & (mtrx_pre[i] | mtrx_tmp) );
        }
        if ( mtrx_cur[i] != mtrx_last[i] ) {
        	kc = 1;
        }
#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
        if(mtrx_cur[i]^mtrx_pre[i]){  //when same, XOR value is 0
        	matrix_differ = 1;
        }
#endif
        mtrx_pre[i] = mtrx_tmp;
        mtrx_last[i] = mtrx_cur[i];
    }

#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
    if(matrix_differ){
    	key_matrix_same_as_last_cnt = 0;
    }
    else{
    	key_matrix_same_as_last_cnt++;
    }
#endif

    return kc;
}


// input:          pressed_matrix,
// key_code:   output keys array
// key_max:    max keys should be returned
static inline void kb_remap_key_row(int drv_ind, u32 m, int key_max, kb_data_t *kb_data){
	foreach_arr(i, scan_pins){
		if(m & 0x01){
			unsigned char kc = kb_k_mp[i][drv_ind];
#if(KB_HAS_CTRL_KEYS)

			if(kc >= VK_CTRL && kc <= VK_RWIN)
				kb_data->ctrl_key |= BIT(kc - VK_CTRL);
			//else if(kc == VK_MEDIA_END)
				//lock_button_pressed = 1;
			else if(VK_ZOOM_IN == kc || VK_ZOOM_OUT == kc){
				kb_data->ctrl_key |= VK_MSK_LCTRL;
				kb_data->keycode[kb_data->cnt++] = (VK_ZOOM_IN == kc)? VK_EQUAL : VK_MINUS;
			}
			else if(kc != VK_FN)//fix fn ghost bug
				kb_data->keycode[kb_data->cnt++] = kc;

#else
			kb_data->keycode[kb_data->cnt++] = kc;
#endif
			if(kb_data->cnt >= key_max){
				break;
			}
		}
		m = m >> 1;
		if(!m){
			break;
		}
	}
}

static inline void kb_remap_key_code(u32 * pressed_matrix, int key_max, kb_data_t *kb_data, int numlock_status){

	kb_k_mp = kb_p_map[(numlock_status&1) | (kb_is_fn_pressed << 1)];
	foreach_arr(i, drive_pins){
		u32 m = pressed_matrix[i];
		if(!m) continue;
		kb_remap_key_row(i, m, key_max, kb_data);
		if(kb_data->cnt >= key_max){
			break;
		}
	}
}


u32 kb_scan_row(int drv_ind, unsigned char * gpio){
	/*
	 * set as gpio mode if using spi flash pin
	 * */
	unsigned char sr = irq_disable();
#if	(KB_KEY_FLASH_PIN_MULTI_USE)
	MSPI_AS_GPIO;
#endif

#if(!KB_LINE_MODE)
	u32 drv_pin = drive_pins[drv_ind];
	gpio_write(drv_pin, KB_LINE_HIGH_VALID);
	gpio_set_output_en(drv_pin, 1);
#endif

	u32 matrix = 0;
	foreach_arr(j, scan_pins){
		if(scan_pin_need & BIT(j)){
			int key = !gpio_read_cache (scan_pins[j], gpio);
			if(KB_LINE_HIGH_VALID != key) {
				if (KB_HAS_FN_KEY && (kb_k_mp[j][drv_ind] == VK_FN)) {
					kb_is_fn_pressed = 1;
				}
				matrix |= (1 << j);
			}
		}
	}
	//sleep_us(KB_DRV_DELAY_TIME);
	gpio_read_all (gpio);
	/*
	 * set as spi mode  if using spi flash pin
	 * */
#if	(KB_KEY_FLASH_PIN_MULTI_USE)
	MSPI_AS_SPI;
#endif

#if(!KB_LINE_MODE)
	////////		float drive pin	////////////////////////////
	//sleep_us(KB_SCAN_DELAY_TIME);
	gpio_write(drv_pin, 0);
	gpio_set_output_en(drv_pin, 0);
#endif

	irq_restore(sr);
	return matrix;
}

u32 	matrix_buff[4][ARRAY_SIZE(drive_pins)];
int		matrix_wptr, matrix_rptr;


u32 kb_key_pressed(unsigned char * gpio)
{
	foreach_arr(i,drive_pins){
		gpio_write(drive_pins[i], KB_LINE_HIGH_VALID);
		gpio_set_output_en(drive_pins[i], 1);
	}
	sleep_us (20);
	gpio_read_all (gpio);

	u32 ret = 0;
	static unsigned char release_cnt = 0;
	static u32 ret_last = 0;

	foreach_arr(i,scan_pins){
		if(KB_LINE_HIGH_VALID != !gpio_read_cache (scan_pins[i], gpio)){
			ret |= (1 << i);
			release_cnt = 6;
			ret_last = ret;
		}
		//ret = ret && gpio_read(scan_pins[i]);
	}
	if(release_cnt){
		ret = ret_last;
		release_cnt--;
	}
	foreach_arr(i,drive_pins){
		gpio_write(drive_pins[i], 0);
		gpio_set_output_en(drive_pins[i], 0);
	}
	return ret;
}

u32 kb_scan_key_value (int numlock_status, int read_key,unsigned char * gpio)
{
		kb_event.cnt = 0;
		kb_event.ctrl_key = 0;
		kb_is_fn_pressed = 0;

		u32 pressed_matrix[ARRAY_SIZE(drive_pins)] = {0};
		kb_k_mp = kb_p_map[0];

		kb_scan_row (0, gpio);
		for (int i=0; i<=ARRAY_SIZE(drive_pins); i++) {
			u32 r = kb_scan_row (i < ARRAY_SIZE(drive_pins) ? i : 0, gpio);
			if (i) {
				pressed_matrix[i - 1] = r;
			}
		}

#if(KB_RM_GHOST_KEY_EN)
		kb_rmv_ghost_key(&pressed_matrix[0]);
#endif

		u32 key_changed = key_debounce_filter( pressed_matrix, \
						(numlock_status & KB_NUMLOCK_STATUS_POWERON) ? 0 : 1);

#if (KB_REPEAT_KEY_ENABLE)
		if(key_changed){
			repeat_key.key_change_flg = KEY_CHANGE;
			repeat_key.key_change_tick = clock_time();
		}
		else{
			if(repeat_key.key_change_flg == KEY_CHANGE){
				repeat_key.key_change_flg = KEY_SAME;
			}

			if( repeat_key.key_change_flg == KEY_SAME &&  repeat_key.key_repeat_flg && \
			    clock_time_exceed(repeat_key.key_change_tick,(KB_REPEAT_KEY_INTERVAL_MS-5)*1000)){
				repeat_key.key_change_tick = clock_time();
				key_changed = 1;
			}
		}
#endif

		///////////////////////////////////////////////////////////////////
		//	insert buffer here
		//       key mapping requires NUMLOCK status
		///////////////////////////////////////////////////////////////////
		u32 *pd;
		if (key_changed) {

			/////////// push to matrix buffer /////////////////////////
			pd = matrix_buff[matrix_wptr&3];
			for (int k=0; k<ARRAY_SIZE(drive_pins); k++) {
				*pd++ = pressed_matrix[k];
			}
			matrix_wptr = (matrix_wptr + 1) & 7;
			if ( ((matrix_wptr - matrix_rptr) & 7) > 4 ) {	//overwrite older data
				matrix_rptr = (matrix_wptr - 4) & 7;
			}
		}

		if (numlock_status & KB_NUMLOCK_STATUS_INVALID) {
			return 1;		//return empty key
		}

		////////// read out //////////
		if (matrix_wptr == matrix_rptr || !read_key) {
			return 0;			//buffer empty, no data
		}
		pd = matrix_buff[matrix_rptr&3];
		matrix_rptr = (matrix_rptr + 1) & 7;

		///////////////////////////////////////////////////////////////////
		kb_remap_key_code(pd, KB_RETURN_KEY_MAX, &kb_event, numlock_status);

#if (KB_REPEAT_KEY_ENABLE)
		if(repeat_key.key_change_flg == KEY_CHANGE){
			repeat_key.key_repeat_flg = 0;

			if(kb_event.cnt == 1){ //handle one key repeat only
				for(int i=0;i<KB_REPEAT_KEY_NUM;i++){
					if(kb_event.keycode[0] == kb_map_repeat[i]){
						repeat_key.key_repeat_flg = 1;
						break;
					}
				}
			}
		}
#endif

		return 1;
}





#endif


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
