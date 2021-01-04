#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/user_config.h"

#include "i2c.h"

extern void user_init_normal(void);
extern void user_init_deepRetn(void);
extern void main_loop(void);

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler();
}

_attribute_ram_code_ int main (void)    //must run in ramcode
{
	blc_pm_select_internal_32k_crystal();
	cpu_wakeup_init();
	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp
	gpio_init(!deepRetWakeUp);  //analog resistance will keep available in deepSleep mode, so no need initialize again
	rf_drv_init(RF_MODE_BLE_1M);
#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	clock_init(SYS_CLK_16M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	clock_init(SYS_CLK_24M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
	clock_init(SYS_CLK_32M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
	clock_init(SYS_CLK_48M_Crystal);
#endif
	reg_clk_en0 = 0 //FLD_CLK0_I2C_EN
//			| FLD_CLK0_UART_EN
			| FLD_CLK0_SWIRE_EN;
//	reg_clk_en1 = FLD_CLK1_ZB_EN | FLD_CLK1_SYS_TIMER_EN | FLD_CLK1_DMA_EN | FLD_CLK1_ALGM_EN;
//	reg_clk_en2 = FLD_CLK2_DFIFO_EN | FLD_CLK2_MC_EN | FLD_CLK2_MCIC_EN;
	adc_power_on_sar_adc(0); // - 0.4 mA
	lpc_power_down();
	blc_app_loadCustomizedParameters();

	if( deepRetWakeUp ){
		user_init_deepRetn();
	}
	else{
		user_init_normal();
	}	
    irq_enable();
	while(1) {
		main_loop();
	}
}

