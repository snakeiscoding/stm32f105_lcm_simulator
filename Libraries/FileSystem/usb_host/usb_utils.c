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
#include "usb_utils.h"


/* Descriptors will be read to this buffer. */
static hcc_u8 dbuf[DBUFFER_SIZE];


/*****************************************************************************
 * fill_setup_packet
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Create data for a setup packet.
 *****************************************************************************/
void fill_setup_packet(hcc_u8* dst, hcc_u8 dir, hcc_u8 type, hcc_u8 recipient,
                       hcc_u8 req, hcc_u16 val, hcc_u16 ndx, hcc_u16 len)
{
  dst[0]=(hcc_u8)(dir|type|recipient);
  dst[1]=req;
  WR_LE16(dst+2,val)
    ;
  WR_LE16(dst+4,ndx)
    ;
  WR_LE16(dst+6,len)
    ;
}


/*****************************************************************************
 * clear_stall
 *****************************************************************************/
int clear_stall (void *ep0_hdl, void *ep_hdl)
{
  hcc_u8 setup[8];
  hcc_u8 ep,dir;

  if (_usbhd_get_ep_inf(ep_hdl,NULL,&ep,NULL,&dir,NULL)==USBH_SUCCESS)
  {
    if (dir==USBH_DIR_IN) ep|=0x80;
    fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_ENDPT
                      ,STDRQ_CLEAR_FEATURE, 0, ep, 0);
    if (usbh_send_control_b(ep0_hdl,setup,NULL,0,NULL)==USBH_SUCCESS)
    {
      if (_usbhd_clear_toggle(ep_hdl)==USBH_SUCCESS)
      {
        return 0;
      }
    }
  }
  return 1;
}


/*****************************************************************************
 * get_dev_desc
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Read device desriptor to dbuf.
 *****************************************************************************/
int get_dev_info (void *ep0_hdl, device_info_t *res)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;

  fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
                    STDRQ_GET_DESCRIPTOR, (STDDTYPE_DEVICE<<8)|0, 0, 18);

  while (retry--)
  {
    if (usbh_receive_control_b(ep0_hdl,setup,dbuf,18,NULL)==USBH_SUCCESS)
    {
      /* Check returned descriptor type and length (ignore extra bytes). */
      if ((USBDSC_TYPE(dbuf)==STDDTYPE_DEVICE) && (USBDSC_LENGTH(dbuf)==18))
      {
        if (res)
        {
          /* give read values to caller */
          res->_class=DEVDESC_CLASS(dbuf);
          res->sclass=DEVDESC_SCLASS(dbuf);
          res->protocol=DEVDESC_PROTOCOL(dbuf);
          res->rev=DEVDESC_REV(dbuf);
          res->vid=DEVDESC_VID(dbuf);
          res->pid=DEVDESC_PID(dbuf);
          res->ncfg=DEVDESC_NCFG(dbuf);
        }
        return 0;
      }
    }
  }
  return 1;
}


/*****************************************************************************
 * get_cfg_desc
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Read device desriptor to dbuf.
 *****************************************************************************/
int get_cfg_desc(void *ep0_hdl, hcc_u8 ndx)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;
  hcc_u32 length;
  int rc;

  while (retry--)
  {
    fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
                      STDRQ_GET_DESCRIPTOR, (hcc_u16)((STDDTYPE_CONFIGURATION<<8)|ndx),
                      0, DBUFFER_SIZE);

    rc=usbh_receive_control_b(ep0_hdl,setup,dbuf,DBUFFER_SIZE,&length);
    if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)
    {
	   if (RD_LE16(dbuf+2)<=length) return 0;
      break;
    }
  }
  return 1;
}


/*****************************************************************************
 * set_config
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Set device configuration
 *****************************************************************************/
int set_config(void *ep0_hdl, hcc_u8 cfg)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;

  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
                   STDRQ_SET_CONFIGURATION, cfg, 0, 0);
  while (retry--)
  {
   if (usbh_send_control_b(ep0_hdl,setup,NULL,0,NULL)==USBH_SUCCESS)
   {
     return 0;
   }
  }
  return 1;
}


/*****************************************************************************
 * set_address
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Set device address
 *****************************************************************************/
int set_address (void *ep0_hdl, USBH_DEV_ADDR daddr)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;

  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
                   STDRQ_SET_ADDRESS, daddr, 0, 0);

  while (retry--)
  {
   if (usbh_send_control_b(ep0_hdl,setup,NULL,0,NULL)==USBH_SUCCESS)
   {
     return 0;
   }
  }
  return 1;
}

/*****************************************************************************
 * set_alt_ifc
 *****************************************************************************/
int set_alt_ifc (void *ep0_hdl, hcc_u8 ifc, hcc_u8 alt)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;

  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_IFC,
                   STDRQ_SET_INTERFACE, alt, ifc, 0);

  while (retry--)
  {
   if (usbh_send_control_b(ep0_hdl,setup,NULL,0,NULL)==USBH_SUCCESS)
   {
     return 0;
   }
  }
  return 1;
}

/*****************************************************************************
 * get_ep0_max_packet_size
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Read device desriptor to dbuf.
 *****************************************************************************/
int get_ep0_max_packet_size (void *ep0_hdl, hcc_u8 *psize)
{
  hcc_u8 setup[8];

  fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
                    STDRQ_GET_DESCRIPTOR, (STDDTYPE_DEVICE<<8)|0, 0, 8);

  /* no retry needed, done from port manager combined with reset */
  if (usbh_receive_control_b(ep0_hdl,setup,dbuf,8,NULL)==USBH_SUCCESS)
  {
    /* Check returned descriptor type and length (ignore extra bytes). */
    if ((USBDSC_TYPE(dbuf)==STDDTYPE_DEVICE) && (USBDSC_LENGTH(dbuf)<=18))
    {
      *psize=dbuf[7];
      return 0;
    }
  }

  return 1;
}


/*****************************************************************************
 * get_next_descriptor
 * IN: ptr - current descriptor pointer
 * OUT: next descriptor pointer
 * Assumptions: n/a
 * Description:
 *   Get configuration info
 *****************************************************************************/
hcc_u8 *get_next_descriptor (hcc_u8 *ptr)
{
  if (ptr==NULL) ptr=dbuf;
  if ((ptr+*ptr)<dbuf+CONFDESC_TOTLENGTH(dbuf)) ptr+=*ptr;
                                           else ptr=NULL;
  return ptr;
}

/*****************************************************************************
 * get_cfg_info
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get configuration info
 *****************************************************************************/
void get_cfg_info(cfg_info_t *res)
{
  res->nifc=CONFDESC_INTRFACES(dbuf);
  res->ndx=CONFDESC_MY_NDX(dbuf);
  res->str=CONFDESC_MY_STR(dbuf);
  res->attrib=CONFDESC_ATTRIB(dbuf);
  res->max_power=CONFDESC_MAX_POW(dbuf);
}


/*****************************************************************************
 * get_ifc_info
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get interface info
 *****************************************************************************/
void get_ifc_info(ifc_info_t *res, hcc_u8 *dsc)
{
  res->_class=IFCDESC_CLASS(dsc);
  res->sclass=IFCDESC_SCLASS(dsc);
  res->protocol=IFCDESC_PROTOCOL(dsc);
  res->ndx=IFCDESC_MY_NDX(dsc);
  res->alt_set=IFCDESC_ALTERNATE(dsc);
  res->str=IFCDESC_MY_STR(dsc);
  res->nep=IFCDESC_ENDPONTS(dsc);
}


/*****************************************************************************
 * get_ep_info
 * IN:
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get endpoint info
 *****************************************************************************/
void get_ep_info(ep_info_t *res, hcc_u8 *dsc)
{
  res->address=EPDESC_ADDRESS(dsc);
  res->type=EPDESC_ATTRIB(dsc);
  res->interval=EPDESC_INTERVAL(dsc);
  res->psize=EPDESC_PSIZE(dsc);
}



