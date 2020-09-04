#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"


void user_init_normal(void)//this will get executed one time after power up
{
	random_generator_init();  //must
	init_ble();	
	init_sensor();
	init_lcd();	
	show_atc_mac();
}

_attribute_ram_code_ void user_init_deepRetn(void)//after sleep this will get executed
{
	blc_ll_initBasicMCU();
	rf_set_power_level_index (RF_POWER_P3p01dBm);
	blc_ll_recoverDeepRetention();
}

RAM uint32_t last_delay = 0xFFFF0000, last_delay_min = 0xFFFF0000;
RAM bool last_smiley;
uint16_t temp = 0;
uint16_t humi = 0;
uint8_t battery_level;

//Settings
RAM bool temp_C_or_F;
RAM bool blinking_smiley = true;
RAM uint8_t advertising_interval = 6;
RAM int8_t temp_offset;
RAM int8_t humi_offset;

void main_loop (void)
{	
	if((clock_time()-last_delay) > 5000*CLOCK_SYS_CLOCK_1MS){		
		battery_level = get_battery_level();
		read_sensor(&temp,&humi,true);		
		
		temp += temp_offset;
		humi += humi_offset;
		
		if(temp_C_or_F){
			temp = ((((temp*10)/5)*9)+3200)/10;//convert C to F
			show_temp_symbol(2);
		}else{
			show_temp_symbol(1);
		}
		
		show_big_number(temp,1);
		show_small_number(humi,1);
		update_lcd();	
		if(ble_get_connected()){
			ble_send_temp(temp);
			ble_send_humi(humi);
			ble_send_battery(battery_level);
		}
		
		if((clock_time()-last_delay_min) > advertising_interval*10000*CLOCK_SYS_CLOCK_1MS){
			set_adv_data(temp, humi, battery_level, get_battery_mv());
			last_delay_min = clock_time();
		}
		show_battery_symbol((battery_level<35)?1:0);		
		
		if(blinking_smiley){
		last_smiley=!last_smiley;
		show_smiley(last_smiley);
		}
		
		update_lcd();
		last_delay = clock_time();
	}
	blt_sdk_main_loop();
	blt_pm_proc();	
}
