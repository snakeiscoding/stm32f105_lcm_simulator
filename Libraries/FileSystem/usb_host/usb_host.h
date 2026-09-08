/***************************************************************************
 *
 *            Copyright (c) 2003-2007 by HCC Embedded
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
#ifndef _USB_HOST_H
#define _USB_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hcc_types.h"
#include "os.h"


#define USBH_DEV_ADDR				hcc_u8

#define USBH_SPEED_ERROR			0
#define USBH_LOW_SPEED				1
#define USBH_FULL_SPEED				2
#define USBH_HIGH_SPEED				3

#define USBH_STATE_DISCONNECTED		0x01
#define USBH_STATE_CONNECTED		0x02
#define USBH_STATE_OVERCURRENT		0x04
#define USBH_STATE_CHANGED			0x08
#define USBH_STATE_INVALID			0x80

#define USBH_TIMEOUT_NODATA			50		/* 50 ms timeout for no stage data transaction */
#define USBH_TIMEOUT_IN				500		/* 500 ms timeout for IN transaction */
#define USBH_TIMEOUT_OUT			5000	/* 5000 ms timeout for OUT transaction */
#define USBH_TIMEOUT_BULK			5000	/* 5000 ms timeout for bulk transaction */
#define USBH_ISO_IMMEDIATE			1
#define USBH_WAIT_FOREVER			((hcc_u32)-1)

#define USBH_SIZE_DEFAULT			((hcc_u32)-1)

#define USBH_MODE_BLOCK				0
#define USBH_MODE_NON_BLOCK			1

#define USBH_INVALID				-1

/* toggle values */
#define USBH_TOGGLE_PRESERVE		0xff
#define USBH_TOGGLE_0				0
#define USBH_TOGGLE_1				1

/* channel types */
#define USBH_TYPE_CONTROL			0
#define USBH_TYPE_ISO				1
#define USBH_TYPE_BULK				2
#define USBH_TYPE_INT				3

/* directions */
#define USBH_DIR_ANY				0
#define USBH_DIR_SETUP				1
#define USBH_DIR_OUT				2
#define USBH_DIR_IN					3

//Jason
#define DealyTimeOut			0
#define DealyTimeOK		1

/* return codes */
enum {
/*  0 */ USBH_SUCCESS,
/*  1 */ USBH_SHORT_PACKET,
/*  2 */ USBH_ERR_ENDPOINT_FULL,
/*  3 */ USBH_ERR_TRANSFER_FULL,
/*  4 */ USBH_ERR_TYPE,
/*  5 */ USBH_ERR_DIR,
/*  6 */ USBH_ERR_TIMEOUT,
/*  7 */ USBH_ERR_TRANSFER,
/*  8 */ USBH_ERR_RESOURCE,
/*  9 */ USBH_ERR_PERIODIC_LIST,
/* 10 */ USBH_ERR_ONGOING_TRANSFER,
/* 11 */ USBH_ERR_NOT_COMPLETE,
/* 12 */ USBH_ERR_NOT_AVAILABLE,
/* 13 */ USBH_ERR_HOST,
/* 14 */ USBH_ERR_INVALID,
/* 15 */ USBH_ERR_SUSPENDED,
/* 16 */ USBH_ERR_ISO_PSIZE
};


typedef void t_usbh_transfer_cb_fn (void *, int);
struct _t_usbh_transfer {
  /***** user variables *****/
  void *eph;			/* handle */
  hcc_u8 dir;			/* direction of the transfer (just for control EP) */
  hcc_u8 toggle;		/* toggle value */

  hcc_u8 *buf;			/* buffer */
  hcc_u32 size;			/* size of transfer */

  hcc_u8 iso_pcnt;		/* number of ISO packets to send using iso_psize */
  hcc_u16 iso_psize;	/* ISO packet size */
  hcc_u16 *iso_csize;	/* ISO completed sizes */

  t_usbh_transfer_cb_fn *cb_fn;/* callback function */
  void *cb_param;		/* callback parameter */
  int rc;				/* return code for callback function */

  hcc_u32 timeout;		/* timeout / ISO start frame */
  hcc_u32 csize;		/* completed size */

  /***** driver specific area *****/
  hcc_u32 driver_area[8];
};
typedef struct _t_usbh_transfer t_usbh_transfer;

int usbh_init (void);
int usbh_start (void);
int usbh_stop (void);
int usbh_receive_control_b (void *eph, hcc_u8 *setup_data, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength);
int usbh_send_control_b (void *eph, hcc_u8 *setup_data, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength);
int usbh_receive_b (void *eph, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength, hcc_u32 timeout);
int usbh_send_b (void *eph, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength, hcc_u32 timeout);

#include "usb_host_def.h"
#define usbh_transfer				_usbhd_transfer
#define usbh_transfer_status		_usbhd_transfer_status
#define usbh_get_ep_inf				_usbhd_get_ep_inf
#define usbh_delay					_usbhd_delay
#define usbh_transfer_task			_usbhd_transfer_task

#include "pmgr.h"
#define usbh_get_dev_state			pmgr_get_dev_state
#define usbh_suspend				pmgr_suspend
#define usbh_resume					pmgr_resume
#define usbh_get_port_number		pmgr_get_port_number
#define usbh_get_ifc_port_number	pmgr_get_ifc_port_number

#include "usb_utils.h"
#define usbh_set_alt_ifc		set_alt_ifc

#ifdef __cplusplus
}
#endif

#endif

