/****************************************************************************
 *
 *            Copyright (c) 2003-2009 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#ifndef _STM32UH_H
#define _STM32UH_H

#ifdef __cplusplus
extern "C" {
#endif


#include "hcc_types.h"
#include "usb_host.h"
#include "os.h"
#include "stm32uh_hw.h"

#define MAX_DEVICE				1		/* maximum number of devices supported */
#define MAX_EP					4		/* max. number of bulk and interrupt endpoints */

#define MAX_NP_TRANSFERS		4
#define MAX_ISO_TRANSFERS		2
#define MAX_INT_TRANSFERS		2

#define STM32UH_MUTEX_COUNT		1		/* for transfers */
#define STM32UH_EVENT_COUNT		1		/* transfer complete */

#define STM32UH_HOST_ISR		0x32040000

int stm32uh_init (void);
int stm32uh_start (hcc_u32 p);
int stm32uh_stop (void);

void *stm32uh_add_ep (hcc_u8 daddr, hcc_u8 ep, hcc_u8 type, hcc_u8 dir, hcc_u16 psize, hcc_u8 interval);
int stm32uh_del_daddr (hcc_u8 daddr);
int stm32uh_clear_toggle (void *ep_handle);

int stm32uh_transfer (t_usbh_transfer *uht, hcc_u8 mode);
int stm32uh_transfer_status (t_usbh_transfer *uht, hcc_u8 mode);

int stm32uh_set_ep0_speed_hub (void *ep0_handle, hcc_u8 speed, hcc_u8 port, hcc_u8 hub_daddr, hcc_u8 hub_speed);
//void stm32uh_delay (hcc_u32 ms); //Jason
int stm32uh_delay (hcc_u32 ms);
int stm32uh_get_ep_inf (void *ep_handle, hcc_u8 *daddr, hcc_u8 *ep, hcc_u8 *type, hcc_u8 *dir, hcc_u16 *psize);

OS_TASK_FN(stm32uh_transfer_task);
OS_ISR_FN(stm32uh_isr_handler);

#ifdef __cplusplus
}
#endif

#endif


