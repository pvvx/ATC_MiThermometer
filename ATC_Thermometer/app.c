#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

RAM uint32_t last_delay = 0xFFFF0000, last_adv_delay = 0xFFFF0000, last_battery_delay = 0xFFFF0000;
RAM bool last_smiley;
uint16_t temp = 0;
uint16_t humi = 0;
RAM uint8_t battery_level;
RAM uint16_t battery_mv;
RAM bool show_batt_or_humi;

//Settings
RAM bool temp_C_or_F;
RAM bool blinking_smiley = true;
RAM uint8_t advertising_interval = 6;
RAM int8_t temp_offset;
RAM int8_t humi_offset;

void user_init_normal(void){//this will get executed one time after power up
	random_generator_init();  //must
	init_ble();	
	init_sensor();
	init_lcd();	
	show_atc_mac();
	battery_mv = get_battery_mv();
	battery_level = get_battery_level(get_battery_mv());
}

_attribute_ram_code_ void user_init_deepRetn(void){//after sleep this will get executed
	blc_ll_initBasicMCU();
	rf_set_power_level_index (RF_POWER_P3p01dBm);
	blc_ll_recoverDeepRetention();
}

void main_loop(){	
	if((clock_time()-last_delay) > 5000*CLOCK_SYS_CLOCK_1MS){//main loop delay
	
		if((clock_time()-last_battery_delay) > 5*60000*CLOCK_SYS_CLOCK_1MS){//Read battery delay
			battery_mv = get_battery_mv();
			battery_level = get_battery_level(get_battery_mv());
			last_battery_delay = clock_time();
		}
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
		
		if(show_batt_or_humi){//Change between Humidity displaying and battery level
			show_battery_symbol(0);
			show_small_number(humi,1);	
		}else{
			show_battery_symbol(1);
			show_small_number((battery_level==100)?99:battery_level,1);
		}
		show_batt_or_humi = !show_batt_or_humi;
		
		update_lcd();	
		if(ble_get_connected()){//If connected notify Sensor data
			ble_send_temp(temp);
			ble_send_humi(humi);
			ble_send_battery(battery_level);
		}
		
		if((clock_time()-last_adv_delay) > advertising_interval*10000*CLOCK_SYS_CLOCK_1MS){//Advetise data delay
			set_adv_data(temp, humi, battery_level, battery_mv);
			last_adv_delay = clock_time();
		}
		
		if(blinking_smiley){//If Smiley should blink do it
		last_smiley=!last_smiley;
		show_smiley(last_smiley);
		}
		
		update_lcd();
		last_delay = clock_time();
	}
	blt_sdk_main_loop();
	blt_pm_proc();	
}
