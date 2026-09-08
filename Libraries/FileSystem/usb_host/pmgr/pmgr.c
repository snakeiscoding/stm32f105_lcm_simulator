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
#include "hcc_types.h"
#include "usb_host.h"
#include "usb_host_def.h"
#include "usb_utils.h"
#include "pmgr.h"
#include "pmgr_def.h"


#define PORT_DSC_MASK_SHIFT		6
#define IFC_DSC_MASK_SHIFT		4
#define MOUNT_COUNT_MASK_SHIFT	(PORT_DSC_MASK_SHIFT+IFC_DSC_MASK_SHIFT)

#define PMGR_SUSPEND			0
#define PMGR_RESUME				1


/*
** HUB descriptor
*/
typedef struct {
  hub_scan_fn scan;				/* scan for device status change */
  hub_fn reset;					/* reset HUB port */
  hub_fn disable;				/* disable HUB port */
  hub_fn suspend;				/* suspend HUB port */
  hub_fn resume;				/* resume HUB port */

  void *ifc_hdl;				/* interface handler */
} t_hub_dsc;

/*
** CD descriptor
*/
typedef struct {
  cd_fn_check check;			/* check callback function */
  cd_fn_connect connect;		/* connect callback function */
  cd_fn_disconnect disconnect;	/* disconnect callback function */
  cd_fn_descriptor descriptor;	/* unknown descriptor callback function */
  t_cd_ep_dsc *ep_dsc;			/* class drivers endpoint descriptor */
} t_cd_dsc;


/*
** Mounted interface descriptor
*/
typedef struct {
  hcc_u8 state;					/* state */
  t_cd_dsc *cd_dsc;				/* class driver descriptor */
  void *dev_hdl;				/* class driver specific device handler */
} t_ifc_dsc;


/*
** Connected port descriptor
*/
typedef struct {
  t_hub_dsc *hub_dsc;			/* HUB descriptor port is attached */
  hcc_u8 port;					/* HUB driver specific port identifier number */
  hcc_u32 mount_count;			/* mount counter */

  hcc_u8 speed;					/* speed of the port */
  USBH_DEV_ADDR daddr;			/* device address assigned */

  t_ifc_dsc ifc_dsc[MAX_INTERFACE_PER_PORT];	/* interface descriptors */
} t_port_dsc;



static t_hub_scan_inf hub_scan_inf[MAX_PORTS];	/* HUB scan information */
static t_hub_dsc hub_dsc[MAX_HUBS];				/* HUB descriptors */

static t_cd_dsc cd_dsc[MAX_CLASS_DRIVERS];		/* installed class drivers */

static t_port_dsc port_dsc[MAX_PORTS];			/* HUB port descriptors */
static t_port_dsc *daddr2port_dsc[MAX_PORTS];	/* address to hub port conversion table */


OS_EVENT_BIT_TYPE pmgr_event_bit;				/* HUB state change event */


/*
** Encode interface handler.
** INPUT: p_port_dsc - pointer to port descriptor
**        p_ifc_dsc - pointer to interface descriptor
** RETURN: interface handler
*/
static void *_pmgr_encode_ifc_hdl (t_port_dsc *p_port_dsc, t_ifc_dsc *p_ifc_dsc)
{
  hcc_u32 rc;

  if ((p_port_dsc->mount_count<<MOUNT_COUNT_MASK_SHIFT)==0) p_port_dsc->mount_count=1;
  rc=p_port_dsc->mount_count;
  rc<<=IFC_DSC_MASK_SHIFT;
  rc|=((hcc_u32)(p_ifc_dsc-(p_port_dsc->ifc_dsc)))&((1<<IFC_DSC_MASK_SHIFT)-1);
  rc<<=PORT_DSC_MASK_SHIFT;
  rc|=((hcc_u32)(p_port_dsc-port_dsc))&((1<<PORT_DSC_MASK_SHIFT)-1);

  return (void *)rc;
}


/*
** Decode interface handler and check whether the handler is still valid
** INPUT: ifc_hdl - interface handler
** OUTPUT: p_port_dsc - pointer to port descriptor
**         p_ifc_dsc - pointer to interface descriptor
** RETURN: USBH_SUCCESS - valid handler / USBH_ERR_INVALID - invalid handler
*/
static int _pmgr_decode_ifc_hdl (void *ifc_hdl, t_port_dsc **p_port_dsc, t_ifc_dsc **p_ifc_dsc)
{
  int rc=USBH_SUCCESS;
  hcc_u32 tmp=(hcc_u32)ifc_hdl;

  *p_port_dsc=port_dsc+(tmp&((1<<PORT_DSC_MASK_SHIFT)-1));
  if (*p_port_dsc>=port_dsc && *p_port_dsc<(port_dsc+MAX_PORTS))
  {
    tmp>>=PORT_DSC_MASK_SHIFT;
    *p_ifc_dsc=((*p_port_dsc)->ifc_dsc)+(tmp&((1<<IFC_DSC_MASK_SHIFT)-1));
    tmp>>=IFC_DSC_MASK_SHIFT;
    if (tmp!=(*p_port_dsc)->mount_count) rc=USBH_ERR_INVALID;
  }
  else rc=USBH_ERR_INVALID;

  return rc;
}


/*
** Suspend/Resume a device
** INPUT: ifc_hdl - interface handler
** RETUEN: USBH_...
*/
static int _pmgr_suspend_resume (void *ifc_hdl, hcc_u8 cmd)
{
  if (ifc_hdl)	/* suspend only one port (associated to this interface) */
  {
    t_port_dsc *p_port_dsc;
    t_ifc_dsc *p_ifc_dsc;

    if (_pmgr_decode_ifc_hdl(ifc_hdl,&p_port_dsc,&p_ifc_dsc)==USBH_SUCCESS)	/* decode interface handler */
    {
      if ((p_ifc_dsc->state)&USBH_STATE_CONNECTED)							/* check connected state */
      {
        hub_fn fn;
        if (cmd==PMGR_SUSPEND) fn=p_port_dsc->hub_dsc->suspend;
                          else fn=p_port_dsc->hub_dsc->resume;

        if (fn!=NULL)														/* check if function available */
        {
          void *hub_ifc_hdl=p_port_dsc->hub_dsc->ifc_hdl;
          if ( hub_ifc_hdl==NULL											/* decode HUB interface handler */
               || _pmgr_decode_ifc_hdl(hub_ifc_hdl,&p_port_dsc,&p_ifc_dsc)==USBH_SUCCESS )
          {
            return fn(hub_ifc_hdl?p_ifc_dsc->dev_hdl:NULL,p_port_dsc->port);
          }
        }
      }
    }
  }
  else			/* suspend whole host controller */
  {
    hcc_uint i;
    for (i=0;i<MAX_HUBS;i++)		/* go through all available HUB drivers */
    {
      if (hub_dsc[i].scan!=NULL)
      {
        if (hub_dsc[i].ifc_hdl==NULL)
        {
          hub_fn fn;

          if (cmd==PMGR_SUSPEND) fn=hub_dsc[i].suspend;
                            else fn=hub_dsc[i].resume;

          if (fn!=NULL)
          {
            return fn(NULL,(hcc_u8)-1);
          }
        }
      }
    }
  }

  return USBH_ERR_NOT_AVAILABLE;
}



/*
** Disconnect all ports belonging to a HUB.
*/
static int _pmgr_remove_hub_ports (void *hub_hdl)
{
  int rc=USBH_SUCCESS;
  hcc_uint i,j;
  t_hub_dsc *p_hub_dsc=(t_hub_dsc *)hub_hdl;
  t_port_dsc *p_hub_port_dsc;
  t_ifc_dsc *p_hub_ifc_dsc;

  if ( p_hub_dsc->ifc_hdl==NULL
       || _pmgr_decode_ifc_hdl(p_hub_dsc->ifc_hdl,&p_hub_port_dsc,&p_hub_ifc_dsc)==USBH_SUCCESS )
  {
    /* check if any of the currently connected devices were attached to this HUB */
    for (j=0;j<MAX_PORTS;j++)
    {
      if (port_dsc[j].hub_dsc==p_hub_dsc)								/* device was attached to this HUB */
      {
        for (i=0;i<MAX_INTERFACE_PER_PORT;i++)
        {
          if (port_dsc[j].ifc_dsc[i].cd_dsc)
          {
            if (port_dsc[j].ifc_dsc[i].cd_dsc->disconnect!=NULL)
            {
              (void)(port_dsc[j].ifc_dsc[i].cd_dsc->disconnect)(port_dsc[j].ifc_dsc[i].dev_hdl);
            }
            port_dsc[j].ifc_dsc[i].dev_hdl=NULL;
          }
          port_dsc[j].ifc_dsc[i].state=USBH_STATE_DISCONNECTED;
        }
        if ((_usbhd_del_daddr)(port_dsc[j].daddr)) rc=USBH_ERR_HOST;	/* delete address from host driver */
        daddr2port_dsc[port_dsc[j].daddr-1]=NULL;
        port_dsc[j].hub_dsc=NULL;

        if (p_hub_dsc->ifc_hdl==NULL)
        {
          (void)(p_hub_dsc->disable)(p_hub_dsc->ifc_hdl?p_hub_ifc_dsc->dev_hdl:NULL,port_dsc[j].port);	/* disable port */
        }
      }
    }
  }

  return rc;
}


/*
** Connect current
*/
static int _pmgr_connect (void *ep0_hdl, t_port_dsc *p_port_dsc, t_cd_dsc *p_cd_dsc, int ifc_pos, hcc_u8 *sc)
{
  void *ifc_hdl;
  int rc=USBH_ERR_NOT_AVAILABLE;

  if (p_port_dsc->ifc_dsc[ifc_pos].dev_hdl)		/* previous class driver to be validated */
  {
    if (*sc==0)
    {
      cfg_info_t cfg_info;
      get_cfg_info(&cfg_info);
      set_config(ep0_hdl,cfg_info.ndx);			/* set configuration only if it wasn't done yet */
      *sc=1;
    }

    ifc_hdl=_pmgr_encode_ifc_hdl(p_port_dsc,&p_port_dsc->ifc_dsc[ifc_pos]);
    if ((p_cd_dsc->connect)(p_port_dsc->ifc_dsc[ifc_pos].dev_hdl,ifc_hdl)==USBH_SUCCESS)
    {
      p_port_dsc->ifc_dsc[ifc_pos].cd_dsc=p_cd_dsc;
      rc=USBH_SUCCESS;
    }
    else
    {
      p_port_dsc->ifc_dsc[ifc_pos].dev_hdl=NULL;/* invalidate current device handler */
    }
  }

  return rc;
}


/*
** _pmgr_search_ifc
** Search interface for the current device.
**
** Input: ep0_hdl - endpoint0 handler
**        daddr - device address
** Return: number of mounted interfaces
*/
static int _pmgr_search_ifc (void *ep0_hdl, t_port_dsc *p_port_dsc)
{
  device_info_t dev_inf;
  hcc_u8 cfg;
  hcc_uint i;
  int ifc_cnt=0,err=0;

  /* see what device is this */
  if (get_dev_info(ep0_hdl,&dev_inf)==0)
  {
    /* go through all configurations and find for valid class drivers while no interfaces
       were mounted */
    for (cfg=0;cfg<dev_inf.ncfg && ifc_cnt==0 && err==0;cfg++)
    {
      hcc_u8 *dsc=NULL;			/* current descriptor pointer */
      ifc_info_t ifc_info;		/* interface information */
      hcc_u8 cd=0;				/* go through class drivers */
      void **p_ep_hdl=NULL;		/* pointer to endpoint handles */
      t_cd_ep_dsc *ep_dsc;		/* required endpoint descriptor */
      hcc_u8 sc=0;


      if (get_cfg_desc(ep0_hdl,cfg)) continue;				/* get configuration descriptor */

      dsc=get_next_descriptor(dsc);
      while (dsc && ifc_cnt<MAX_INTERFACE_PER_PORT && err==0)
      {
        switch (USBDSC_TYPE(dsc))							/* descriptor type */
        {
          /* INTERFACE descriptor found */
          case STDDTYPE_INTERFACE:
              if (_pmgr_connect(ep0_hdl,p_port_dsc,cd_dsc+cd,ifc_cnt,&sc)==USBH_SUCCESS) ++ifc_cnt;

              if (ifc_cnt<MAX_INTERFACE_PER_PORT)
              {
                hcc_u8 _cd;
                get_ifc_info(&ifc_info,dsc);					/* get interface information */
                for (_cd=0;_cd<MAX_CLASS_DRIVERS;_cd++)
                {
                  if (cd_dsc[cd].connect!=NULL)
                  {
                    p_ep_hdl=(cd_dsc[cd].check)(&p_port_dsc->ifc_dsc[ifc_cnt].dev_hdl,p_port_dsc,&dev_inf,&ifc_info);
                    if (p_port_dsc->ifc_dsc[ifc_cnt].dev_hdl)	/* class driver found? */
                    {
                      if (p_ep_hdl)								/* endpoints required */
                      {
                        ep_dsc=cd_dsc[cd].ep_dsc+1;
                        for (i=1;i<*(cd_dsc[cd].ep_dsc)+1;i++)
                        {										/* set control endpoint and reset everything else */
                          if (USBH_EP_DSC_t(*ep_dsc)==USBH_TYPE_CONTROL) *(p_ep_hdl+i-1)=ep0_hdl;
                                                                    else *(p_ep_hdl+i-1)=NULL;
                          ++ep_dsc;
                        }
                      }
                      break;									/* valid class driver found */
                    }
                  }

                  if (++cd==MAX_CLASS_DRIVERS) cd=0;
                }
              }
              break;

          /* ENDPOINT descriptor found */
          case STDDTYPE_ENDPOINT:
              if (p_port_dsc->ifc_dsc[ifc_cnt].dev_hdl)		/* valid class driver for current interface */
              {
                if (p_ep_hdl)									/* endpoints required */
                {
                  ep_info_t ep_inf;
                  get_ep_info(&ep_inf,dsc);					/* get endpoint information */

                  ep_dsc=cd_dsc[cd].ep_dsc+1;					/* first endpoint type descriptor */
                  for (i=1;i<*(cd_dsc[cd].ep_dsc)+1;i++)		/* go through all required endpoints */
                  {
                    if (*(p_ep_hdl+i-1)==NULL)				/* current endpoint is still not added */
                    {
                      if ((ep_inf.type&3)==USBH_EP_DSC_t(*ep_dsc))
                      {										/* current type is required? */
                        if (  ((ep_inf.address>0x80) && (USBH_EP_DSC_d(*ep_dsc)==USBH_DIR_IN))
                            || ((ep_inf.address<0x80) && (USBH_EP_DSC_d(*ep_dsc)==USBH_DIR_OUT)) )
                        {									/* check if direction matches */
                          *(p_ep_hdl+i-1)=_usbhd_add_ep(p_port_dsc->daddr,(hcc_u8)(ep_inf.address&0x7f)
                                                        ,(hcc_u8)USBH_EP_DSC_t(*ep_dsc)
                                                        ,(hcc_u8)USBH_EP_DSC_d(*ep_dsc)
                                                        ,ep_inf.psize,ep_inf.interval);
                          if (*(p_ep_hdl+i-1)==NULL)
                          {
                            err=1;
                          }
                          break;							/* add the endpoint */
                        }
                      }
                    }
                    ++ep_dsc;
                  }
                }
              }
              break;

          /* unknown descriptor */
          default:
              if (p_port_dsc->ifc_dsc[ifc_cnt].dev_hdl)		/* valid class driver for current interface */
              {
                if (cd_dsc[cd].descriptor!=NULL)			/* unknown descriptor, call class driver descriptor */
                {											/* handler if present */
                  if ((cd_dsc[cd].descriptor)(p_port_dsc->ifc_dsc[ifc_cnt].dev_hdl,dsc)) err=1;
                }
              }
              break;
        }
        dsc=get_next_descriptor(dsc);
      }
      if (_pmgr_connect(ep0_hdl,p_port_dsc,cd_dsc+cd,ifc_cnt,&sc)==USBH_SUCCESS) ++ifc_cnt;
    }
  }

  return ifc_cnt;
}




/*
** port manager PCD task (port change detect)
** Scans all available HUB drivers for change.
*/
OS_TASK_FN(pmgr_pcd_task)
{
  hcc_uint i,j,k;
  int hsi_cnt;
  t_port_dsc *p_port_dsc;
  hcc_u8 ep0_psize;
  t_hub_dsc *p_hub_dsc;
  t_hub_scan_inf *hsi;

  t_port_dsc *p_hub_port_dsc=NULL;
  t_ifc_dsc *p_hub_ifc_dsc=NULL;


#if OS_TASK_POLL_MODE
  if (os_event_get(pmgr_event_bit)==OS_SUCCESS)
  {
#else
  for (;;)
  {
    if (os_event_get(pmgr_event_bit)!=OS_SUCCESS) continue;
#endif
    p_hub_dsc=hub_dsc;
    for (i=0;i<MAX_HUBS;i++,p_hub_dsc++)		/* go through all available HUB drivers */
    {
      if (p_hub_dsc->scan!=NULL)				/* scan for port changes in the current hub driver */
      {
        if ( p_hub_dsc->ifc_hdl==NULL
            || _pmgr_decode_ifc_hdl(p_hub_dsc->ifc_hdl,&p_hub_port_dsc,&p_hub_ifc_dsc)==USBH_SUCCESS )
        {
          hsi=hub_scan_inf;
          hsi_cnt=(p_hub_dsc->scan)(p_hub_dsc->ifc_hdl?p_hub_ifc_dsc->dev_hdl:NULL,hsi);
          for (j=0;j<hsi_cnt;j++,hsi++)			/* step through all new scan information */
          {										/* check if port already connected and empty entry */
            p_port_dsc=NULL;
            for (k=0;k<MAX_PORTS;k++)
            {
              if (port_dsc[k].hub_dsc==NULL && p_port_dsc==NULL)
              {
                p_port_dsc=port_dsc+k;			/* set first empty entry */
              }

              if (port_dsc[k].hub_dsc==p_hub_dsc && port_dsc[k].port==hsi->port)
              {
                p_port_dsc=port_dsc+k;			/* device was already connected */
                break;
              }
            }

            if (hsi->state&USBH_STATE_CONNECTED)				/* device was connected */
            {
              hcc_u8 state=USBH_STATE_CONNECTED;				/* set state to connected */

              ++p_port_dsc->mount_count;						/* increase mount counter */
              if (k==MAX_PORTS)
              {
                /* if port was not connected previously then fill port descriptor structure */
                if (p_port_dsc)
                {
                  p_port_dsc->hub_dsc=p_hub_dsc;				/* set HUB iface pointer */
                  p_port_dsc->port=hsi->port;					/* set port number */
                  for (k=0;k<MAX_PORTS;k++)
                  {
                    if (daddr2port_dsc[k]==0)
                    {
                      p_port_dsc->daddr=(hcc_u8)(k+1);			/* assign address */
                      break;
                    }
                  }
                  daddr2port_dsc[k]=p_port_dsc;
                }
                else break;										/* ERR_NOMOREENTRY */
              }
              else
              {
                for (k=0;k<MAX_INTERFACE_PER_PORT;k++)
                {
                  if (p_port_dsc->ifc_dsc[k].cd_dsc!=NULL)
                  {
                    if (p_port_dsc->ifc_dsc[k].cd_dsc->disconnect!=NULL)
                    {
                      (void)(p_port_dsc->ifc_dsc[k].cd_dsc->disconnect)(p_port_dsc->ifc_dsc[k].dev_hdl);
                    }
                    p_port_dsc->ifc_dsc[k].dev_hdl=NULL;
                  }
                }
                (void)_usbhd_del_daddr(p_port_dsc->daddr);		/* delete address from host driver */

              }

              state|=USBH_STATE_CHANGED;						/* set changed state if previously connected */
              for (k=0;k<MAX_INTERFACE_PER_PORT;k++)
              {
                p_port_dsc->ifc_dsc[k].state=state;
                p_port_dsc->ifc_dsc[k].cd_dsc=NULL;
              }

              /* reset port */
              p_port_dsc->speed=(hcc_u8)(p_hub_dsc->reset)(p_hub_dsc->ifc_hdl?p_hub_ifc_dsc->dev_hdl:NULL,hsi->port);

              /* Now, get EP0 max. packet size, add it to the host driver and assign the address */
              if (p_port_dsc->speed!=USBH_SPEED_ERROR)
              {
                if (_usbhd_set_ep0_speed_hub(NULL,p_port_dsc->speed,p_port_dsc->port,(hcc_u8)(p_hub_dsc->ifc_hdl?p_hub_port_dsc->daddr:0)
                                             ,(hcc_u8)(p_hub_dsc->ifc_hdl?p_hub_port_dsc->speed:0))==USBH_SUCCESS)
                {
                  unsigned int err=0;
                  unsigned int retry=3;
                  while (retry-- && err==0)
                  {
                    if (get_ep0_max_packet_size(NULL,&ep0_psize)==USBH_SUCCESS)	/* get device EP0 max. packet size */
                    {
                      if (set_address(NULL,p_port_dsc->daddr)==USBH_SUCCESS)	/* set new address */
                      {											/* and add new endpoint0 to host driver */
                        void *ep0_hdl;
                        _usbhd_delay(2);
                        ep0_hdl=(_usbhd_add_ep)(p_port_dsc->daddr,0,USBH_TYPE_CONTROL,USBH_DIR_ANY,ep0_psize,0);
                        if (ep0_hdl)
                        {										/* search class driver for this device */
                          if (_usbhd_set_ep0_speed_hub(ep0_hdl,p_port_dsc->speed,p_port_dsc->port,(hcc_u8)(p_hub_dsc->ifc_hdl?p_hub_port_dsc->daddr:0)
                                                       ,(hcc_u8)(p_hub_dsc->ifc_hdl?p_hub_port_dsc->speed:0))==USBH_SUCCESS)
                          {
                            (void)_pmgr_search_ifc(ep0_hdl,p_port_dsc);
                            break;
                          }
                          else err=1;							/* ERR_HOST */
                        }
                        else err=1;								/* ERR_HOST */
                      }
                      else err=1;								/* ERR_HOST */
                    }
                    else
                    {
                      (void)(p_hub_dsc->reset)(p_hub_dsc->ifc_hdl?p_hub_ifc_dsc->dev_hdl:NULL,hsi->port);	/* reset port */
                    }
                  }
                  if (err)
                  {
                    (void)_usbhd_del_daddr(p_port_dsc->daddr);	/* delete address from host driver */
                    continue;
                  }
                }
                else continue;									/* ERR_HOST */
              }
              else continue;									/* ERR_RESET */
            }
            else if (hsi->state&(USBH_STATE_DISCONNECTED|USBH_STATE_OVERCURRENT))
            {													/* device disconnected or overcurrent */
              if (k<MAX_PORTS)
              {
                for (k=0;k<MAX_INTERFACE_PER_PORT;k++)
                {
                  if (p_port_dsc->ifc_dsc[k].cd_dsc)
                  {
                    if (p_port_dsc->ifc_dsc[k].cd_dsc->disconnect!=NULL)
                    {													/* call disconnect function if needed */
                      (void)(p_port_dsc->ifc_dsc[k].cd_dsc->disconnect)(p_port_dsc->ifc_dsc[k].dev_hdl);
                    }
                    p_port_dsc->ifc_dsc[k].dev_hdl=NULL;
                  }
                  p_port_dsc->ifc_dsc[k].state=USBH_STATE_DISCONNECTED;	/* set disconnected state */
                }

                daddr2port_dsc[p_port_dsc->daddr-1]=NULL;		/* invalidate address */
                p_port_dsc->hub_dsc=NULL;						/* invalidate HUB port descriptor */
                (void)_usbhd_del_daddr(p_port_dsc->daddr);		/* delete address from host driver */
              }
              (void)(p_hub_dsc->disable)(p_hub_dsc->ifc_hdl?p_hub_ifc_dsc->dev_hdl:NULL,hsi->port);			/* disable port */
            }
          }
        }
      }
    }
  }
}


/*
** Returns the port number of an address
** INPUT: daddr - device address
** RETURN: port number
*/
hcc_u8 pmgr_get_port_number (USBH_DEV_ADDR daddr)
{
  return daddr2port_dsc[daddr-1]->port;
}


/*
** Returns the port number of an address
** INPUT: void *ifc_hdl
** OUTPUT: port - port number
** RETURN: error code
*/
int pmgr_get_ifc_port_number (void *ifc_hdl, hcc_u8 *port)
{
  t_port_dsc *p_port_dsc;
  t_ifc_dsc *p_ifc_dsc;

  if (_pmgr_decode_ifc_hdl(ifc_hdl,&p_port_dsc,&p_ifc_dsc)==USBH_SUCCESS)
  {
    if (port) *port=p_port_dsc->port;
    return USBH_SUCCESS;
  }
  return USBH_STATE_INVALID;
}


/*
** Get device state. Clears changed state
** INPUT: ifc_hdl - interface handler
** RETURN: state - disconnected/connected/changed or invalid
*/
hcc_u8 pmgr_get_dev_state (void *ifc_hdl)
{
  t_port_dsc *p_port_dsc;
  t_ifc_dsc *p_ifc_dsc;

  if (_pmgr_decode_ifc_hdl(ifc_hdl,&p_port_dsc,&p_ifc_dsc)==USBH_SUCCESS)
  {
    if ((p_ifc_dsc->state)&USBH_STATE_CONNECTED)
    {
      hcc_u8 rc=p_ifc_dsc->state;
      p_ifc_dsc->state&=~USBH_STATE_CHANGED;
      return rc;
    }
  }
  return USBH_STATE_INVALID;
}


/*
** Suspend a device
** INPUT: ifc_hdl - interface handler
** RETUEN: USBH_...
*/
int pmgr_suspend (void *ifc_hdl)
{
  return _pmgr_suspend_resume(ifc_hdl,PMGR_SUSPEND);
}



/*
** Resume a device
** INPUT: port_hdl - port handler
** RETUEN: USBH_...
*/
int pmgr_resume (void *ifc_hdl)
{
  return _pmgr_suspend_resume(ifc_hdl,PMGR_RESUME);
}


/*
** Install new class driver
** Input: connect - search interface function of the class driver
**        disconnect - disconnect function to call
**        id - class driver identification structure
** Return: USBH_SUCCESS / USBH_ERR_NOT_AVAILABLE
*/
int pmgr_add_cd (cd_fn_check check, cd_fn_connect connect, cd_fn_disconnect disconnect, cd_fn_descriptor descriptor, t_cd_ep_dsc *ep_dsc)
{
  unsigned int i;

  for (i=0;i<MAX_CLASS_DRIVERS;i++)
  {
    if (cd_dsc[i].connect==NULL)
    {
      cd_dsc[i].check=check;
      cd_dsc[i].connect=connect;
      cd_dsc[i].disconnect=disconnect;
      cd_dsc[i].descriptor=descriptor;
      cd_dsc[i].ep_dsc=ep_dsc;
      return USBH_SUCCESS;
    }
  }

  return USBH_ERR_NOT_AVAILABLE;
}


/*
** Register HUB driver
** INPUT: ifc_hdl - interface handler
**        scan - HUB scan function
**        reset - port reset function
**        disable - port disable function
**        suspend - suspend function
**        resume - resume function
** RETURN: pointer or NULL if no more entries available
**
*/
void *pmgr_add_hub (void *ifc_hdl, hub_scan_fn scan, hub_fn reset, hub_fn disable, hub_fn suspend, hub_fn resume)
{
  hcc_u8 i;

  for (i=0;i<MAX_HUBS;i++)
  {
    if (hub_dsc[i].scan==NULL)
    {
      hub_dsc[i].ifc_hdl=ifc_hdl;
      hub_dsc[i].scan=scan;
      hub_dsc[i].reset=reset;
      hub_dsc[i].disable=disable;
      hub_dsc[i].suspend=suspend;
      hub_dsc[i].resume=resume;
      return hub_dsc+i;
    }
  }
  return NULL;
}

/*
** Delete a registered HUB driver
** INPUT: hub_hdl - HUB handler
** RETURN: USBH_SUCCESS
**         USBH_ERR_HOST - if deletion of any of the previously attached
**                         devices to the HUB failed.
*/
int pmgr_delete_hub (void *hub_hdl)
{
  int rc;
  t_hub_dsc *p_hub_dsc=(t_hub_dsc *)hub_hdl;

  rc=_pmgr_remove_hub_ports(hub_hdl);
  p_hub_dsc->scan=NULL;
  p_hub_dsc->ifc_hdl=NULL;

  return rc;
}


/*
** Initialize port manager
*/
int pmgr_init (void)
{
  hcc_uint i,j;

  _memset(hub_dsc,0,sizeof(hub_dsc));
  _memset(cd_dsc,0,sizeof(cd_dsc));
  _memset(port_dsc,0,sizeof(port_dsc));
  for (i=0;i<MAX_PORTS;i++)
  {
    for (j=0;j<MAX_INTERFACE_PER_PORT;j++)
    {
      port_dsc[i].ifc_dsc[j].state=USBH_STATE_DISCONNECTED;
      port_dsc[i].ifc_dsc[j].dev_hdl=NULL;
    }
  }
  _memset(daddr2port_dsc,0,sizeof(daddr2port_dsc));
  if (os_event_create(&pmgr_event_bit)) return USBH_ERR_RESOURCE;

  return USBH_SUCCESS;
}


/*
** Stop port manager
*/
int pmgr_start (void)
{
  return USBH_SUCCESS;
}


/*
** Stop port manager
*/
int pmgr_stop (void)
{
  hcc_u8 i;
  int rc;

  for (i=0;i<MAX_HUBS;i++)
  {
    if (hub_dsc[i].scan!=NULL)
    {
      rc=_pmgr_remove_hub_ports(hub_dsc+i);
      if (rc) return rc;
    }
  }

  return USBH_SUCCESS;
}


