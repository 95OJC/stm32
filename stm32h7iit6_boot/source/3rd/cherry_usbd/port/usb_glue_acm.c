/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usb_config.h"
#include "stdint.h"
#include "usb_dwc2_reg.h"


uint32_t usbd_get_dwc2_gccfg_conf(uint32_t reg_base)
{
	return 0;
}

uint32_t usbh_get_dwc2_gccfg_conf(uint32_t reg_base)
{
	return 0;
}


int OTG_ID_ChangeChk(DWC2_GlobalTypeDef *pUsbOtg)
{

	if(pUsbOtg->GINTSTS & USB_OTG_GINTSTS_CIDSCHG)	//ID status changed
	{
		pUsbOtg->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;

		return 1;
	}

	return 0;
}