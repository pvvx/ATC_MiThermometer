#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "ble.h"
#include "vendor/common/blt_common.h"
#include "cmd_parser.h"
#include "lcd.h"
#include "app.h"
#include "flash_eep.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#endif
#if USE_FLASH_MEMO
#include "logger.h"
#endif
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif

void bls_set_advertise_prepare(void *p); // add ll_adv.h

RAM uint8_t ble_connected; // bit 0 - connected, bit 1 - conn_param_update, bit 2 - paring success, bit 7 - reset of disconnect
uint8_t send_buf[SEND_BUFFER_SIZE];

RAM uint8_t blt_rxfifo_b[64 * 8] = { 0 };
RAM my_fifo_t blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b, };
RAM uint8_t blt_txfifo_b[40 * 16] = { 0 };
RAM my_fifo_t blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b, };
RAM uint8_t ble_name[12] = { 11, 0x09,
#if DEVICE_TYPE == DEVICE_MHO_C401
		'M', 'H', 'O', '_', '0', '0', '0', '0',	'0', '0' };
#elif DEVICE_TYPE == DEVICE_CGG1
		'C', 'G', 'G', '_', '0', '0', '0', '0',	'0', '0' };
#else
		'A', 'T', 'C', '_', '0', '0', '0', '0',	'0', '0' };
#endif
RAM uint8_t mac_public[6];
RAM uint8_t mac_random_static[6];
RAM uint32_t adv_send_count;
RAM uint32_t adv_old_count;
RAM adv_buf_t adv_buf;
uint8_t ota_is_working = 0;

void app_enter_ota_mode(void) {
#if USE_NEW_OTA
	bls_ota_clearNewFwDataArea();
#endif
	ota_is_working = 1;
	bls_ota_setTimeout(45 * 1000000); // set OTA timeout  45 seconds
}

void ble_disconnect_callback(uint8_t e, uint8_t *p, int n) {
	if(ble_connected & 0x80) // reset device on disconnect?
		start_reboot();

	bls_pm_setManualLatency(0); // ?

	ble_connected = 0;
	ota_is_working = 0;
	mi_key_stage = 0;
	//lcd_flg.b.notify_on = 0;
	lcd_flg.uc = 0;
#if USE_FLASH_MEMO
	rd_memo.cnt = 0;
#endif
	if(cfg.flg.tx_measures)
		tx_measures = 0xff;
	else
		tx_measures = 0;
}

void ble_connect_callback(uint8_t e, uint8_t *p, int n) {
	// bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1000);
	ble_connected = 1;
	if(cfg.connect_latency) {
		my_periConnParameters.intervalMin = 16;
		my_periConnParameters.intervalMax = 16;
	}
	my_periConnParameters.latency = cfg.connect_latency;
	my_periConnParameters.timeout = connection_timeout;
	bls_l2cap_requestConnParamUpdate(my_periConnParameters.intervalMin, my_periConnParameters.intervalMax, my_periConnParameters.latency, my_periConnParameters.timeout);
}

int app_conn_param_update_response(u8 id, u16  result) {
	if(result == CONN_PARAM_UPDATE_ACCEPT)
		ble_connected |= 2;
	else if(result == CONN_PARAM_UPDATE_REJECT) {
		// bls_l2cap_requestConnParamUpdate(160, 160, 4, 300); // (200 ms, 200 ms, 1 s, 3 s)
	}
	return 0;
}

extern u32 blt_ota_start_tick;
int otaWritePre(void * p) {
	blt_ota_start_tick = clock_time() | 1;
	return otaWrite(p);
}

_attribute_ram_code_
int app_advertise_prepare_handler(rf_packet_adv_t * p)	{
	adv_send_count++;
//	set_adv_data();
	return 1;		// = 1 ready to send ADV packet, = 0 not send ADV
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
	if (event == GAP_EVT_SMP_TK_DISPALY) { // PK_Resp_Dsply_Init_Input
			//u32 *pinCode = (u32*) para;
			uint32_t * p = (uint32_t *)&smp_param_own.paring_tk[0];
			memset(p, 0, sizeof(smp_param_own.paring_tk));
			p[0] = pincode;
#if 0
	} else if (event == GAP_EVT_SMP_PARING_SUCCESS) {
		gap_smp_paringSuccessEvt_t* p = (gap_smp_paringSuccessEvt_t*)para;
		if(p->bonding && p->bonding_result)  // paring success ?
			ble_connected |= 4;
	} else if (event == GAP_EVT_SMP_PARING_FAIL) {
		//gap_smp_paringFailEvt_t * p = (gap_smp_paringFailEvt_t *)para;
	} else if (event == GAP_EVT_SMP_TK_REQUEST_PASSKEY) {
		//blc_smp_setTK_by_PasskeyEntry(pincode);
	} else if (event == GAP_EVT_SMP_PARING_BEAGIN) {
		//gap_smp_paringBeginEvt_t * p = (gap_smp_paringBeginEvt_t*)para;
		// ...
	} else if(event == GAP_EVT_SMP_TK_REQUEST_OOB) {
		//blc_smp_setTK_by_OOB();
	} else if(event == GAP_EVT_SMP_TK_NUMERIC_COMPARE) {
		//uint32_t * pin = (uint32_t*)para;
		//blc_smp_setNumericComparisonResult(*pin == pincode);
	} else if(event == GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE) {
#endif
	}
	return 0;
}
#endif

extern attribute_t my_Attributes[ATT_END_H];
const char* hex_ascii = { "0123456789ABCDEF" };
void ble_get_name(void) {
	int16_t len = flash_read_cfg(&ble_name[2], EEP_ID_DVN, sizeof(ble_name)-2);
	if(len < 1) {
		//Set the BLE Name to the last three MACs the first ones are always the same
#if DEVICE_TYPE == DEVICE_MHO_C401
		ble_name[2] = 'M';
		ble_name[3] = 'H';
		ble_name[4] = 'O';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_CGG1
		ble_name[2] = 'C';
		ble_name[3] = 'G';
		ble_name[4] = 'G';
		ble_name[5] = '_';
#else
		ble_name[2] = 'A';
		ble_name[3] = 'T';
		ble_name[4] = 'C';
		ble_name[5] = '_';
#endif
		ble_name[6] = hex_ascii[mac_public[2] >> 4];
		ble_name[7] = hex_ascii[mac_public[2] & 0x0f];
		ble_name[8] = hex_ascii[mac_public[1] >> 4];
		ble_name[9] = hex_ascii[mac_public[1] & 0x0f];
		ble_name[10] = hex_ascii[mac_public[0] >> 4];
		ble_name[11] = hex_ascii[mac_public[0] & 0x0f];
		my_Attributes[GenericAccess_DeviceName_DP_H].attrLen = 10;
		ble_name[0] = 11;
	} else {
		my_Attributes[GenericAccess_DeviceName_DP_H].attrLen = len;
		ble_name[0] = (uint8_t)(len + 1);
	}
}

__attribute__((optimize("-Os"))) void init_ble(void) {
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
	my_att_init(); //gatt initialization
	blc_l2cap_register_handler(blc_l2cap_packet_receive);

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//   is about to exceed the sector threshold, this sector must be erased, and all useful information
	//   should re_stored) , so it must be done after battery check
#if BLE_SECURITY_ENABLE
	if(pincode) {
		//bls_smp_configParingSecurityInfoStorageAddr(0x074000);
		//bls_smp_eraseAllParingInformation();
		//blc_smp_param_setBondingDeviceMaxNumber(SMP_BONDING_DEVICE_MAX_NUM); //if not set, default is : SMP_BONDING_DEVICE_MAX_NUM
#if 0
		//set security level: "LE_Security_Mode_1_Level_2"
		blc_smp_setSecurityLevel(Unauthenticated_Paring_with_Encryption);  //if not set, default is : LE_Security_Mode_1_Level_2(Unauthenticated_Paring_with_Encryption)
		blc_smp_setParingMethods(LE_Secure_Connection);
		blc_smp_setSecurityParamters(Bondable_Mode, 1, 0, 0, IO_CAPABLITY_NO_IN_NO_OUT);
		//blc_smp_setEcdhDebugMode(debug_mode); //use debug mode for sniffer decryption
#elif 1
		//set security level: "LE_Security_Mode_1_Level_3"
		blc_smp_setSecurityLevel(Authenticated_Paring_with_Encryption); //if not set, default is : LE_Security_Mode_1_Level_2(Unauthenticated_Paring_with_Encryption)
		blc_smp_setParingMethods(LE_Secure_Connection);
		blc_smp_enableAuthMITM(1);
		//blc_smp_setBondingMode(Bondable_Mode);	// if not set, default is : Bondable_Mode
		blc_smp_setIoCapability(IO_CAPABILITY_DISPLAY_ONLY);	// if not set, default is : IO_CAPABILITY_NO_INPUT_NO_OUTPUT
#else
		//set security level: "LE_Security_Mode_1_Level_4"
		blc_smp_setSecurityLevel(Authenticated_LE_Secure_Connection_Paring_with_Encryption);  //if not set, default is : LE_Security_Mode_1_Level_2(Unauthenticated_Paring_with_Encryption)
		blc_smp_setParingMethods(LE_Secure_Connection);
		blc_smp_setSecurityParamters(Bondable_Mode, 1, 0, 0, IO_CAPABILITY_DISPLAY_ONLY);

#endif
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
		blc_gap_setEventMask(GAP_EVT_MASK_SMP_TK_DISPALY
#if 0
				| GAP_EVT_MASK_SMP_PARING_BEAGIN
				| GAP_EVT_MASK_SMP_TK_NUMERIC_COMPARE
				| GAP_EVT_MASK_SMP_PARING_SUCCESS
				| GAP_EVT_MASK_SMP_PARING_FAIL
				| GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY
				| GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE
				| GAP_EVT_MASK_SMP_TK_REQUEST_OOB
#endif
				);
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
	blc_pm_setDeepsleepRetentionThreshold(50, 30);
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(240);
	blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);
#if USE_NEW_OTA == 0
	bls_ota_clearNewFwDataArea();
#endif
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
	blc_l2cap_registerConnUpdateRspCb(app_conn_param_update_response);
	bls_set_advertise_prepare(app_advertise_prepare_handler);
#if USE_MIHOME_BEACON
	mi_beacon_init();
#endif

#if 0 // BLE_SECURITY_ENABLE && DEVICE_TYPE != DEVICE_MHO_C401
	if(pincode && *((u32 *)(CFG_ADR_BIND)) != 0xffffffff) {
		smp_param_save_t  bondInfo;
		u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
		if(bond_number) {  // at least 1 bonding device exist
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
			bls_ll_setAdvEnable(1);
		}
	} else
#endif
	ev_adv_timeout(0,0,0);
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
void set_adv_data() {
	uint8_t adv_type = cfg.flg.advertising_type; // 0 - atc1441, 1 - pvvx, 2 - Mi, 3 - all
	adv_old_count = adv_send_count;
	if(adv_type == ADV_TYPE_ALL)
		adv_type = adv_old_count & 3;
	/* adv_type: 0 - atc1441, 1 - Custom,  2,3 - Mi  */
	if(adv_type == ADV_TYPE_PVVX) {
		if(cfg.flg2.mi_beacon)
			pvvx_encrypt_beacon(measured_data.count);
		else {
			padv_custom_t p = (padv_custom_t)&adv_buf.data;
			memcpy(p->MAC, mac_public, 6);
#if USE_TRIGGER_OUT
			p->size = sizeof(adv_custom_t) - 1;
#else
			p->size = sizeof(adv_custom_t) - 2;
#endif
			p->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
			p->UUID = ADV_CUSTOM_UUID16; // GATT Service 0x181A Environmental Sensing (little-endian)
			p->temperature = measured_data.temp; // x0.01 C
			p->humidity = measured_data.humi; // x0.01 %
			p->battery_mv = measured_data.battery_mv; // x mV
			p->battery_level = battery_level; // x1 %
			p->counter = (uint8_t)measured_data.count;
#if USE_TRIGGER_OUT
			p->flags = trg.flg_byte;
#endif
		}
	} else if(adv_type & ADV_TYPE_MASK_REF) { // adv_type == 2 or 3
#if USE_MIHOME_BEACON
		if(cfg.flg2.mi_beacon) {
			if(cfg.flg.advertising_type == ADV_TYPE_ALL)
				mi_encrypt_beacon(measured_data.count);
			else
				mi_encrypt_beacon(measured_data.count >> 2);
		}
		else
#endif
		{
			padv_mi_t p = (padv_mi_t)&adv_buf.data;
			memcpy(p->MAC, mac_public, 6);
			p->size = sizeof(adv_mi_t) - 1;
			p->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
			p->UUID = ADV_XIAOMI_UUID16; // 16-bit UUID for Members 0xFE95 Xiaomi Inc.
#if 0
			p->ctrl.word = 0;
			p->ctrl.bit.version = 3; // XIAOMI_DEV_VERSION
			p->ctrl.bit.MACInclude = 1;
			p->ctrl.bit.ObjectInclude = 1;
#else
			p->ctrl.word = 0x3050; // 0x3050 version = 3, MACInclude, ObjectInclude
#endif
			p->dev_id = DEVICE_TYPE;
			p->nx10 = (XIAOMI_DATA_ID_TempAndHumidity >> 8) & 0xff; // (hi byte XIAOMI_DATA_ID)
			p->counter = (uint8_t)measured_data.count;
			if (adv_old_count & 1) {
				p->data_id = XIAOMI_DATA_ID_TempAndHumidity & 0xff; // (lo byte XIAOMI_DATA_ID)
				p->t0d.len = 0x04;
				p->t0d.temperature = last_temp; // x0.1 C
				p->t0d.humidity = measured_data.humi / 10; // x0.1 %
			} else {
				p->data_id = XIAOMI_DATA_ID_Power & 0xff; // (lo byte XIAOMI_DATA_ID)
				p->t0a.len1 = 1;
				p->t0a.battery_level = battery_level; // Battery percentage, Range: 0-100
			}
#if USE_MIHOME_BEACON
		}
#endif
	} else { // adv_type == 0 == ADV_TYPE_ATC
		if(cfg.flg2.mi_beacon)
			atc_encrypt_beacon(measured_data.count);
		else {
			padv_atc1441_t p = (padv_atc1441_t)&adv_buf.data;
			p->size = sizeof(adv_atc1441_t) - 1;
			p->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
			p->UUID = ADV_CUSTOM_UUID16; // GATT Service 0x181A Environmental Sensing (little-endian)
			p->MAC[0] = mac_public[5];
			p->MAC[1] = mac_public[4];
			p->MAC[2] = mac_public[3];
			p->MAC[3] = mac_public[2];
			p->MAC[4] = mac_public[1];
			p->MAC[5] = mac_public[0];
			p->temperature[0] = (uint8_t)(last_temp >> 8);
			p->temperature[1] = (uint8_t)last_temp; // x0.1 C
			p->humidity = (uint8_t)last_humi; // x1 %
			p->battery_level = battery_level; // x1 %
			p->battery_mv[0] = (uint8_t)(measured_data.battery_mv >> 8);
			p->battery_mv[1] = (uint8_t)measured_data.battery_mv; // x1 mV
			p->counter = (uint8_t)measured_data.count;
		}
	}
	if(cfg.flg2.adv_flags) {
		adv_buf.flag[0] = 0x02; // size
		adv_buf.flag[1] = GAP_ADTYPE_FLAGS; // type
		/*	Flags:
		 	bit0: LE Limited Discoverable Mode
			bit1: LE General Discoverable Mode
			bit2: BR/EDR Not Supported
			bit3: Simultaneous LE and BR/EDR to Same Device Capable (Controller)
			bit4: Simultaneous LE and BR/EDR to Same Device Capable (Host)
			bit5..7: Reserved
		 */
		adv_buf.flag[2] = 0x06; // Flags
		bls_ll_setAdvData((u8 *)&adv_buf, adv_buf.data[0] + 4);
	} else
		bls_ll_setAdvData((u8 *)&adv_buf.data, adv_buf.data[0] + 1);
}

_attribute_ram_code_ void ble_send_measures(void) {
	send_buf[0] = CMD_ID_MEASURE;
	memcpy(&send_buf[1], &measured_data, sizeof(measured_data));
#if	USE_TRIGGER_OUT
	send_buf[sizeof(measured_data)+1] = trg.flg_byte;
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(measured_data) + 2);
#else
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(measured_data) + 1);
#endif
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

#if USE_FLASH_MEMO
__attribute__((optimize("-Os"))) void send_memo_blk(void) {
	send_buf[0] = CMD_ID_LOGGER;
	if(++rd_memo.cur > rd_memo.cnt || (!get_memo(rd_memo.cur, (pmemo_blk_t)&send_buf[3]))) {
		send_buf[1] = 0;
		send_buf[2] = 0;
		bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, 3);
		bls_pm_setManualLatency(cfg.connect_latency);
		rd_memo.cnt = 0;
	} else {
		send_buf[1] = rd_memo.cur;
		send_buf[2] = rd_memo.cur >> 8;
		bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, 3 + sizeof(memo_blk_t));
	}
}
#endif


