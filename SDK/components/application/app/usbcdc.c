/********************************************************************************************************
 * @file     usbcdc.c 
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

#if(USB_CDC_ENABLE)

#include "usbcdc.h"
#include "../drivers/usbhw.h"
#include "../drivers/usbhw_i.h"
#include "../drivers/usb.h"
#include "../os/ev.h"
//#include "../os/sys.h"
#include "../os/ev_queue.h"
#include "../os/ev_buffer.h"



void usbcdc_write32(u32 value);
void usbcdc_read32(u32* value);
int usbcdc_recvTimeoutCb(void* arg);


typedef struct {
    u8 *rxBuf;
	u8 *txBuf;

	/* Following variables are used in the RX more than CDC_TXRX_EPSIZE */
	ev_time_event_t timer;
	u8  lastIndex;


	u16 lenToSend;
	u16 lastSendIndex;
	
	cdc_handlerFn_t rxCb;
    cdc_handlerFn_t txCb;
	
} cdc_ctrl_t;

#ifdef STATIC_V_INST
cdc_ctrl_t cdc_vs;
#endif
cdc_ctrl_t *cdc_v;





USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
{
	//.Config =
    {
	    0, //ControlInterfaceNumber

		CDC_TX_EPNUM, // DataINEndpointNumber
		CDC_TXRX_EPSIZE, // DataINEndpointSize
		false, // DataINEndpointDoubleBank

		CDC_RX_EPNUM,  // DataOUTEndpointNumber
		CDC_TXRX_EPSIZE, // DataOUTEndpointSize
		false, // DataOUTEndpointDoubleBank

		CDC_NOTIFICATION_EPNUM, // NotificationEndpointNumber
		CDC_NOTIFICATION_EPSIZE, // NotificationEndpointSize
		false,  // NotificationEndpointDoubleBank
	},
};

USB_ClassInfo_CDC_Device_t *CDCInterfaceInfo = &VirtualSerial_CDC_Interface; 




void CDC_Device_ProcessControlRequest(u8 bRequest, u16 wValue, u16 wIndex, u16 wLength)
{

	if (wIndex != CDCInterfaceInfo->Config.ControlInterfaceNumber)
	  return;

	switch (bRequest)
	{
		case CDC_REQ_GetLineEncoding:
		
            usbcdc_write32(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
			usbhw_write_ctrl_ep_data(CDCInterfaceInfo->State.LineEncoding.CharFormat);
            usbhw_write_ctrl_ep_data(CDCInterfaceInfo->State.LineEncoding.ParityType);
            usbhw_write_ctrl_ep_data(CDCInterfaceInfo->State.LineEncoding.DataBits);
			break;
			
		case CDC_REQ_SetLineEncoding:
			
			usbcdc_read32(&CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
			CDCInterfaceInfo->State.LineEncoding.CharFormat  = usbhw_read_ctrl_ep_data();
			CDCInterfaceInfo->State.LineEncoding.ParityType  = usbhw_read_ctrl_ep_data();
			CDCInterfaceInfo->State.LineEncoding.DataBits    = usbhw_read_ctrl_ep_data();

			//EVENT_CDC_Device_LineEncodingChanged(CDCInterfaceInfo);
			break;
			
		case CDC_REQ_SetControlLineState:
			CDCInterfaceInfo->State.ControlLineStates.HostToDevice = wValue;
			//EVENT_CDC_Device_ControLineStateChanged(CDCInterfaceInfo);
			break;
			
		case CDC_REQ_SendBreak:

			break;
	}
}

void usbcdc_write32(u32 value)
{
	usbhw_write_ctrl_ep_data(value&0xff);
	usbhw_write_ctrl_ep_data((value>>8)&0xff);
	usbhw_write_ctrl_ep_data((value>>16)&0xff);
	usbhw_write_ctrl_ep_data((value>>24)&0xff);
}

void usbcdc_read32(u32* value)
{
	u32 temp = 0;
	*value = usbhw_read_ctrl_ep_data();
	
	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 8) | (*value); 
	temp = 0;

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 16) | (*value); 
	temp = 0;

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 24) | (*value); 
	
}


void usbcdc_init(void)
{
	/* Init UART parameters */
	CDCInterfaceInfo->State.LineEncoding.BaudRateBPS = 115200;
	CDCInterfaceInfo->State.LineEncoding.CharFormat = 0;
	CDCInterfaceInfo->State.LineEncoding.ParityType = 0;
	CDCInterfaceInfo->State.LineEncoding.DataBits = 8;
	

	cdc_v = &cdc_vs;
    cdc_v->lastIndex = 0;
    cdc_v->timer.cb = usbcdc_recvTimeoutCb;
}


void usbcdc_setRxBuf(u8 *buf)
{
	cdc_v->rxBuf = buf;
}

void usbcdc_setCB(cdc_handlerFn_t rxFunc, cdc_handlerFn_t txCb)
{
	cdc_v->rxCb = rxFunc;
    cdc_v->txCb = txCb;
}

int usbcdc_recvTimeoutCb(void* arg)
{
	u8* p;

	cdc_v->lastIndex = 0;

	/* Clear the buffer */
	p = cdc_v->rxBuf;
	cdc_v->rxBuf = NULL;

	/* Callback */
	if (cdc_v->rxCb) {
		cdc_v->rxCb(p);
	}

	return -1;
}


void usbcdc_recvData(void)
{
	u8 i;
	u8 *p;
	u8 len;
	u8 fEnd = 0;
	
	/* No buffer */
	if (!cdc_v->rxBuf) {
		while(1);
	}

	if (!is_timer_expired(&cdc_v->timer)) {
		ev_unon_timer(&cdc_v->timer);
	}

	len = reg_usb_ep_ptr(CDC_RX_EPNUM & 0x07);
	fEnd = (len == CDC_TXRX_EPSIZE) ? 0 : 1;
	usbhw_reset_ep_ptr(CDC_RX_EPNUM);

	for (i = 0; i < len; i++) {
		cdc_v->rxBuf[cdc_v->lastIndex++] = usbhw_read_ep_data(CDC_RX_EPNUM);
	}

	if (fEnd) {
		cdc_v->lastIndex = 0;

		/* Clear the buffer */
		p = cdc_v->rxBuf;
		cdc_v->rxBuf = NULL;

		/* Callback */
		if (cdc_v->rxCb) {
			cdc_v->rxCb(p);
		}
	} else {
		ev_on_timer(&cdc_v->timer, 500);
	}
}

u8 T_BUF[60];
u32 T_CNT;
u8 usbcdc_sendBulkData(void)
{
	u16 len;
	
	/* Wait until not busy */
	if (usbhw_is_ep_busy(CDC_TX_EPNUM)) {
        /* Return to wait IRQ come again */
        return 0;
    }

    /* Get the length to send in this bulk transaction */
	len = (cdc_v->lenToSend > CDC_TXRX_EPSIZE) ? CDC_TXRX_EPSIZE : cdc_v->lenToSend;
	cdc_v->lenToSend -= len;

	if (len == 0) {
		return 0;
	}
	

    reg_usb_ep_ptr(CDC_TX_EPNUM) = 0;

	/* Write data to USB fifo */
    foreach (i, len) {
    	T_BUF[i] = cdc_v->txBuf[cdc_v->lastSendIndex];
        reg_usb_ep_dat(CDC_TX_EPNUM) = cdc_v->txBuf[cdc_v->lastSendIndex++];
    }

	/* Write ACK */
    reg_usb_ep_ctrl(CDC_TX_EPNUM) = FLD_EP_DAT_ACK;        // ACK
    u16 t = 0;
    while(usbhw_is_ep_busy(CDC_TX_EPNUM)) {
        if (t++ > 10000) {
            T_CNT++;
            reg_usb_ep_ctrl(CDC_TX_EPNUM) &= 0xfe; // clear bit(0)
        }
    };

    /* TX transaction finish */
	if (cdc_v->lenToSend == 0) {
		cdc_v->lenToSend = 0;
		cdc_v->lastSendIndex = 0;

        if (cdc_v->txCb) {
            EV_SCHEDULE_TASK(cdc_v->txCb, cdc_v->txBuf);
        }

		cdc_v->txBuf = NULL;
	}

	return len;
}





usbcdc_sts_t usbcdc_sendData(u8 *buf, u8 len)
{
	if (cdc_v->txBuf) {
		return USB_BUSY;
	}

	/* Init the bulk transfer */
    cdc_v->lenToSend = len;
	cdc_v->txBuf = buf;
	cdc_v->lastSendIndex = 0;

    /* Send first bulk */
	usbcdc_sendBulkData();

	usbhw_data_ep_ack(USB_EDP_CDC_OUT);
	
	return SUCCESS;
	
   
}


u8 usbcdc_isAvailable(void)
{
	return (cdc_v->txBuf == NULL);
}


#endif  /* USB_CDC_ENABLE */
