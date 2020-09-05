#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

extern bool advertising_type;
extern bool temp_C_or_F;
extern bool blinking_smiley;
extern bool show_batt_enabled;
extern uint8_t advertising_interval;
extern int8_t temp_offset;
extern int8_t humi_offset;
void cmd_parser(void * p){
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	uint8_t inData = req->dat[0];
	if(inData == 0xFF){
		temp_C_or_F = true;//Temp in F
	}else if(inData == 0xCC){
		temp_C_or_F = false;//Temp in C
	}else if(inData == 0xB1){
		show_batt_enabled = true;//Enable battery on LCD
	}else if(inData == 0xB0){
		show_batt_enabled = false;//Disable battery on LCD
	}else if(inData == 0xA0){
		blinking_smiley = false;
		show_smiley(0);//Smiley off
	}else if(inData == 0xA1){
		blinking_smiley = false;
		show_smiley(1);//Smiley happy
	}else if(inData == 0xA2){
		blinking_smiley = false;
		show_smiley(2);//Smiley sad
	}else if(inData == 0xAB){
		blinking_smiley = true;//Smiley blinking
	}else if(inData == 0xAE){
		advertising_type = false;//Advertising type Custom
	}else if(inData == 0xAF){
		advertising_type = true;//Advertising type Mi Like
	}else if(inData == 0xFE){
		advertising_interval = req->dat[1];//Set advertising interval with second byte, value*10second / 0=main_delay
	}else if(inData == 0xFA){
		temp_offset = req->dat[1];//Set temp offset, -12,5 - +12,5 °C
	}else if(inData == 0xFB){
		humi_offset = req->dat[1];//Set humi offset, -50 - +50 %
		if(humi_offset<-50)humi_offset=-50;
		if(humi_offset>50)humi_offset=50;
	}
}