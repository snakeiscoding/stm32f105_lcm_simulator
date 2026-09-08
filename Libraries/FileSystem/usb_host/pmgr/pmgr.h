/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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
#ifndef _PMGR_H
#define _PMGR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hcc_types.h"
#include "os.h"
#include "usb_utils.h"


/*
** HUB scan information
*/
typedef struct {
  hcc_u8 port;
  hcc_u8 state;
} t_hub_scan_inf;

typedef int (*hub_scan_fn) (void *dev_hdl, t_hub_scan_inf *hsi);
typedef int (*hub_fn) (void *dev_hdl, hcc_u8 port);


typedef hcc_u8 t_cd_ep_dsc;
#define USBH_EP_DSC(t,d)			((t<<2)|(d))
#define USBH_EP_DSC_t(v)			((v>>2)&0x3)
#define USBH_EP_DSC_d(v)			(v&0x3)

typedef void **(*cd_fn_check)(void **dev_hdl, void *port_hdl, device_info_t *dev_inf, ifc_info_t *ifc_inf);
typedef int(*cd_fn_connect)(void *dev_hdl, void *ifc_hdl);
typedef int(*cd_fn_disconnect)(void *dev_hdl);
typedef int(*cd_fn_descriptor)(void *dev_hdl, hcc_u8 *dsc);

extern OS_EVENT_BIT_TYPE pmgr_event_bit;


int pmgr_init (void);
int pmgr_start (void);
int pmgr_stop (void);
int pmgr_add_cd (cd_fn_check , cd_fn_connect , cd_fn_disconnect, cd_fn_descriptor, t_cd_ep_dsc *);
void *pmgr_add_hub (void *ifc_hdl, hub_scan_fn scan, hub_fn reset, hub_fn disable, hub_fn suspend, hub_fn resume);
int pmgr_delete_hub (void *hub_hdl);
OS_TASK_FN(pmgr_pcd_task);
hcc_u8 pmgr_get_port_number (USBH_DEV_ADDR daddr);
int pmgr_get_ifc_port_number (void *ifc_hdl, hcc_u8 *port);
hcc_u8 pmgr_get_dev_state (void *ifc_hdl);
int pmgr_suspend (void *ifc_hdl);
int pmgr_resume (void *ifc_hdl);

#ifdef __cplusplus
}
#endif


#endif

