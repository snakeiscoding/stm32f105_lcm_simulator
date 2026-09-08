/***************************************************************************
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
#include "hcc_types.h"
#include "usb_host.h"
#include "usb_host_def.h"


/*
** Receive control packet
** INPUT: eph - endpoint handle
**        setup_data - pointer to setup data
**        buffer - pointer to store incoming data
**        length - length of requested data
** OUTPUT: rlength - received size
** RETURN: USBH_....
*/
int usbh_receive_control_b (void *eph, hcc_u8 *setup_data, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength)
{
  t_usbh_transfer uht;
  int rc;

  /* setup transaction. */
  uht.eph=eph;
  uht.dir=USBH_DIR_SETUP;
  uht.toggle=USBH_TOGGLE_0;
  uht.buf=setup_data;
  uht.size=8;
  uht.timeout=USBH_TIMEOUT_NODATA;
  uht.cb_fn=NULL;
  rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
  if (length && rc==USBH_SUCCESS)
  {
    /* data transactions */
    uht.dir=USBH_DIR_IN;
    uht.toggle=USBH_TOGGLE_1;
    uht.buf=buffer;
    uht.size=length;
    uht.timeout=USBH_TIMEOUT_IN;
    rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
    if (rlength) *rlength=uht.csize;
  }
  /* status change */
  if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)
  {
    uht.dir=USBH_DIR_OUT;
    uht.toggle=USBH_TOGGLE_1;
    uht.buf=NULL;
    uht.size=0;
    uht.timeout=USBH_TIMEOUT_NODATA;
    rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
  }
  return rc;
}


/*
** Send control packet
** INPUT: eph - endpoint handle
**        setup_data - pointer to setup data
**        buffer - buffer to send
**        length - length of data to send
** OUTPUT: rlength - sent size
** RETURN: USBH_....
*/
int usbh_send_control_b (void *eph, hcc_u8 *setup_data, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength)
{
  t_usbh_transfer uht;
  int rc;

  /* setup transaction. */
  uht.eph=eph;
  uht.dir=USBH_DIR_SETUP;
  uht.toggle=USBH_TOGGLE_0;
  uht.buf=setup_data;
  uht.size=8;
  uht.timeout=USBH_TIMEOUT_NODATA;
  uht.cb_fn=NULL;
  rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
  if (length && rc==USBH_SUCCESS)
  {
    /* data transactions */
    uht.dir=USBH_DIR_OUT;
    uht.toggle=USBH_TOGGLE_1;
    uht.buf=buffer;
    uht.size=length;
    uht.timeout=USBH_TIMEOUT_OUT;
    rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
    if (rlength) *rlength=uht.csize;
  }
  /* status change */
  if (rc==USBH_SUCCESS)
  {
    uht.dir=USBH_DIR_IN;
    uht.toggle=USBH_TOGGLE_1;
    uht.buf=NULL;
    uht.size=0;
    uht.timeout=USBH_TIMEOUT_NODATA;
    rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
  }
  return rc;
}


/*
** Receive general packet
** INPUT: eph - endpoint handle
**        buffer - pointer to store incoming data
**        length - length of requested data
**        timeout - USBH_WAIT_FOREVER/n-number of frames to wait
** OUTPUT: rlength - received size
** RETURN: USBH_....
*/
int usbh_receive_b (void *eph, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength, hcc_u32 timeout)
{
  t_usbh_transfer uht;
  int rc;

  /* setup transaction. */
  uht.eph=eph;
  uht.dir=USBH_DIR_IN;
  uht.toggle=USBH_TOGGLE_PRESERVE;
  uht.buf=buffer;
  uht.size=length;
  uht.timeout=timeout;
  uht.cb_fn=NULL;
  rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
  if (rlength) *rlength=uht.csize;
  return rc;
}



/*
** Send general packet
** INPUT: eph - endpoint handle
**        buffer - buffer to send
**        length - length of data to send
**        timeout - USBH_WAIT_FOREVER/n-number of frames to wait
** OUTPUT: rlength - sent size
** RETURN: USBH_....
*/
int usbh_send_b (void *eph, hcc_u8 *buffer, hcc_u32 length, hcc_u32 *rlength, hcc_u32 timeout)
{
  t_usbh_transfer uht;
  int rc;

  uht.eph=eph;
  uht.dir=USBH_DIR_OUT;
  uht.toggle=USBH_TOGGLE_PRESERVE;
  uht.buf=buffer;
  uht.size=length;
  uht.timeout=timeout;
  uht.cb_fn=NULL;
  rc=_usbhd_transfer(&uht,USBH_MODE_BLOCK);
  if (rlength) *rlength=uht.csize;
  return rc;
}


/*
** Init USB host stack
*/
int usbh_init (void)
{
  int rc;

  rc=pmgr_init();
  if (rc) return rc;

  rc=_usbhd_init();
  if (rc) return rc;

  return USBH_SUCCESS;
}


/*
** Start USB host stack
*/
int usbh_start (void)
{
  int rc;

  rc=pmgr_start();
  if (rc) return rc;

  rc=_usbhd_start(0);
  if (rc) return rc;

  return USBH_SUCCESS;
}


/*
** Stop USB host stack
*/
int usbh_stop (void)
{
  int rc=USBH_SUCCESS;

  if (pmgr_stop()!=USBH_SUCCESS) rc=USBH_ERR_HOST;
  if (_usbhd_stop()!=USBH_SUCCESS) rc=USBH_ERR_HOST;

  return rc;
}



