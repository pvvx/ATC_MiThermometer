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
#include "battery.h"
#if (DEV_SERVICES & SERVICE_TH_TRG)
#include "trigger.h"
#include "rds_count.h"
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
#include "logger.h"
#endif
#include "custom_beacon.h"
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#if USE_BTHOME_BEACON
#include "bthome_beacon.h"
#endif
#include "stack/ble/service/ble_ll_ota.h"

void bls_set_advertise_prepare(void *p); // add ll_adv.h

RAM uint8_t ble_connected; // BIT(CONNECTED_FLG_BITS): bit 0 - connected, bit 1 - conn_param_update, bit 2 - paring success, bit 7 - reset of disconnect

#if (DEV_SERVICES & SERVICE_LE_LR) // support extension advertise

#define	APP_ADV_SETS_NUMBER						1			// Number of Supported Advertising Sets
#define APP_MAX_LENGTH_ADV_DATA					64			// Maximum Advertising Data Length,   (if legacy ADV, max length 31 bytes is enough)
#define APP_MAX_LENGTH_SCAN_RESPONSE_DATA		31			// Maximum Scan Response Data Length, (if legacy ADV, max length 31 bytes is enough)

RAM	u8	app_adv_set_param[ADV_SET_PARAM_LENGTH * APP_ADV_SETS_NUMBER]; // struct ll_ext_adv_t
RAM	u8	app_primary_adv_pkt[MAX_LENGTH_PRIMARY_ADV_PKT * APP_ADV_SETS_NUMBER];
RAM	u8	app_secondary_adv_pkt[MAX_LENGTH_SECOND_ADV_PKT * APP_ADV_SETS_NUMBER];
RAM	u8	app_advData[APP_MAX_LENGTH_ADV_DATA	* APP_ADV_SETS_NUMBER];
RAM	u8	app_scanRspData[APP_MAX_LENGTH_SCAN_RESPONSE_DATA * APP_ADV_SETS_NUMBER];

extern u32 blt_advExpectTime;
extern  void * ll_module_adv_cb;

#endif


uint8_t send_buf[SEND_BUFFER_SIZE];

RAM uint8_t blt_rxfifo_b[64 * 8] = { 0 };
RAM my_fifo_t blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b, };
RAM uint8_t blt_txfifo_b[40 * 16] = { 0 };
RAM my_fifo_t blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b, };
RAM uint8_t ble_name[MAX_DEV_NAME_LEN+2]; /* = { 11, GAP_ADTYPE_LOCAL_NAME_COMPLETE,
#if ((DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_MHO_C401N)) || (DEVICE_TYPE == DEVICE_MHO_C122)
		'M', 'H', 'O', '_', '0', '0', '0', '0',	'0', '0' };
#elif DEVICE_TYPE == DEVICE_CGG1
		'C', 'G', 'G', '_', '0', '0', '0', '0',	'0', '0' };
#elif DEVICE_TYPE == DEVICE_CGDK2
		'C', 'G', 'D', '_', '0', '0', '0', '0',	'0', '0' };
#elif DEVICE_TYPE == DEVICE_MJWSD05MMC
		'B', 'T', 'H', '_', '0', '0', '0', '0',	'0', '0' };
#else
		'A', 'T', 'C', '_', '0', '0', '0', '0',	'0', '0' };
#endif
*/

RAM uint8_t mac_public[6], mac_random_static[6];
RAM adv_buf_t adv_buf;
RAM uint8_t ota_is_working; // OTA_STAGES:  =1 ota enabled, =2 - ota wait, = 0xff flag ext.ota

void app_enter_ota_mode(void) {
#if (DEV_SERVICES & SERVICE_OTA_EXT)
	if(ota_is_working != OTA_EXTENDED)
#endif
	{
		ota_is_working = OTA_WORK;
		SHOW_OTA_SCREEN();
	}
	ble_connected &= ~BIT(CONNECTED_FLG_PAR_UPDATE);
	bls_pm_setManualLatency(0);
	bls_ota_setTimeout(16 * 1000000); // set OTA timeout  16 seconds
}

void ble_disconnect_callback(uint8_t e, uint8_t *p, int n) {
	if (ble_connected & BIT(CONNECTED_FLG_RESET_OF_DISCONNECT)) // reset device on disconnect?
		start_reboot();

	ble_connected = 0;
	ota_is_working = OTA_NONE;
	mi_key_stage = 0;
#if (DEV_SERVICES & SERVICE_SCREEN)
	lcd_flg.all_flg = 0;
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
	rd_memo.cnt = 0;
#endif
	if (cfg.flg.tx_measures)
		tx_measures = 0xff;
	else
		tx_measures = 0;
#if (DEV_SERVICES & SERVICE_LE_LR)
#if defined(GPIO_KEY2) || (DEV_SERVICES & SERVICE_RDS)
	set_adv_con_time(1); // restore default adv. interval
	bls_pm_setManualLatency(0);
	if(adv_buf.ext_adv_init) {
		// patch: restart ext_adv after 1 second
		ll_ext_adv_t *pea = (ll_ext_adv_t *)&app_adv_set_param;
		blt_advExpectTime = clock_time() + CLOCK_16M_SYS_TIMER_CLK_1S; // set time next ext.adv
		pea->advInt_use = adv_interval; // restore next ext.adv. interval
		pea->extAdv_en = 1;
	}
#else
	test_config();
	if(adv_buf.ext_adv_init) {
		// patch: restart ext_adv after 1 second
		ll_ext_adv_t *pea = (ll_ext_adv_t *)&app_adv_set_param;
		blt_advExpectTime = clock_time() + CLOCK_16M_SYS_TIMER_CLK_1S; // set time next ext.adv
		pea->advInt_use = adv_interval; // restore next ext.adv. interval
		pea->extAdv_en = 1;
	} else {
		ev_adv_timeout(0,0,0);
	}
#endif // #if defined(GPIO_KEY2) || (DEV_SERVICES & SERVICE_RDS)
#endif // #if (DEV_SERVICES & SERVICE_LE_LR)
	SHOW_CONNECTED_SYMBOL(false);
}

void ble_connect_callback(uint8_t e, uint8_t *p, int n) {
	(void) e; (void) p; (void) n;

	ble_connected = BIT(CONNECTED_FLG_ENABLE);
	tim_measure = clock_time();
	if(cfg.connect_latency > DEF_CONNECT_LATENCY && measured_data.average_battery_mv < LOW_VBAT_MV)
		cfg.connect_latency = DEF_CONNECT_LATENCY;
	my_periConnParameters.latency = cfg.connect_latency;
	if (cfg.connect_latency) {
		my_periConnParameters.intervalMin = DEF_CON_INERVAL; // 16*1.25 = 20 ms
		my_periConnParameters.intervalMax = DEF_CON_INERVAL; // 16*1.25 = 20 ms
	}
	my_periConnParameters.timeout = connection_timeout;
	bls_l2cap_requestConnParamUpdate(my_periConnParameters.intervalMin, my_periConnParameters.intervalMax, my_periConnParameters.latency, my_periConnParameters.timeout);
	SHOW_CONNECTED_SYMBOL(true);
#if defined(GPIO_KEY2) || (DEV_SERVICES & SERVICE_RDS)
	ext_key.rest_adv_int_tad = 0;
#endif
	bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1000);
}

#ifdef CHG_CONN_PARAM
int chgConnParameters(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	gap_periConnectParams_t * q = (gap_periConnectParams_t *)&req->dat;
	if (req->l2cap - 3 == sizeof(my_periConnParameters)
			&& q->intervalMin >= 7
			&& q->intervalMax >= q->intervalMin
			&& q->timeout > ((q->intervalMax >> 2) + 1)) {
		if (q->intervalMax != q->intervalMin)
			q->latency = 0;
		if(memcmp(&my_periConnParameters, q, sizeof(my_periConnParameters))!= 0) {
			memcpy(&my_periConnParameters, q, sizeof(my_periConnParameters));
			bls_l2cap_requestConnParamUpdate(q->intervalMin, q->intervalMax, q->latency, q->timeout);
			//bls_pm_setManualLatency(q->latency);
		}
	}
	return 0;
}
#endif

int app_conn_param_update_response(u8 id, u16  result) {
	if (result == CONN_PARAM_UPDATE_ACCEPT)
		ble_connected |= BIT(CONNECTED_FLG_PAR_UPDATE);
	else if (result == CONN_PARAM_UPDATE_REJECT) {
		// TODO: необходимо подбирать другие параметры соединения если внешний адаптер не согласен или плюнуть и послать,
		// что является лучшим решением для OTA, т.к. использовать плохие и сверх медленные адаптеры типа ESP32 для OTA нет смысла.
		bls_l2cap_requestConnParamUpdate(my_periConnParameters.intervalMin, my_periConnParameters.intervalMax, my_periConnParameters.latency, my_periConnParameters.timeout);

	}
	return 0;
}

//extern u32 blt_ota_start_tick; // in "stack/ble/service/ble_ll_ota.h"

int otaWritePre(void * p) {
	blt_ota_start_tick = clock_time() | 1;
	return otaWrite(p);
}

_attribute_ram_code_
int app_advertise_prepare_handler(rf_packet_adv_t * p)	{
	(void) p;
#if (DEV_SERVICES & SERVICE_RDS)
	if (!blta.adv_duraton_en)
#endif
	{
		if(++adv_buf.meas_count >= cfg.measure_interval) {
			adv_buf.meas_count = 0;
			start_measure = 1;
		}
		if (flg_measured & BIT(FLG_UPDATE_ADV)) { // new measured_data ?
			flg_measured &= ~BIT(FLG_UPDATE_ADV);
			adv_buf.call_count = 1; // count 1..cfg.measure_interval
			adv_buf.send_count++; // count & id advertise, = beacon_nonce.cnt32
			adv_buf.update_count = 0;
			set_adv_data();
		} else {
#if (DEV_SERVICES & SERVICE_RDS)
			if (!adv_buf.data_size) // flag
				adv_buf.send_count++; // count & id advertise, = beacon_nonce.cnt32
#endif
			if (++adv_buf.call_count > adv_buf.update_count) // refresh adv_buf.data ?
				set_adv_data();
		}
#if defined(GPIO_KEY2) || USE_WK_RDS_COUNTER
		if(ext_key.rest_adv_int_tad) {
			ext_key.rest_adv_int_tad--;
#if (!USE_EPD)
			if (ext_key.rest_adv_int_tad & 1)
				SET_LCD_UPDATE(); // show "ble"
#endif
		}
#endif // GPIO_KEY2 || (DEV_SERVICES & SERVICE_RDS)
	}
#if (DEV_SERVICES & SERVICE_RDS)
	else {
		// restore next adv. interval
		blta.adv_interval = EXT_ADV_INTERVAL*625*CLOCK_16M_SYS_TIMER_CLK_1US; // system tick
	}
#endif
	return 1;		// = 1 ready to send ADV packet, = 0 not send ADV
}

#if (DEV_SERVICES & SERVICE_LE_LR)
_attribute_ram_code_ int _blt_ext_adv_proc (void) {
#if defined(GPIO_KEY2) || (DEV_SERVICES & SERVICE_RDS)
	if(adv_buf.ext_adv_init && blta.adv_duraton_en) {
		blta.adv_duraton_en--;
		if(blta.adv_duraton_en == 0) {
			ll_ext_adv_t *pea = (ll_ext_adv_t *)&app_adv_set_param;
			pea->advInt_use = adv_interval; // restore next ext.adv. interval
		}
	} else
#endif
		app_advertise_prepare_handler(0);
	return blt_ext_adv_proc();
}
#endif

_attribute_ram_code_ int RxTxWrite(void * p) {
	cmd_parser(p);
	return 0;
}

/* moved app.c: suspend_exit_cb
_attribute_ram_code_ void user_set_rf_power(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	rf_set_power_level_index(cfg.rf_tx_power);
}
*/

#if (DEV_SERVICES & SERVICE_LE_LR)
/* cfg.rf_tx_power -> ext.advertising TX power */
int ext_adv_tx_level(void) {
	int txl = TX_POWER_0dBm;
	if ((cfg.rf_tx_power & 0x80) == 0) {
		if (cfg.rf_tx_power < RF_POWER_P3p94dBm)
			txl = TX_POWER_3dBm;
		else if(cfg.rf_tx_power < RF_POWER_P5p13dBm)
			txl = TX_POWER_4dBm;
		else if(cfg.rf_tx_power < RF_POWER_P6p14dBm)
			txl = TX_POWER_5dBm;
		else if(cfg.rf_tx_power < RF_POWER_P7p02dBm)
			txl = TX_POWER_6dBm;
		else if(cfg.rf_tx_power < RF_POWER_P8p13dBm)
			txl = TX_POWER_7dBm;
		else if(cfg.rf_tx_power < RF_POWER_P9p24dBm)
			txl = TX_POWER_8dBm;
		else if(cfg.rf_tx_power < RF_POWER_P10p01dBm)
			txl = TX_POWER_9dBm;
		else
			txl = TX_POWER_10dBm;
	} else if (cfg.rf_tx_power < RF_POWER_P1p17dBm)
		txl = TX_POWER_0dBm;
	else if(cfg.rf_tx_power < RF_POWER_P2p39dBm)
		txl = TX_POWER_1dBm;
	else if(cfg.rf_tx_power < RF_POWER_P3p01dBm)
		txl = TX_POWER_2dBm;
	else
		txl = TX_POWER_3dBm;
	return txl;
}
#endif // #if (DEV_SERVICES & SERVICE_LE_LR)
/*
 * bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);
 * blt_event_callback_t(): */
void ev_adv_timeout(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
#if (DEV_SERVICES & SERVICE_LE_LR)
	if (adv_buf.ext_adv_init) { // extension advertise
		if(ble_connected)
			return;
		if(adv_buf.ext_adv_init == 3) {
			//adv_set: Legacy
			blc_ll_setExtAdvParam(ADV_HANDLE0,
					ADV_EVT_PROP_LEGACY_CONNECTABLE_SCANNABLE_UNDIRECTED,
					CONNECTABLE_ADV_INERVAL, CONNECTABLE_ADV_INERVAL + ADV_DELAY,
					BLT_ENABLE_ADV_ALL, // primary advertising channel map
					OWN_ADDRESS_PUBLIC, // own address type
					BLE_ADDR_PUBLIC, // peer address type
					NULL, // * peer address
					ADV_FP_NONE, // advertising filter policy
					ext_adv_tx_level(), // cfg.rf_tx_power -> advertising TX power
					BLE_PHY_1M, // primary advertising channel PHY type
					0, // secondary advertising minimum skip number
					BLE_PHY_1M, // secondary advertising channel PHY type
					ADV_SID_0,
					0); // scan response notify enable ?
		} else {
			//adv_set: Extended, Connectable_undirected
			blc_ll_setExtAdvParam(ADV_HANDLE0,
				ADV_EVT_PROP_EXTENDED_CONNECTABLE_UNDIRECTED,
				adv_interval, adv_interval + ADV_DELAY,
				BLT_ENABLE_ADV_ALL, // primary advertising channel map
				OWN_ADDRESS_PUBLIC, // own address type
				BLE_ADDR_PUBLIC, // peer address type
				NULL, // * peer address
				ADV_FP_NONE, // advertising filter policy
				ext_adv_tx_level(), // cfg.rf_tx_power -> advertising TX power
				BLE_PHY_CODED, // primary advertising channel PHY type
				0, // secondary advertising minimum skip number
				BLE_PHY_CODED, // secondary advertising channel PHY type
				ADV_SID_0,
				0); // scan response notify enable ?
		}
		blc_ll_setExtScanRspData(ADV_HANDLE0, DATA_OPER_COMPLETE, DATA_FRAGM_ALLOWED,
				ble_name[0]+1, (uint8_t *) ble_name);

		// debug:  Fix CodedPHY channel
		// blc_ll_setAuxAdvChnIdxByCustomers(20); // auxiliary data channel, must be range of 0~36

		bls_ll_setScanRspData((uint8_t *) ble_name, ble_name[0]+1);
		blc_ll_setExtAdvEnable_1(BLC_ADV_ENABLE, 1, ADV_HANDLE0, 0 , 0);
	} else
#endif
	{
		bls_ll_setAdvParam(adv_interval, adv_interval + ADV_DELAY,
			ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
			BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
		bls_ll_setScanRspData((uint8_t *) ble_name, ble_name[0]+1);
		bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //ADV enable
	}
}

#if defined(GPIO_KEY2) || (DEV_SERVICES & SERVICE_RDS)
/**
 * @brief This function change adv. interval.
 * @param[in]  != 0 restore default adv. interval, = 0 connection adv. interval
 * @return none
 */
void set_adv_con_time(int restore) {
	if(restore) {
		test_config(); // restore adv_interval
		ext_key.rest_adv_int_tad = 0; // stop timer event restore adv.intervals
	} else {
		adv_interval = CONNECTABLE_ADV_INERVAL; // new adv.intervals = 1 sec
		ext_key.rest_adv_int_tad = -1; // start timer event restore adv.intervals
	}
	if(!ble_connected) {
#if (DEV_SERVICES & SERVICE_LE_LR)
		if (adv_buf.ext_adv_init) { // support extension advertise
			if(restore)
				adv_buf.ext_adv_init = 1; // LE long range
			else
				adv_buf.ext_adv_init = 3; // legacy
			blc_ll_removeAdvSet(ADV_HANDLE0);
			ev_adv_timeout(0,0,0);
			load_adv_data();
/*
			// patch: restart ext_adv after 1 second
			ll_ext_adv_t *pea = (ll_ext_adv_t *)&app_adv_set_param;
			blt_advExpectTime = clock_time() + CLOCK_16M_SYS_TIMER_CLK_1S; // set time next ext.adv
			pea->advInt_use = adv_interval; // restore next ext.adv. interval
			pea->extAdv_en = 1;
*/
		}
		else
#endif
			ev_adv_timeout(0,0,0);
			load_adv_data();
	}
}
#endif // GPIO_KEY2 || (DEV_SERVICES & SERVICE_RDS)

#if (DEV_SERVICES & SERVICE_PINCODE)
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
		if (p->bonding && p->bonding_result)  // paring success ?
			ble_connected |= BIT(CONNECTED_FLG_BONDING);
	} else if (event == GAP_EVT_SMP_PARING_FAIL) {
		//gap_smp_paringFailEvt_t * p = (gap_smp_paringFailEvt_t *)para;
	} else if (event == GAP_EVT_SMP_TK_REQUEST_PASSKEY) {
		//blc_smp_setTK_by_PasskeyEntry(pincode);
	} else if (event == GAP_EVT_SMP_PARING_BEAGIN) {
		//gap_smp_paringBeginEvt_t * p = (gap_smp_paringBeginEvt_t*)para;
		// ...
	} else if (event == GAP_EVT_SMP_TK_REQUEST_OOB) {
		//blc_smp_setTK_by_OOB();
	} else if (event == GAP_EVT_SMP_TK_NUMERIC_COMPARE) {
		//uint32_t * pin = (uint32_t*)para;
		//blc_smp_setNumericComparisonResult(*pin == pincode);
	} else if (event == GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE) {
#endif
	}
	return 0;
}
#endif

extern attribute_t my_Attributes[ATT_END_H];

void ble_set_name(void) {
	int16_t len = flash_read_cfg(&ble_name[2], EEP_ID_DVN, min(sizeof(ble_name)-3, MAX_DEV_NAME_LEN));
	if (len < 1 || len > MAX_DEV_NAME_LEN) {
		//Set the BLE Name to the last three MACs the first ones are always the same
#if ((DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_MHO_C401N) || (DEVICE_TYPE == DEVICE_MHO_C122))
		ble_name[2] = 'M';
		ble_name[3] = 'H';
		ble_name[4] = 'O';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_CGG1
		ble_name[2] = 'C';
		ble_name[3] = 'G';
		ble_name[4] = 'G';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_CGDK2
		ble_name[2] = 'C';
		ble_name[3] = 'G';
		ble_name[4] = 'D';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_MJWSD05MMC
		ble_name[2] = 'B';
		ble_name[3] = 'T';
		ble_name[4] = 'H';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_TS0201
		ble_name[2] = 'T';
		ble_name[3] = 'S';
		ble_name[4] = '0';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_TH03Z
		ble_name[2] = 'T';
		ble_name[3] = 'H';
		ble_name[4] = '0';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_ZTH01
		ble_name[2] = 'Z';
		ble_name[3] = 'T';
		ble_name[4] = 'H';
		ble_name[5] = '_';
#elif DEVICE_TYPE == DEVICE_ZTH02
		ble_name[2] = 'Z';
		ble_name[3] = 'T';
		ble_name[4] = 'H';
		ble_name[5] = '_';
#else
		ble_name[2] = 'A';
		ble_name[3] = 'T';
		ble_name[4] = 'C';
		ble_name[5] = '_';
#endif
		uint8_t *p = str_bin2hex(&ble_name[6], &mac_public[2], 1);
		p = str_bin2hex(p, &mac_public[1], 1);
		p = str_bin2hex(p, &mac_public[0], 1);
		len = 10;
	}
	my_Attributes[GenericAccess_DeviceName_DP_H].attrLen = len;
	ble_name[0] = (uint8_t)(len + 1);
	ble_name[1] = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
}


_attribute_ram_code_
__attribute__((optimize("-Os")))
void load_adv_data(void) {
	u8 size = adv_buf.data_size;
	if((!size)
#if (DEV_SERVICES & SERVICE_LE_LR)
		|| adv_buf.ext_adv_init == 1 // extension advertise LE long Range ?
#endif
		) {
		memcpy(&adv_buf.data[size], ble_name, ble_name[0] + 1);
		size += ble_name[0] + 1;
	}
	u8 *p = adv_buf.flag;
	if (cfg.flg2.adv_flags)
		size += sizeof(adv_buf.flag);
	else
		p += sizeof(adv_buf.flag);
#if (DEV_SERVICES & SERVICE_LE_LR)
	if (adv_buf.ext_adv_init)  // support extension advertise
		blc_ll_setExtAdvData(ADV_HANDLE0, DATA_OPER_COMPLETE, DATA_FRAGM_ALLOWED, size, p);
	else
#endif
	bls_ll_setAdvData(p, size);
}

__attribute__((optimize("-Os"))) void init_ble(void) {
	////////////////// adv buffer initialization //////////////////////
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
	adv_buf.flag[2] = 0x04 | GAP_ADTYPE_LE_GENERAL_DISCOVERABLE_MODE_BIT; // Flags
	////////////////// BLE stack initialization //////////////////////
	blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
	/// if bls_ll_setAdvParam( OWN_ADDRESS_RANDOM ) ->  blc_ll_setRandomAddr(mac_random_static);
	ble_set_name();
	////// Controller Initialization  //////////
	//blc_ll_initBasicMCU(); // -> app.c user_init_normal()
	blc_ll_initStandby_module(mac_public); //must
	blc_ll_initAdvertising_module(mac_public); // adv module: 		 must for BLE slave,
	blc_ll_initConnection_module(); // connection module  must for BLE slave/master
	blc_ll_initSlaveRole_module(); // slave module: 	 must for BLE slave,
#if (DEV_SERVICES & SERVICE_LE_LR)
	adv_buf.ext_adv_init = 0;
#endif
	if (cfg.flg2.bt5phy) { // 1M, 2M, Coded PHY
		blc_ll_init2MPhyCodedPhy_feature();	//if use 2M or Coded PHY
		// set Default Connection Coding
		blc_ll_setDefaultConnCodingIndication(CODED_PHY_PREFER_S8);
		// set Default Connection Coding
		// set CSA2
		blc_ll_initChannelSelectionAlgorithm_2_feature();
		//bls_app_registerEventCallback (BLT_EV_FLAG_PHY_UPDATE, &callback_phy_update_complete_event);
#if (DEV_SERVICES & SERVICE_LE_LR)
		if(cfg.flg2.longrange) { // support extension advertise Coded PHY
			// init buffers ext adv
			// and ll_module_adv_cb = blt_ext_adv_proc; pFunc_ll_SetAdv_Enable = ll_setExtAdv_Enable;
			blc_ll_initExtendedAdvertising_module(app_adv_set_param, app_primary_adv_pkt, APP_ADV_SETS_NUMBER);
			blc_ll_initExtSecondaryAdvPacketBuffer(app_secondary_adv_pkt, MAX_LENGTH_SECOND_ADV_PKT);
			blc_ll_initExtAdvDataBuffer(app_advData, APP_MAX_LENGTH_ADV_DATA);
			blc_ll_initExtScanRspDataBuffer(app_scanRspData, APP_MAX_LENGTH_SCAN_RESPONSE_DATA);
			// if Coded PHY is used, this API set default S2/S8 mode for Extended ADV
			blc_ll_setDefaultExtAdvCodingIndication(ADV_HANDLE0, CODED_PHY_PREFER_S8);
			// patch, set advertise prepare user cb (app_advertise_prepare_handler)
			ll_module_adv_cb = _blt_ext_adv_proc;
			adv_buf.ext_adv_init = 1; // flag ext_adv init
			blc_ll_setDefaultPhy(PHY_TRX_PREFER, BLE_PHY_CODED, BLE_PHY_CODED);
		} else
#endif
			blc_ll_setDefaultPhy(PHY_TRX_PREFER, PHY_PREFER_1M, PHY_PREFER_1M);
	}
	////// Host Initialization  //////////
	blc_gap_peripheral_init();
	my_att_init(); //gatt initialization
	blc_l2cap_register_handler(blc_l2cap_packet_receive);

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//   is about to exceed the sector threshold, this sector must be erased, and all useful information
	//   should re_stored) , so it must be done after battery check
#if (DEV_SERVICES & SERVICE_PINCODE)
	if (pincode) {
		//adv_buf.flag[2] = 0x04 | GAP_ADTYPE_LE_LIMITED_DISCOVERABLE_MODE_BIT; // Flags

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
	rf_set_power_level_index(cfg.rf_tx_power);
	// bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power); // -> app.c: suspend_exit_cb
	bls_app_registerEventCallback(BLT_EV_FLAG_CONNECT, &ble_connect_callback);
	bls_app_registerEventCallback(BLT_EV_FLAG_TERMINATE, &ble_disconnect_callback);

	///////////////////// Power Management initialization///////////////////
	blc_ll_initPowerManagement_module();
	bls_pm_setSuspendMask(SUSPEND_DISABLE);
	blc_pm_setDeepsleepRetentionThreshold(40, 18);
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(240);
	blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);
	bls_ota_clearNewFwDataArea();
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
	blc_l2cap_registerConnUpdateRspCb(app_conn_param_update_response);
	bls_set_advertise_prepare(app_advertise_prepare_handler); // TODO: not work if EXTENDED_ADVERTISING
#if defined(GPIO_KEY2) || (DEV_SERVICES & SERVICE_RDS)
	bls_app_registerEventCallback(BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);
#endif
	ev_adv_timeout(0,0,0);	// set advertise config
}

/* adv_type: 0 - atc1441, 1 - Custom,  2 - Mi, 3 - HA_BLE  */
_attribute_ram_code_
__attribute__((optimize("-Os")))
void set_adv_data(void) {
	uint8_t adv_type = cfg.flg.advertising_type;
#if (DEV_SERVICES & SERVICE_BINDKEY)
	if (cfg.flg2.adv_crypto) {
		if (adv_type == ADV_TYPE_PVVX) {
			pvvx_encrypt_data_beacon();
#if USE_MIHOME_BEACON
		} else if (adv_type == ADV_TYPE_MI) { // adv_type == 2
			mi_encrypt_data_beacon();
#endif
#if USE_BTHOME_BEACON
		} else if (adv_type == ADV_TYPE_BTHOME) { // adv_type == 3
			bthome_encrypt_data_beacon();
#endif
		} else { // adv_type == 0 == ADV_TYPE_ATC
			atc_encrypt_data_beacon();
		}
	} else
#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)
	{
		if (adv_type == ADV_TYPE_PVVX) {
			pvvx_data_beacon();
#if USE_MIHOME_BEACON
		} else if (adv_type == ADV_TYPE_MI) { // adv_type == 2
			mi_data_beacon();
#endif
#if USE_BTHOME_BEACON
		} else if (adv_type == ADV_TYPE_BTHOME) { // adv_type == 3
			bthome_data_beacon();
#endif
		} else { // adv_type == 0 == ADV_TYPE_ATC
			atc_data_beacon();
		}
	}
	adv_buf.data_size = adv_buf.data[0] + 1;
	load_adv_data();
}

_attribute_ram_code_ void ble_send_measures(void) {
	send_buf[0] = CMD_ID_MEASURE;
	memcpy(&send_buf[1], &measured_data, MEASURED_MSG_SIZE);
#if (DEV_SERVICES & SERVICE_TH_TRG)
	send_buf[MEASURED_MSG_SIZE+1] = trg.flg_byte;
#if (DEV_SERVICES & SERVICE_RDS)
	send_buf[MEASURED_MSG_SIZE+2] = rds.count_byte[0];
	send_buf[MEASURED_MSG_SIZE+3] = rds.count_byte[1];
	send_buf[MEASURED_MSG_SIZE+4] = rds.count_byte[2];
	send_buf[MEASURED_MSG_SIZE+5] = rds.count_byte[3];
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, MEASURED_MSG_SIZE + 6);
#else
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, MEASURED_MSG_SIZE + 2);
#endif
#else
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, MEASURED_MSG_SIZE + 1);
#endif
}

#if (DEV_SERVICES & SERVICE_SCREEN)
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
#endif

#if (DEV_SERVICES & SERVICE_TH_TRG)
void ble_send_trg(void) {
	send_buf[0] = CMD_ID_TRG;
	memcpy(&send_buf[1], &trg, sizeof(trg));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, sizeof(trg) + 1);
}
void ble_send_trg_flg(void) {
	test_trg_on();
	send_buf[0] = CMD_ID_TRG_OUT;
	send_buf[1] = *((uint8_t *)(&trg.flg));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, 2);
}
#endif

#if (DEV_SERVICES & SERVICE_HISTORY)
__attribute__((optimize("-Os")))
void send_memo_blk(void) {
	send_buf[0] = CMD_ID_LOGGER;
	if (++rd_memo.cur > rd_memo.cnt || (!get_memo(rd_memo.cur, (pmemo_blk_t)&send_buf[3]))) {
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
