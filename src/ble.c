#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "ble.h"
#include "cmd_parser.h"
#include "lcd.h"
#include "app.h"
#include "flash_eep.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#endif

RAM uint8_t ble_connected; // bit 0 - connected, bit 1 - conn_param_update, bit 2 - reset of disconnect
uint8_t send_buf[20];

extern uint8_t my_tempVal[2];
extern uint8_t my_humiVal[2];
extern uint8_t my_batVal[1];

RAM uint8_t blt_rxfifo_b[64 * 8] = { 0 };
RAM my_fifo_t blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b, };
RAM uint8_t blt_txfifo_b[40 * 16] = { 0 };
RAM my_fifo_t blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b, };
RAM uint8_t ble_name[12] = { 11, 0x09,
		'A', 'T', 'C', '_', '0', '0', '0', '0',	'0', '0' };
RAM uint8_t mac_public[6];
RAM uint8_t mac_random_static[6];
RAM uint8_t adv_mi_count;
RAM uint8_t adv_buffer[24];
uint8_t ota_is_working = 0;

void app_enter_ota_mode(void) {
	ota_is_working = 1;
	bls_ota_setTimeout(45 * 1000000); // set OTA timeout  45 seconds
}

void ble_disconnect_callback(uint8_t e, uint8_t *p, int n) {
	if(ble_connected & 0x80) // reset device on disconnect?
		start_reboot();
	ble_connected = 0;
	ota_is_working = 0;
	mi_key_stage = 0;
	//lcd_flg.b.notify_on = 0;
	lcd_flg.uc = 0;
	if(cfg.flg.tx_measures)
		tx_measures = 0xff;
	else
		tx_measures = 0;
}

void ble_connect_callback(uint8_t e, uint8_t *p, int n) {
	ble_connected = 1;
	if(cfg.connect_latency)
		bls_l2cap_requestConnParamUpdate(16, 16, cfg.connect_latency, connection_timeout); // (16*1.25 ms, 16*1.25 ms, (16*1.25)*100 ms, 800*10 ms)
	else
		bls_l2cap_requestConnParamUpdate(my_periConnParameters.intervalMin, my_periConnParameters.intervalMax, cfg.connect_latency, connection_timeout); // (16*1.25 ms, 16*1.25 ms, (16*1.25)*100 ms, 800*10 ms)
	// bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1000);
}

int app_conn_param_update_response(u8 id, u16  result) {
/*
	if(result == CONN_PARAM_UPDATE_ACCEPT) {
	} else if(result == CONN_PARAM_UPDATE_REJECT) {	}
*/
	ble_connected |= 2;
	return 0;
}

extern u32 blt_ota_start_tick;
int otaWritePre(void * p) {
	blt_ota_start_tick = clock_time() | 1;
	otaWrite(p);
	return 0;
}

_attribute_ram_code_ int RxTxWrite(void * p) {
	cmd_parser(p);
	return 0;
}

_attribute_ram_code_ void user_set_rf_power(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	rf_set_power_level_index(cfg.rf_tx_power);
}
/*
 * bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);
 * blt_event_callback_t(): */
_attribute_ram_code_ void ev_adv_timeout(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	bls_ll_setAdvParam(adv_interval, adv_interval + 50,
			ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
			BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	bls_ll_setAdvEnable(1);
}

#if BLE_SECURITY_ENABLE
int app_host_event_callback(u32 h, u8 *para, int n) {
	(void) para; (void) n;
	uint8_t event = (uint8_t)h;
	if (event == GAP_EVT_SMP_TK_DISPALY) {
			//u32 pinCode = *(u32*) para;
			uint32_t * p = (uint32_t *)&smp_param_own.paring_tk[0];
			memset(p, 0, sizeof(smp_param_own.paring_tk));
			p[0] = pincode;
	}
//	else if (event == GAP_EVT_SMP_TK_REQUEST_PASSKEY)
//		blc_smp_setTK_by_PasskeyEntry(pincode);
	return 0;
}
#endif

const char* hex_ascii = { "0123456789ABCDEF" };
void ble_get_name(void) {
	int16_t len = flash_read_cfg(&ble_name[2], EEP_ID_DVN, sizeof(ble_name)-2);
	if(len < 1) {
		//Set the BLE Name to the last three MACs the first ones are always the same
		ble_name[2] = 'A';
		ble_name[3] = 'T';
		ble_name[4] = 'C';
		ble_name[5] = '_';
		ble_name[6] = hex_ascii[mac_public[2] >> 4];
		ble_name[7] = hex_ascii[mac_public[2] & 0x0f];
		ble_name[8] = hex_ascii[mac_public[1] >> 4];
		ble_name[9] = hex_ascii[mac_public[1] & 0x0f];
		ble_name[10] = hex_ascii[mac_public[0] >> 4];
		ble_name[11] = hex_ascii[mac_public[0] & 0x0f];
		ble_name[0] = 11;
	} else
		ble_name[0] = (uint8_t)(len + 1);
}

void init_ble(void) {
	////////////////// BLE stack initialization //////////////////////
	blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
	/// if bls_ll_setAdvParam( OWN_ADDRESS_RANDOM ) ->  blc_ll_setRandomAddr(mac_random_static);
	ble_get_name();
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(); //must
	blc_ll_initStandby_module(mac_public); //must
	blc_ll_initAdvertising_module(mac_public); // adv module: 		 must for BLE slave,
	blc_ll_initConnection_module(); // connection module  must for BLE slave/master
	blc_ll_initSlaveRole_module(); // slave module: 	 must for BLE slave,
	blc_ll_initPowerManagement_module(); //pm module:      	 optional

	////// Host Initialization  //////////
	blc_gap_peripheral_init();
	extern void my_att_init();
	my_att_init(); //gatt initialization
	blc_l2cap_register_handler(blc_l2cap_packet_receive);

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//   is about to exceed the sector threshold, this sector must be erased, and all useful information
	//   should re_stored) , so it must be done after battery check
#if BLE_SECURITY_ENABLE
	if(pincode) {
		// bls_smp_eraseAllParingInformation();
		// bls_smp_configParingSecurityInfoStorageAddr(0x074000);
		// blc_smp_setParingMethods(LE_Secure_Connection);
		// blc_smp_param_setBondingDeviceMaxNumber(SMP_BONDING_DEVICE_MAX_NUM);
		//set security level: "LE_Security_Mode_1_Level_3"
		blc_smp_setSecurityLevel(Authenticated_Paring_with_Encryption);  //if not set, default is : LE_Security_Mode_1_Level_2(Unauthenticated_Paring_with_Encryption)
		blc_smp_enableAuthMITM(1);
		blc_smp_setBondingMode(Bondable_Mode);	// if not set, default is : Bondable_Mode
		blc_smp_setIoCapability(IO_CAPABILITY_DISPLAY_ONLY);	// if not set, default is : IO_CAPABILITY_NO_INPUT_NO_OUTPUT
		//Smp Initialization may involve flash write/erase(when one sector stores too much information,
		//   is about to exceed the sector threshold, this sector must be erased, and all useful information
		//   should re_stored) , so it must be done after battery check
		//Notice:if user set smp parameters: it should be called after usr smp settings
		blc_smp_peripheral_init();
		// Hid device on android7.0/7.1 or later version
		// New paring: send security_request immediately after connection complete
		// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
		//host(GAP/SMP/GATT/ATT) event process: register host event callback and set event mask
		blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )
		blc_gap_registerHostEventHandler(app_host_event_callback);
		blc_gap_setEventMask(GAP_EVT_MASK_SMP_TK_DISPALY); // | GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE | GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY);
	} else
#endif
	blc_smp_setSecurityLevel(No_Security);

	///////////////////// USER application initialization ///////////////////
	bls_ll_setScanRspData((uint8_t *) ble_name, ble_name[0]+1);
	rf_set_power_level_index(cfg.rf_tx_power);
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);
	bls_app_registerEventCallback(BLT_EV_FLAG_CONNECT, &ble_connect_callback);
	bls_app_registerEventCallback(BLT_EV_FLAG_TERMINATE,
			&ble_disconnect_callback);

	///////////////////// Power Management initialization///////////////////
	blc_ll_initPowerManagement_module();
	bls_pm_setSuspendMask(SUSPEND_DISABLE);
	blc_pm_setDeepsleepRetentionThreshold(95, 95);
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(240);
	blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);

	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
	blc_l2cap_registerConnUpdateRspCb(app_conn_param_update_response);
#if BLE_SECURITY_ENABLE
#if 1
#else
	if(pincode) {
		u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
		if(bond_number) {  // at least 1 bonding device exist
			smp_param_save_t  bondInfo;
			bls_smp_param_loadByIndex(bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
			//set direct adv
			bls_ll_setAdvParam( adv_interval, adv_interval + 50,
						ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY,
						OWN_ADDRESS_PUBLIC,
						bondInfo.peer_addr_type, bondInfo.peer_addr,
						BLT_ENABLE_ADV_ALL,
						ADV_FP_NONE);
			//it is recommended that direct adv only last for several seconds, then switch to indirect adv
			bls_ll_setAdvDuration(adv_interval*625*2, 1); // interval usec, duration enable
			bls_app_registerEventCallback(BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);
		}
	} else
#endif
#endif
	ev_adv_timeout(0,0,0);
}

/* adv_type: 0 - atc1441, 1 - Custom,  2,3 - Mi  */
_attribute_ram_code_ void set_adv_data(uint8_t adv_type) {
	if(adv_type == 3)
		adv_type = adv_mi_count & 3;
	if(adv_type == 1) {
#if USE_TRIGGER_OUT
		test_trg_input();
#endif
		padv_custom_t p = (padv_custom_t)adv_buffer;
		memcpy(p->MAC, mac_public, 6);
#if USE_TRIGGER_OUT
		p->size = sizeof(adv_custom_t) - 1;
#else
		p->size = sizeof(adv_custom_t) - 2;
#endif
		p->uid = 0x16; // 16-bit UUID
		p->UUID = 0x181A; // GATT Service 0x181A Environmental Sensing (little-endian)
		p->temperature = measured_data.temp; // x0.01 C
		p->humidity = measured_data.humi; // x0.01 %
		p->battery_mv = measured_data.battery_mv;
		p->battery_level = battery_level;
		p->counter = (uint8_t)measured_data.count;
#if USE_TRIGGER_OUT
		p->flags = *(uint8_t *)(&trg.flg);
#endif
	} else if(adv_type & 2) { // adv_type == 2 or 3
		padv_mi_t p = (padv_mi_t)adv_buffer;
		memcpy(p->MAC, mac_public, 6);
		p->size = sizeof(adv_mi_t) - 1;
		p->uid = 0x16; // 16-bit UUID
		p->UUID = 0xFE95; // 16-bit UUID for Members 0xFE95 Xiaomi Inc.
		p->ctrl = 0x3050;
		p->dev_id = 0x055b;
		p->nx10 = 0x10;
		p->counter = (uint8_t)measured_data.count;
		if (adv_mi_count & 1) {
			p->data_id = 0x0d;
			p->t0d.len = 0x04;
			p->t0d.temperature = last_temp; // x0.1 C
			p->t0d.humidity = measured_data.humi / 10; // x0.1 %
		} else {
			p->data_id = 0x0a;
			p->t0a.len1 = 0x01;
			p->t0a.battery_level = battery_level;
			p->t0a.len2 = 0x02;
			p->t0a.battery_mv = measured_data.battery_mv;
		}
	} else { // adv_type == 0
		padv_atc1441_t p = (padv_atc1441_t)adv_buffer;
		p->size = sizeof(adv_atc1441_t) - 1;
		p->uid = 0x16; // 16-bit UUID
		p->UUID = 0x181A; // GATT Service 0x181A Environmental Sensing (little-endian)
		p->MAC[0] = mac_public[5];
		p->MAC[1] = mac_public[4];
		p->MAC[2] = mac_public[3];
		p->MAC[3] = mac_public[2];
		p->MAC[4] = mac_public[1];
		p->MAC[5] = mac_public[0];
		p->temperature[0] = (uint8_t)(last_temp >> 8);
		p->temperature[1] = (uint8_t)last_temp; // x0.1 C
		p->humidity = (uint8_t)last_humi; // x1 %
		p->battery_level = battery_level;
		p->battery_mv[0] = (uint8_t)(measured_data.battery_mv >> 8);
		p->battery_mv[1] = (uint8_t)measured_data.battery_mv;
		p->counter = (uint8_t)measured_data.count;
	}
	bls_ll_setAdvData(adv_buffer, adv_buffer[0]+1);
}

_attribute_ram_code_ void ble_send_measures(void) {
	send_buf[0] = CMD_ID_MEASURE;
	memcpy(&send_buf[1], &measured_data, sizeof(measured_data));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(measured_data) + 1);
}

void ble_send_ext(void) {
	send_buf[0] = CMD_ID_EXTDATA;
	memcpy(&send_buf[1], &ext, sizeof(ext));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(ext) + 1);
}

_attribute_ram_code_ void ble_send_lcd(void) {
	send_buf[0] = CMD_ID_LCD_DUMP;
	memcpy(&send_buf[1], display_buff, sizeof(display_buff));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(display_buff) + 1);
}

void ble_send_cmf(void) {
	send_buf[0] = CMD_ID_COMFORT;
	memcpy(&send_buf[1], &cmf, sizeof(cmf));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(cmf) + 1);
}

#if USE_TRIGGER_OUT
void ble_send_trg(void) {
	send_buf[0] = CMD_ID_TRG;
	memcpy(&send_buf[1], &trg, sizeof(trg));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(trg) + 1);
}
void ble_send_trg_flg(void) {
	send_buf[0] = CMD_ID_TRG_OUT;
	send_buf[1] = *((uint8_t *)(&trg.flg));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, 2);
}
#endif



