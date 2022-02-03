/********************************************************************************************************
 * @file     rf_pa.c
 *
 * @brief    This is the header file for TLSR8258
 *
 * @author	 public@telink-semi.com;
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
 *
 *******************************************************************************************************/

#include "rf_pa.h"
#include "gpio.h"
#include "compiler.h"

_attribute_data_retention_	rf_pa_callback_t  blc_rf_pa_cb = 0;

void app_rf_pa_handler(int type)
{
#if(PA_ENABLE)
	if(type == PA_TYPE_TX_ON){
	    gpio_set_output_en(PA_RXEN_PIN, 0);
	    gpio_write(PA_RXEN_PIN, 0);
	    gpio_set_output_en(PA_TXEN_PIN, 1);
	    gpio_write(PA_TXEN_PIN, 1);
	}
	else if(type == PA_TYPE_RX_ON){
	    gpio_set_output_en(PA_TXEN_PIN, 0);
	    gpio_write(PA_TXEN_PIN, 0);
	    gpio_set_output_en(PA_RXEN_PIN, 1);
	    gpio_write(PA_RXEN_PIN, 1);
	}
	else{
	    gpio_set_output_en(PA_RXEN_PIN, 0);
	    gpio_write(PA_RXEN_PIN, 0);
	    gpio_set_output_en(PA_TXEN_PIN, 0);
	    gpio_write(PA_TXEN_PIN, 0);
	}
#endif
}


void rf_pa_init(void)
{
#if(PA_ENABLE)
    //rf_set_power_level_index (RF_POWER_0dBm);
    gpio_set_func(PA_TXEN_PIN, AS_GPIO);
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);

    gpio_set_func(PA_RXEN_PIN, AS_GPIO);
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);

    blc_rf_pa_cb = app_rf_pa_handler;
#endif
}

