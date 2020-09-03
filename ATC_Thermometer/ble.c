#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

RAM	u8	ble_connected = 0;

extern u8 my_tempVal[2];
extern u8 my_humiVal[2];
extern u8 my_batVal[1];

RAM u8 		 	blt_rxfifo_b[64 * 8] = {0};
RAM	my_fifo_t	blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b,};

RAM u8 			blt_txfifo_b[40 * 16] = {0};
RAM	my_fifo_t	blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b,};

RAM u8	tbl_scanRsp [] = {11, 0x09, 'A', 'T', 'C', '_', '0', '0', '0', '0', '0', '0'};

RAM u8	advertising_data[] = {
 0x10, 0x16, 0x1a, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xbb, 0xcc, 0xdd, 0xdd, 0x00
};

u8  mac_public[6];
	
u8 	ota_is_working = 0;

void app_enter_ota_mode(void)
{
	ota_is_working = 1;
	bls_ota_setTimeout(5 * 1000000);
	show_smiley(1);
}

void app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{
	bls_ll_setAdvParam( ADVERTISING_INTERVAL, ADVERTISING_INTERVAL+50, ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0,  NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	bls_ll_setAdvEnable(1);
}

void ble_remote_terminate(u8 e,u8 *p, int n)
{
	ble_connected = 0;
	show_ble_symbol(0);
}

_attribute_ram_code_ void user_set_rf_power (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (RF_POWER_P3p01dBm);
}

void task_connect(u8 e, u8 *p, int n)
{
	ble_connected = 1;
//	bls_l2cap_requestConnParamUpdate (8, 8, 2, 30);  // 200mS
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  //1S
	show_ble_symbol(1);
}

extern u32 blt_ota_start_tick;
int otaWritePre(void * p)
{
	blt_ota_start_tick = clock_time()|1;
	otaWrite(p);
	return 0;
}

extern bool temp_C_or_F;
int RxTxWrite(void * p)
{
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	uint8_t inData = req->dat[0];
	if(inData == 0xFF){
		temp_C_or_F = 1;
	}else if(inData == 0xCC){
		temp_C_or_F = 0;
	}	
	return 0;
}

void task_conn_update_req(u8 e, u8 *p, int n)
{
}

void task_conn_update_done(u8 e, u8 *p, int n)
{
}

_attribute_ram_code_ void blt_pm_proc(void)
{
	if(ota_is_working){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		bls_pm_setManualLatency(0);
	}else{
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	}
}

void init_ble(){
////////////////// BLE stack initialization ////////////////////////////////////
	u8  mac_random_static[6];
	blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
	
	//Set the BLE Name to the last three MACs the first ones are always the same
    const char* hex_ascii = {"0123456789ABCDEF"};	
	tbl_scanRsp[6] = hex_ascii[mac_public[2]>>4];
	tbl_scanRsp[7] = hex_ascii[mac_public[2] &0x0f];	
	tbl_scanRsp[8] = hex_ascii[mac_public[1]>>4];
	tbl_scanRsp[9] = hex_ascii[mac_public[1] &0x0f];		
	tbl_scanRsp[10] = hex_ascii[mac_public[0]>>4];
	tbl_scanRsp[11] = hex_ascii[mac_public[0] &0x0f];
	
	advertising_data[4] = mac_public[5];
	advertising_data[5] = mac_public[4];
	advertising_data[6] = mac_public[3];
	advertising_data[7] = mac_public[2];
	advertising_data[8] = mac_public[1];
	advertising_data[9] = mac_public[0];
	
////// Controller Initialization  //////////
	blc_ll_initBasicMCU();                      //must
	blc_ll_initStandby_module(mac_public);		//must
	blc_ll_initAdvertising_module(mac_public); 	//adv module: 		 must for BLE slave,
	blc_ll_initConnection_module();				//connection module  must for BLE slave/master
	blc_ll_initSlaveRole_module();				//slave module: 	 must for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional
	
////// Host Initialization  //////////
	blc_gap_peripheral_init();
	extern void my_att_init();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_smp_setSecurityLevel(No_Security);
	
///////////////////// USER application initialization ///////////////////
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));
	bls_ll_setAdvParam(  ADVERTISING_INTERVAL, ADVERTISING_INTERVAL+50, ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0,  NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	bls_ll_setAdvEnable(1);
	user_set_rf_power(0, 0, 0);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);
	bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_REQ, &task_conn_update_req);
	bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_UPDATE, &task_conn_update_done);
	
///////////////////// Power Management initialization///////////////////
	blc_ll_initPowerManagement_module();
	bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	blc_pm_setDeepsleepRetentionThreshold(95, 95);
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(240);
	blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);

	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(app_enter_ota_mode);	
}

bool ble_get_connected(){
	return ble_connected;
}

void set_adv_data(uint16_t temp, uint16_t humi, uint8_t battery_level, uint16_t battery_mv){
	advertising_data[10] = temp>>8;
	advertising_data[11] = temp&0xff;
	advertising_data[12] = humi&0xff;
	advertising_data[13] = battery_level;
	advertising_data[14] = battery_mv>>8;
	advertising_data[15] = battery_mv&0xff;
	advertising_data[16]++;
	bls_ll_setAdvData( (u8 *)advertising_data, sizeof(advertising_data) );	
}

void ble_send_temp(u16 temp){
	my_tempVal[0] = temp & 0xFF;
	my_tempVal[1] = temp >> 8;
	bls_att_pushNotifyData(TEMP_LEVEL_INPUT_DP_H, my_tempVal, 2);
}

void ble_send_humi(u16 humi){
	humi*=100;
	my_humiVal[0] = humi & 0xFF;
	my_humiVal[1] = humi >> 8;
	bls_att_pushNotifyData(HUMI_LEVEL_INPUT_DP_H, (u8 *)my_humiVal, 2);
}

void ble_send_battery(u8 value){
	my_batVal[0] = value;
	bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (u8 *)my_batVal, 1);
}