/***************************************************************************

 ***************************************************************************/
#include "usb_host.h"
#include "VADefines.h"
#include "va_device.h"
#include "VAPacket.h"

#include "wchar.h"

#define     _MAX_PACKET_SIZE_     2048
//
hcc_u8 VAD_SendPacketBuffer[_MAX_PACKET_SIZE_] ;
hcc_u32 VAD_SendPacketSize ;
hcc_u8 VAD_ReadBuffer[_MAX_PACKET_SIZE_] ;
hcc_u32 VAD_ReadPacketSize ;

ResponsePacketHeader  VAD_rPacketHeader ;
CommandPacketHeader VAD_sPacketHeader ;
hcc_u8 VAD_sPacketData[2048] ;
hcc_u8 *VA_sPacketDataAddr ;

//
int VAD_SendPacket(void) ;
int VAD_Write(hcc_u8 dev, void *buf, hcc_u32 buf_length, hcc_u32 *rlength );
int VAD_Read(hcc_u8 dev, void *buf, hcc_u32 buf_length, hcc_u32 *rlength ) ;
void VAD_Append(hcc_u8 *des,hcc_u8 *source,hcc_u32 rlength );
int VAD_GetReceiverPacketStatus(void);
int VAD_SendPacket(void);
void VAD_CmdToSendBuffer(void);

hcc_u32 VAD_GetResponseResult(void) ;
hcc_u32 VAD_GetResponseCommand(void) ;
hcc_u32 VAD_GetResponseParameter1(void) ;
hcc_u32 VAD_GetResponseDataLength(void) ;
hcc_u8* VAD_GetResponseDataAddress(void) ;
void VAD_SendPacketReset(void) ;
void VAD_ReceiverPacketReset(void) ;
void VAD_SendPacketDataReset(void) ;

void VAD_SetCommand(long command,long parameter1,long parameter2,long length,hcc_u8 *addr ) ;
void VAD_SetString(const wchar_t *pString) ;
void VAD_SetData(hcc_u8 *DataAddr, hcc_u32 DataLength) ;
hcc_u8 VAD_GetState(void);
//
/******************************************************************************
 Local variables
 *****************************************************************************/
typedef struct {
  va_st_func *status;
  va_rw_func *transfer;
  void *dev_hdl;
  hcc_u8 state ;
} t_VA_dsc;

static t_VA_dsc VA_dsc[VA_MAX_DEVICE]; 



/******************************************************************************
 *****************************************************************************/
int VAD_init(void)
{
  _memset(&VA_dsc,0,sizeof(VA_dsc));	/* reset descriptor area */
  return 0;
}


void VAD_SendPacketReset(void) 
{
  _memset(&VAD_sPacketHeader, 0, sizeof(VAD_sPacketHeader));
  VAD_SendPacketSize = 0 ;
  return ;
}

void VAD_ReceiverPacketReset(void) 
{
  _memset(&VAD_rPacketHeader, 0, sizeof(VAD_rPacketHeader));
  VAD_ReadPacketSize = 0 ;
  return ;
}



void VAD_SendPacketDataReset(void)
{
  _memset(&VAD_sPacketData, 0, sizeof(VAD_sPacketData));
  VA_sPacketDataAddr = &VAD_sPacketData[0] ;
  return ;
}


/******************************************************************************
 *****************************************************************************/
int VAD_open(hcc_u8 dev,  va_rw_func *f, va_st_func *st, void *dev_hdl)
{
  t_VA_dsc *dsc=VA_dsc+dev;		/* get device descriptor position */
  int rc=VACR_DEVICE_NOT_OPEN;
  //hcc_u8 *TEST = "BIKV01b" ;
  //hcc_u8 *TEST1 = "Utah" ;
  //
  VAD_SetCommand(CMD_RESET,0,0,0,NULL) ;
  //----------------------------------------------------------------------------
  dsc->status=st;					            /* save status request function */
  dsc->transfer=f;					          /* save access function */
  dsc->dev_hdl=dev_hdl;				        /* save device handle */
  //----------------------------------------------------------------------------
  //
  if( VAD_SendPacket() == VACR_SUCCESS && VAD_rPacketHeader.Command == VAD_sPacketHeader.Command && VAD_rPacketHeader.Result == RSLT_SUCCESS )
      {
      rc = VACR_SUCCESS ;
      dsc->state = 1 ;
      /*
      VAD_sPacketHeader.Command = CMD_CHANGE_DIRECTORY ;
      VAD_sPacketHeader.DataLength = 8 ;
      VA_sPacketDataAddr = TEST ;
      VAD_SendPacket() ;
      
      VAD_sPacketHeader.Command = CMD_CHANGE_DIRECTORY ;
      VAD_sPacketHeader.DataLength = 5 ;
      VA_sPacketDataAddr = TEST1 ;
      VAD_SendPacket() ;
      
      VAD_sPacketHeader.Command = CMD_GET_DIRECTORY_LISTING ;
      VAD_sPacketHeader.DataLength = 0 ;
      VAD_SendPacket() ;
      
      VAD_sPacketHeader.Command = CMD_GET_DIRECTORY_LISTING ;
      VAD_sPacketHeader.Parameter1 = 1 ;
      VAD_sPacketHeader.DataLength = 0 ;
      VAD_SendPacket() ;
      */
      }
  //
  return rc ;
}


/******************************************************************************
 *****************************************************************************/
int VA_close(hcc_u8 dev)
{
  return 0;
}





int VAD_Write(hcc_u8 dev, void *buf, hcc_u32 buf_length, hcc_u32 *rlength )
{
  t_VA_dsc *dsc=VA_dsc+dev;		/* get device descriptor position */
  int rc ;
  //
  rc = dsc->transfer(dsc->dev_hdl,VA_DIR_OUT,buf,buf_length,rlength ) ;
  //
  return rc ;
}




int VAD_Read(hcc_u8 dev, void *buf, hcc_u32 buf_length, hcc_u32 *rlength )
{
  t_VA_dsc *dsc=VA_dsc+dev;		/* get device descriptor position */
  int rc ;
  //
  rc = dsc->transfer(dsc->dev_hdl,VA_DIR_IN,buf,buf_length,rlength ) ;
  //
  return rc ;
}


//
//
void VAD_Append(hcc_u8 *des,hcc_u8 *source,hcc_u32 rlength )
{
  hcc_u32 i ;
  //
  if( (VAD_ReadPacketSize+rlength) < _MAX_PACKET_SIZE_ )
      {   
        
      for( i = 0 ; i < rlength ; i++ )
          {
          *(des+i) = *(source+i) ;
          }
      //
      VAD_ReadPacketSize += rlength ;
      }
  //
  return ;
}




int VAD_GetReceiverPacketStatus(void)
{
	ResponsePacketHeader *pHdr;
	hcc_s32 PacketSize;
	hcc_u32 ChecksumCalculated = 0;
	hcc_u32 ChecksumActual = 0;
	hcc_u8 *pRawData = &VAD_ReadBuffer[0] ;

	pHdr = (ResponsePacketHeader*)&VAD_ReadBuffer[0];

	// Check initial size
	if( VAD_ReadPacketSize < sizeof(ResponsePacketHeader))
		return RPS_NotComplete;

	// Check packet size isn't too large
	PacketSize = sizeof(ResponsePacketHeader) + pHdr->DataLength + sizeof(hcc_u32);
	if((PacketSize > _MAX_PACKET_SIZE_) || (PacketSize < 0))
		return RPS_InvalidSize;

	// Check to see if the data arrival has completed
	if(VAD_ReadPacketSize < PacketSize)
		return RPS_NotComplete;

	// Verify checksum
	for(int i=0; i<(PacketSize - sizeof(hcc_u32)); i++)
		  ChecksumCalculated += pRawData[i];
  
	// To avoid data misalignment
	memcpy(&ChecksumActual, pRawData + sizeof(ResponsePacketHeader) + pHdr->DataLength, sizeof(hcc_u32));
	if(ChecksumCalculated != ChecksumActual)
		return RPS_InvalidChecksum;
  
  //
  VAD_rPacketHeader.Result = pHdr->Result ;
  VAD_rPacketHeader.Command = pHdr->Command ;
  VAD_rPacketHeader.Parameter1 = pHdr->Parameter1 ;
  VAD_rPacketHeader.DataLength = pHdr->DataLength ;
	//
  
  return RPS_Complete ;
}





hcc_u32 VAD_GetResponseResult(void)
{
  return VAD_rPacketHeader.Result ;
}


hcc_u32 VAD_GetResponseCommand(void)
{
  return VAD_rPacketHeader.Command ;
}

hcc_u32 VAD_GetResponseParameter1(void)
{
  return VAD_rPacketHeader.Parameter1 ;
}


hcc_u32 VAD_GetResponseDataLength(void) 
{
  return VAD_rPacketHeader.DataLength ;
}


hcc_u8* VAD_GetResponseDataAddress(void)
{
  return (&VAD_ReadBuffer[0]+16) ;
}


hcc_u8 VAD_GetState(void)
{
  t_VA_dsc *dsc=VA_dsc;		/* get device descriptor position */
  return dsc->state ;
}



void VAD_SetCommand(long command,long parameter1,long parameter2,long length,hcc_u8 *addr ) 
{
  VAD_sPacketHeader.Command = command ;
  VAD_sPacketHeader.Parameter1 = parameter1 ;
  VAD_sPacketHeader.Parameter2 = parameter2 ;
  VAD_sPacketHeader.DataLength = length ;
  VA_sPacketDataAddr = addr ;
  return ;
}


void VAD_SetString(const wchar_t *pString) 
{
  hcc_u32 Size;
  //
  if(pString == NULL)
		return;
  //
  VAD_SendPacketDataReset() ;
  //
  Size = wcslen(pString);
  
  for(int i=0; i<Size; i++)
			VAD_sPacketData[i] = pString[i];
  
  VAD_sPacketHeader.DataLength = Size ;
  //
  return ;
}


void VAD_SetData(hcc_u8 *DataAddr, hcc_u32 DataLength)
{
  //
  VAD_SendPacketDataReset() ;
  //
  VAD_sPacketHeader.DataLength = DataLength ;
	_memcpy(VA_sPacketDataAddr, DataAddr, DataLength);
  //
  return ;
}


//
//
int VAD_SendPacket(void)
{
  //
  int rc = VACR_DEVICE_NOT_OPEN ;
  ResponsePacketHeader ResponseHeader ;
  hcc_u32 ResponseChecksum = 0 ;
  hcc_u8  ResponseData[64] ;
  hcc_u32 rlength = 0 ;
  hcc_u32 DataLength = 0 ;
  hcc_u8 retry_cmd ;
  int ReceiveStatus = 0 ;
  //
  VAD_CmdToSendBuffer() ;
  // Clear previous Response Header
  _memset(&VAD_rPacketHeader, 0, sizeof(VAD_rPacketHeader));
  //
  for(retry_cmd=0 ; retry_cmd < VA_RETRY_COUNT ;retry_cmd++) 
      {
      //
      VAD_ReadPacketSize = 0 ;
      //_memset(&VAD_ReadBuffer, 0, sizeof(VAD_ReadBuffer));
      //
      if( VAD_Write(0, &VAD_SendPacketBuffer, VAD_SendPacketSize, &rlength ) == USBH_SUCCESS )
          {
          //
          //_memset(&ResponseHeader, 0, sizeof(ResponseHeader));
          //
          if( VAD_Read(0, &ResponseHeader, sizeof(ResponseHeader), &rlength ) == USBH_SUCCESS )
              {
              //
              VAD_Append((&VAD_ReadBuffer[0]+VAD_ReadPacketSize),(hcc_u8 *)&ResponseHeader,rlength) ;
              //
              while(ResponseHeader.DataLength != DataLength )
                  {
                  if( ResponseHeader.DataLength > 64 )
                      rc = VAD_Read(0, &ResponseData, 64, &rlength ) ;
                  else  
                      rc = VAD_Read(0, &ResponseData, ResponseHeader.DataLength, &rlength ) ;
                  
                  if( rc == USBH_SUCCESS || rc == USBH_SHORT_PACKET )
                      {
                      VAD_Append((&VAD_ReadBuffer[0]+VAD_ReadPacketSize),(hcc_u8 *)&ResponseData,rlength) ;
                      DataLength += rlength ;
                      if( rlength == 0 )
                          DataLength = ResponseHeader.DataLength ;  
                      usbh_delay(1);  /* retry */
                      }
                  else
                      DataLength = ResponseHeader.DataLength ;
                  }
              //
              if( VAD_Read(0, &ResponseChecksum, sizeof(ResponseChecksum), &rlength ) == USBH_SUCCESS )
                  {
                  VAD_Append((&VAD_ReadBuffer[0]+VAD_ReadPacketSize),(hcc_u8 *)&ResponseChecksum, rlength) ;
                  }
              //
              ReceiveStatus = VAD_GetReceiverPacketStatus() ;
              switch(ReceiveStatus)
                  {
                  case RPS_Complete:
                                          retry_cmd = VA_RETRY_COUNT ;
                                          rc = VACR_SUCCESS ;
						                              break;
                  case RPS_NotComplete:
                                          break;
                  case RPS_InvalidChecksum:
                                          rc = VACR_INVALID_CHECKSUM;
                                          break;
                  case RPS_InvalidSize:
                                          rc = VACR_INVALID_SIZE;
                                          break;
                  default:
                                          retry_cmd = VA_RETRY_COUNT ;
                                          rc = VACR_GENERIC_FAILURE;
                                          break;
                  }
              //
              } 
          }
      //
      if( ReceiveStatus != RPS_Complete )
          usbh_delay(VA_RETRY_WAIT_MS);  /* retry */
       
      //
      }
  return rc ;
}



//
//
void VAD_CmdToSendBuffer(void)
{
  hcc_u32 CheckSum = 0 ;
  hcc_u32 i ;
    
  VAD_SendPacketSize = 0 ;
  
  VAD_Append((&VAD_SendPacketBuffer[0]+VAD_SendPacketSize),(hcc_u8 *)&VAD_sPacketHeader,sizeof(VAD_sPacketHeader)) ;
  VAD_SendPacketSize += sizeof(VAD_sPacketHeader) ;
  //
  if( VAD_sPacketHeader.DataLength > 0 && VA_sPacketDataAddr != NULL )
      {
      VAD_Append((&VAD_SendPacketBuffer[0]+VAD_SendPacketSize),VA_sPacketDataAddr,VAD_sPacketHeader.DataLength) ;
      VAD_SendPacketSize += VAD_sPacketHeader.DataLength ;
      }
  //
  for( i = 0 ; i < VAD_SendPacketSize ; i++ )
      {
      CheckSum += VAD_SendPacketBuffer[i] ;
      }
  //
  WR_LE32((&VAD_SendPacketBuffer[0]+VAD_SendPacketSize),CheckSum) ;
  VAD_SendPacketSize += sizeof(CheckSum) ;
  //
  return ;
}

































