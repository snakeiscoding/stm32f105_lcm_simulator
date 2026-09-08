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
 * Budapest 1132
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _USB_UTILS_H
#define _USB_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_host.h"
#include "os.h"


#define DBUFFER_SIZE    511u

/* Standard descriptor types */
#define STDDTYPE_DEVICE        1u
#define STDDTYPE_CONFIGURATION 2u
#define STDDTYPE_STRING        3u
#define STDDTYPE_INTERFACE     4u
#define STDDTYPE_ENDPOINT      5u

/* Setup packet fields. */
#define STP_DIR_IN            (1u<<7)
#define STP_DIR_OUT           (0u<<7)
#define STP_TYPE_STD          (0u<<5)
#define STP_TYPE_CLASS        (1u<<5)
#define STP_TYPE_VENDOR       (2u<<5)
#define STP_RECIPIENT_DEVICE  (0<<0)
#define STP_RECIPIENT_IFC     (1<<0)
#define STP_RECIPIENT_ENDPT   (2<<0)
#define STP_RECIPIENT_OTHER   (3<<0)

/* Standard request values */
#define STDRQ_GET_STATUS         0u
#define STDRQ_CLEAR_FEATURE      1u
#define STDRQ_SET_FEATURE        3u
#define STDRQ_SET_ADDRESS        5u
#define STDRQ_GET_DESCRIPTOR     6u
#define STDRQ_SET_DESCRIPTOR     7u
#define STDRQ_GET_CONFIGURATION  8u
#define STDRQ_SET_CONFIGURATION  9u
#define STDRQ_GET_INTERFACE      10u
#define STDRQ_SET_INTERFACE      11u
#define STDRQ_SYNCH_FRAME        12u

/* Access common properties of USB descriptors. */
#define USBDSC_LENGTH(p)       (p[0])
#define USBDSC_TYPE(p)         (p[1])

/* Read device descriptor members. */
#define DEVDESC_BCDUSB(p)      RD_LE16(p+2)
#define DEVDESC_CLASS(p)       (p[4])
#define DEVDESC_SCLASS(p)      (p[5])
#define DEVDESC_PROTOCOL(p)    (p[6])
#define DEVDESC_PACKET_SIZE(p) (p[7])
#define DEVDESC_VID(p)         RD_LE16(p+8)
#define DEVDESC_PID(p)         RD_LE16(p+10)
#define DEVDESC_REV(p)         RD_LE16(p+12)
#define DEVDESC_MANUFACT(p)    (p[14))
#define DEVDESC_PRODUCT(p)     (p[15])
#define DEVDESC_SERNO(p)       (p[16])
#define DEVDESC_NCFG(p)        (p[17])

/* Read configuration descriptor members. */
#define CONFDESC_TOTLENGTH(p)  RD_LE16(p+2)
#define CONFDESC_INTRFACES(p)  (p[4])
#define CONFDESC_MY_NDX(p)     (p[5])
#define CONFDESC_MY_STR(p)     (p[6])
#define CONFDESC_ATTRIB(p)     (p[7])
#define CONFDESC_MAX_POW(p)    (p[8])

/* Read interface descriptor members. */
#define IFCDESC_MY_NDX(p)      (p[2])
#define IFCDESC_ALTERNATE(p)   (p[3])
#define IFCDESC_ENDPONTS(p)    (p[4])
#define IFCDESC_CLASS(p)       (p[5])
#define IFCDESC_SCLASS(p)      (p[6])
#define IFCDESC_PROTOCOL(p)    (p[7])
#define IFCDESC_MY_STR(p)      (p[8])

#define EPDESC_ADDRESS(p)      (p[2])
#define EPDESC_ATTRIB(p)       (p[3])
#define EPDESC_PSIZE(p)        RD_LE16(p+4)
#define EPDESC_INTERVAL(p)     (p[6])


/* Driver returns information extracted from device descriptors using this
   type. */
typedef struct {
  hcc_u16 vid;
  hcc_u16 pid;
  hcc_u16 rev;
  hcc_u8  _class;
  hcc_u8  sclass;
  hcc_u8  protocol;
  hcc_u8  ncfg;
} device_info_t;

/* Driver returns information extracted from interface descriptors using this
   type. */
typedef struct {
  hcc_u8 _class;
  hcc_u8 sclass;
  hcc_u8 protocol;
  hcc_u8 ndx;
  hcc_u8 alt_set;
  hcc_u8 str;
  hcc_u8 nep;
} ifc_info_t;

/* Driver returns information extracted from configuration descriptors using
   this type. */
typedef struct {
  hcc_u8 nifc;
  hcc_u8 ndx;
  hcc_u8 str;
  hcc_u8 attrib;
  hcc_u8 max_power;
} cfg_info_t;

/* Driver returns information extracted from endpoint descriptors using this
   type. */
typedef struct {
  hcc_u16 psize;
  hcc_u8 address;
  hcc_u8 type;
  hcc_u8 interval;
} ep_info_t;

void fill_setup_packet(hcc_u8* dst, hcc_u8 dir, hcc_u8 type, hcc_u8 recipient,
                       hcc_u8 req, hcc_u16 val, hcc_u16 ndx, hcc_u16 len);

int clear_stall (void *ep0_hdl, void *ep_hdl);
int get_dev_info (void *ep0_hdl, device_info_t *res);
int get_cfg_desc(void *ep0_hdl, hcc_u8 ndx);
int set_config(void *ep0_hdl, hcc_u8 cfg);
int set_address (void *ep0_hdl, USBH_DEV_ADDR daddr);
int set_alt_ifc(void *ep0_hdl, hcc_u8 ifc, hcc_u8 alt);
int get_ep0_max_packet_size (void *ep0_hdl, hcc_u8 *psize);

hcc_u8 *get_next_descriptor (hcc_u8 *ptr);
void get_cfg_info(cfg_info_t *res);
void get_ifc_info(ifc_info_t *res, hcc_u8 *dsc);
void get_ep_info(ep_info_t *res, hcc_u8 *dsc);

#ifdef __cplusplus
}
#endif


#endif



