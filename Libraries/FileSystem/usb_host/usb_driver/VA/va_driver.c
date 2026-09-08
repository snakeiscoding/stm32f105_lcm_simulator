/***************************************************************************
 
 ***************************************************************************/
#include "usb_host.h"
#include "usb_utils.h"
#include "pmgr.h"
#include "VAPacket.h"
#include "va_device.h"
#include "va_driver.h"
#include "VADefines.h"

#define VA_VID        0x0471
#define VA_PID        0x0666
#define VA_CLASS		  0xDC
#define VA_SCLASS		  0xA0
#define VA_PROTOCOL	  0xB0


typedef enum {
  VAerr_none,
  VAerr_comunication_error
} VA_error_t;


#define EP0		      0
#define INT_IN		  1
#define INT_OUT		  2
#define BULK_IN		  3
#define BULK_OUT		4
static const t_cd_ep_dsc VA_cd_ep_dsc[6] = {
  5,
  USBH_EP_DSC(USBH_TYPE_CONTROL,0),
  USBH_EP_DSC(USBH_TYPE_INT,USBH_DIR_IN),
  USBH_EP_DSC(USBH_TYPE_INT,USBH_DIR_OUT),
  USBH_EP_DSC(USBH_TYPE_BULK,USBH_DIR_IN),
  USBH_EP_DSC(USBH_TYPE_BULK,USBH_DIR_OUT)
};



typedef struct {
  hcc_u8 VA_device;
  void *ifc_hdl;
  void *ep_hdl[5];
  //hcc_u8 nlun;
  VA_error_t VA_error;
} t_VA_info;
t_VA_info VA_info[VA_MAX_DEVICE];






void VA_CopyDataToBuffer(hcc_u8 *des, hcc_u8 *source,hcc_u32 rlength ) ;

/*
** VA_check
**
** Check whether current interface is Virtual Active
**
** Input: dev_inf - device info
**        port_hdl - port handler
**        ifc_info - interface info
** Output: dev_hdl - device handler
** Return: NULL - not found
**         otherwise pointer to ep_hdl array
*/
static void **VA_check (void **dev_hdl, void *port_hdl, device_info_t *dev_inf, ifc_info_t *ifc_inf)
{
  (void)port_hdl;
  (void)dev_inf;
  if ( dev_inf->vid == VA_VID && dev_inf->pid == VA_PID && ifc_inf->_class==VA_CLASS 
      && ifc_inf->sclass==VA_SCLASS && ifc_inf->protocol==VA_PROTOCOL )
  {
    hcc_uint i;
    for (i=0;i<VA_MAX_DEVICE && VA_info[i].VA_device;i++) ;
    if (i < VA_MAX_DEVICE )
    {
    *dev_hdl=VA_info+i;
    return VA_info[i].ep_hdl;
    }
  }
  return NULL;
}



/*
** VA_connect
**
** Connect Virtual Active device
** Callback function for port manager class driver handler.
**
** Input: dev_hdl - device handler
**        ifc_hdl - port handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static int VA_connect (void *dev_hdl, void *ifc_hdl)
{
 
  t_VA_info *VA=(t_VA_info *)dev_hdl;
  if(VA->ep_hdl[EP0] && VA->ep_hdl[BULK_IN] && VA->ep_hdl[BULK_OUT])
  {
    VA->VA_error=VAerr_none;
 
    if( VAD_open((hcc_u8)(VA-VA_info),VA_transfer,VA_state,dev_hdl) == VACR_SUCCESS  )
        {
        VA->VA_device=1;
        VA->ifc_hdl=ifc_hdl;
        return USBH_SUCCESS;
        }
  }
  return USBH_ERR_NOT_AVAILABLE;
}


/*
** VA_disconnect
**
** Input: dev_hdl - device handler
**
** Callback function for usb host class driver handler.
*/
static int VA_disconnect (void *dev_hdl)
{
  t_VA_info *VA=(t_VA_info *)dev_hdl;
  VA->VA_device=0;
  VA_close((hcc_u8)(VA-VA_info));

  return USBH_SUCCESS;
}







static int VA_clear_stall_rd(t_VA_info *VA)
{
  if (clear_stall(VA->ep_hdl[EP0],VA->ep_hdl[BULK_IN])!=USBH_SUCCESS)
  {
    VA->VA_error=VAerr_comunication_error;
    return 1;
  }
  return 0;
}

static int VA_clear_stall_wr(t_VA_info *VA)
{
  if (clear_stall(VA->ep_hdl[EP0],VA->ep_hdl[BULK_OUT])!=USBH_SUCCESS)
  {
    VA->VA_error=VAerr_comunication_error;
    return 1;
  }
  return 0;
}



/*
** Initialize mass storage module
*/
int VA_init (void)
{
  unsigned int i;
  for (i=0;i<VA_MAX_DEVICE;VA_info[i++].VA_device=0);
  VAD_init() ;
  return pmgr_add_cd(VA_check,VA_connect,VA_disconnect,NULL,(t_cd_ep_dsc *)&VA_cd_ep_dsc);
}


/*
** Suspend
*/
int VA_suspend (hcc_u8 dev)
{
  t_VA_info *VA=VA_info+dev;
  return pmgr_suspend(VA->ifc_hdl);
}

/*
** Resume
*/
int VA_resume (hcc_u8 dev)
{
  t_VA_info *VA=VA_info+dev;
  return pmgr_resume(VA->ifc_hdl);
}




/*
** Get current state of the device.
** RETURN: USBH_CONNECTED/USBH_DISCONNECTED/USBH_CHANGED
*/
hcc_u8 VA_state (void *dev_hdl)
{
  t_VA_info *VA=(t_VA_info *)dev_hdl;
  if (VA->VA_device==0) return USBH_STATE_DISCONNECTED;
  return pmgr_get_dev_state(VA->ifc_hdl);
}







/*
** VA transfer
*/
//int VA_transfer( void *dev_hdl ,void *cmd, hcc_u32 cmd_length, void *buf , hcc_u32 *buf_length) 
int VA_transfer( void *dev_hdl ,hcc_u8 dir , void *buf , hcc_u32 buf_length ,hcc_u32 *rlength ) 
{
  //
  t_VA_info *VA=(t_VA_info *)dev_hdl;
  int rc ;
  
  /* send/read data if needed */
  if (buf_length && dir!=VA_DIR_NONE)
      {
      switch(dir)
          {
          case VA_DIR_IN: /* transfer data to host */
                        rc = usbh_receive_b(VA->ep_hdl[BULK_IN],(hcc_u8 *)buf,buf_length,rlength,USBH_TIMEOUT_BULK) ;
                        if ( rc != USBH_SUCCESS )
                            {
                            /* Less data read than expected. In this state the spec says, the device may
                            stall its endpoint to stop the transfer. Another way is if the device sends a
                            short packet.
                            Since on windows we don't know if the endpoint is stalled or not, we send a
                            CLEAR_FEATURE ENDPOINT_HALT request to the endpoint anyway. Though this is not
                            neccessary in the sencond case, it can not harm. After this we continue to the
                            status stage, to see what happend. */
                            VA_clear_stall_rd(VA) ;
                            }
                        break;
          case VA_DIR_OUT: /* transfer data to device */
                        rc = usbh_send_b(VA->ep_hdl[BULK_OUT],(hcc_u8 *)buf,buf_length,rlength,USBH_TIMEOUT_BULK) ;
                        if ( rc != USBH_SUCCESS )
                            {
                            /* Data write failed. If the device is still connected, the only way this may
                            happen, if the device stalled its input endpoint.
                            Clear stall, and continue to the status stage to see what happend. */
                            VA_clear_stall_wr(VA);
                            }
                        break;
          default:
                        break;
          }
      }
  return rc ; 
  
  
  
  
  
  /*
  ResponsePacketHeader ResponseHeader ;
  hcc_u32 ResponseChecksum ;
  hcc_u8  ResponseData[64] ;
  hcc_u32 rlength ;
  int rc= 0 ;
  hcc_u8 *rbuf ;
  rbuf = buf ;
  *buf_length = 0 ;
  */
  //
  //  
  /*
  rc = usbh_send_b(VA->ep_hdl[BULK_OUT],(hcc_u8 *)cmd,cmd_length,NULL,USBH_TIMEOUT_BULK);
  if( rc == USBH_SUCCESS )
      {
      //
      _memset(&ResponseHeader, 0, sizeof(ResponseHeader));
      // 
      rc = usbh_receive_b(VA->ep_hdl[BULK_IN],(hcc_u8 *)&ResponseHeader,sizeof(ResponseHeader),&rlength,USBH_TIMEOUT_BULK) ;
      if( rc == USBH_SUCCESS )
          {
          //
          *buf_length += rlength ;
          VA_CopyDataToBuffer(rbuf,(hcc_u8 *)&ResponseHeader,rlength) ;
          rbuf += rlength ;
          //
          if(ResponseHeader.DataLength > 0)
              {
              rc = usbh_receive_b(VA->ep_hdl[BULK_IN],(hcc_u8 *)&ResponseData,ResponseHeader.DataLength,&rlength,USBH_TIMEOUT_BULK) ;
              if( rc != USBH_SUCCESS )
                  {
                  VA_clear_stall_rd(VA) ;
                  return rc ;
                  }    
              else
                  {
                  //
                  *buf_length += rlength ;
                  VA_CopyDataToBuffer(rbuf,(hcc_u8 *)&ResponseData,rlength) ;
                  rbuf += rlength ;
                  //
                  }
              }
          //
          rc = usbh_receive_b(VA->ep_hdl[BULK_IN],(hcc_u8 *)&ResponseChecksum,sizeof(ResponseChecksum),&rlength,USBH_TIMEOUT_BULK) ;
          if( rc != USBH_SUCCESS )
              {
              VA_clear_stall_rd(VA) ;
              return rc ;
              }
          else
              {
              //
              *buf_length += rlength ;
              VA_CopyDataToBuffer(rbuf,(hcc_u8 *)&ResponseChecksum,rlength) ;
              rbuf += rlength ;
              //
              }
          }
      else
          VA_clear_stall_rd(VA) ;
      }
  else
      VA_clear_stall_wr(VA) ;
  return rc;
  */
  
}



/*
void VA_CopyDataToBuffer(hcc_u8 *des, hcc_u8 *source,hcc_u32 rlength )
{
  hcc_u32 i ;
  //
  for( i = 0 ; i < rlength ; i++ )
      {
      *(des+i) = *(source+i) ;
      }
  //
  return ;
}
*/








