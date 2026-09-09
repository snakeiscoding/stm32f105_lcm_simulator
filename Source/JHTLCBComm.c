
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
#define _EnableErrorCode_

// Private Variables
/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#include  "JHTLCBComm.h"
#include  "JHTCOMMAND.h"
#include  "Errorcode.h"
#include  "LCBSimulator.h"
#include  "EE93CXX.h"


#ifdef _SupportPhoenixConsole
#include  "RowerWatts.h"
#endif

#ifdef  DebugMonitor
#include <stdio.h>
#include <string.h>
#endif


//
#define   JHTLCBComm_TxRxInterrupt        _JHT_COMM_IRQ_Func
//

//#define   _LogUCBCommandData_


/* External variables -----------------------------------------------------------*/



/* Private typedef -----------------------------------------------------------*/



/*
typedef struct {
  unsigned char Command ;
  unsigned char Length ;
  unsigned char SubCommand ;
  unsigned char Rev ;
} SupportCommand ;


const SupportCommand LCB1_CommandList[] = { { CmdInitial, 0, 0,0}, { CmdGetStatus , 0, 0, 0 }, { CmdGetErrorCode, 0, 0, 0 } } ;
const SupportCommand LCB2_CommandList[] ;
const SupportCommand LCB3_CommandList[] ;
const SupportCommand LCBA_CommandList[] ;
const SupportCommand Climbmill_CommandList[] ;
const SupportCommand TM_CommandList[] ;
*/





/* Private define ------------------------------------------------------------*/
#define   _RxTimeoutTime                500
#define   _RxDisconnectTime             7000    // Modify 20250701 , 3000 
#define   _CheckErrorTime               200
#define   _CheckErrorSwapDelayTime      (3000/_CheckErrorTime)
#define   _CheckErrorDelayTime          (10000/_CheckErrorTime)
#define   _TMCheckErrorDelayTime        (3000/_CheckErrorTime)
#define   _MaxErrorBufferSize           5
#define   _UCBOfflineTime               (60000/_RxDisconnectTime)
#define   _PackageLength_MaxDataLegth   255

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const unsigned char JHTCrcTab[16]={0x00,0x31,0x62,0x53,0xc4,0xf5,0xa6,0x97,0xb9,0x88,0xdb,0xea,0x7d,0x4c,0x1f,0x2e};


volatile CommControl  CommControlFlag ;

//------------------------------------------------------------------------------
#define RxSize        (1024*2+10)//200
unsigned char RxBuffer[RxSize] ;
unsigned char *RxStartPoint; 
unsigned char *RxEndPoint;
// 20120204 Add Tx FIFO Control
#define TxSize        (1024*2+10)//200 
unsigned char TxBuffer[TxSize] ;
unsigned char *TxStartPoint; 
unsigned char *TxEndPoint;
//
RxDataStruct RxAnalsys ;
ErrorStatusDataStruct ErrorCodeStatus ;
SpecControlDataStruct SpecControlStatus ; //Add 20250731 No user
//------------------------------------------------------------------------------
unsigned short TxDataPoint ;
unsigned short TxDataLength ;
TxDataStruct  TxData ;
volatile unsigned char TxDelayTimeCounter ;
LCBStatusDataStruct   LCBStatus ;
volatile unsigned short RxTimeoutCounter ;
volatile unsigned short RxDisconnectCounter ;
volatile unsigned char  UCBOfflineCounter ;
//------------------------------------------------------------------------------
//20210602 ITC
ITCLCBStatusDataStruct  ITCStatus ;



//------------------------------------------------------------------------------
// Error code
unsigned short	ErrorMessage[3][_MaxErrorBufferSize] ;
unsigned char	RetErrType ;
unsigned short	RetErrorCode ;
volatile unsigned char	  OldRetErrType ;
volatile unsigned short	OldRetErrorCode ;
unsigned short	CheckErrorCodeTimeCounter  ;
unsigned short  CheckErrorCodeDelayTimeCounter ;
unsigned short  ErrorSendDelayTime ;
unsigned char   GetClassCErr ;

// Rx/Tx by pass 
volatile unsigned char UpdateLoader_A_TxRxChangeTime ;
volatile unsigned char EnableTxRxChangeTime ;

// Version Struct
LCBVersionInfo  LCBVersion ;

// for 2017 RIS LCB Master Mode
RISLCBStatusDataStruct    RIS_LCBStatus ;
unsigned char RIS_SeqNumber ;
RISPacketFileType *RISRxAnalsys ;
RISPacketFileType RISTxData ;
unsigned short JHTLCBCommSystemTime ;
unsigned short RISMasterModeTime ;

// 20200115 log commamd
#define _Log0441CmdSize     30
unsigned short Log0441Cmd[_Log0441CmdSize] ;
unsigned char Log0441Index ;

//Log 20130702
#ifdef  _LogUCBCommandData_
#define   _LogCommandSize                 25
unsigned char CommandLogData[_LogCommandSize][2] ;
unsigned long LogRxCount ;
unsigned long LogTxCount ;
unsigned short LogWaitTxTime ;
#endif
//
//Add by Kunlung 20180402 ESTOP Release Check
unsigned char TMReleaseEstopStatus ;
//

/* Private function prototypes -----------------------------------------------*/
unsigned char JHTLCBComm_CRC8( unsigned char *ptr, unsigned short DATALENGTH ) ;
void JHTLCBComm_CommandDecoder(unsigned char ECmd ) ;
unsigned char JHTLCBComm_CheckSupportCommand(unsigned char CheckCmd,unsigned char SubCmd) ;
unsigned char JHTLCBComm_CheckCommandAndLength(unsigned char CheckCmd, unsigned char SubCmd,unsigned char CheckLen ) ;
void JHTLCBComm_ReturnData( unsigned char TransCmd, unsigned char *DataPtr, unsigned char DataLen ) ;
//

//
void RxFunc_Init(void) ;
unsigned char* RxFunc_NextPt(unsigned char *pt) ;
unsigned char RxFunc_IsEmpty(void) ;
unsigned char RxFunc_IsFull(void) ;
unsigned short RxFunc_Length(void) ;
void RxFunc_Putc(unsigned char c) ;
unsigned char RxFunc_Getc(void) ;
// 20120204 Add Tx FIFO Control
void TxFunc_Init(void) ;
unsigned char* TxFunc_NextPt(unsigned char *pt) ;
unsigned char TxFunc_IsEmpty(void) ;
unsigned char TxFunc_IsFull(void) ;
unsigned short TxFunc_Length(void) ;
void TxFunc_Putc(unsigned char c) ;
unsigned char TxFunc_Getc(void) ;
//

void JHTLCBComm_RISCommandDecoder(unsigned char ECmd ) ;
unsigned char JHTLCBComm_CheckRISCommandAndLength(unsigned char CheckCmd,unsigned char CheckLen ) ;
void JHTLCBComm_RISReturnData( unsigned char TransCmd, unsigned char *DataPtr, unsigned char DataLen ) ;
unsigned char JHTLCBComm_RISSensorData( unsigned char *DataPtr ) ;
void JHTLCMComm_LogUCBCommand(unsigned char iCmd,unsigned char sCmd) ;
//
void JHTLCBComm_log0441Command(unsigned char LogCMD, unsigned char LogCMD1) ;
//

/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_Initial(void)
{
  unsigned short i ;
  //
  RxFunc_Init() ;             // Initial Rx Function
  TxFunc_Init() ;             // Initial Tx Function
  TxDataPoint = 0 ;           // Clear Tx Data Point
  TxDelayTimeCounter = 0 ;    // Clear Tx Delay Time
  TxDataLength = 0 ;          // Clear Tx Data Length
  LCBStatus.Full = 0 ;        // Clear All Status Bit
  RxTimeoutCounter = 0 ;      // Clear Rx Timeout Count 
  RxDisconnectCounter = 0 ;   // Clear Disconnect Count 
  UCBOfflineCounter = 0 ;     // Clear Offline Count 
  CommControlFlag.Full = 0 ;  // Clear All Control Flag 
  //
  for( i = 0 ; i < _MaxErrorBufferSize ; i++ )
      {  
      ErrorMessage[0][i] = 0 ;
      ErrorMessage[1][i] = 0 ;
      ErrorMessage[2][i] = 0 ;
      }
  RetErrType = 0 ;
  RetErrorCode = 0 ;
  OldRetErrType = 0 ;
  OldRetErrorCode = 0 ;  
  CheckErrorCodeTimeCounter = 0 ;
  CheckErrorCodeDelayTimeCounter = 0 ;
  ErrorSendDelayTime = _CheckErrorDelayTime ;
  GetClassCErr = 0 ;
  //
  //
  /*
  LCBVersion.Ver.MCU_A_Loader = 0x1301 ;
  LCBVersion.Ver.MCU_A_API = 0x1301 ;
  LCBVersion.Ver.MCU_B_Loader = 0x1301 ;
  LCBVersion.Ver.MCU_B_API = 0x1304 ;
  */
  //
  CommControlFlag.Bits.DisableErPOnCmd = 1 ;
  CommControlFlag.Bits.DirectorRXD = 1 ;

  // for RIS LCB
  CommControlFlag.Bits.RIS_LCBMode = 0 ;              // Default for Normal LCB Communication Mode  
  CommControlFlag.Bits.RIS_MasterMode = 0 ;           // Add By Kunlung 20170511
  RISLCBParameter.StreamRate = 50 ;                   // Default 50ms
  //
  oRS485Rx(_RXD) ; // 20130724
  // clear command log data buffer
  // 20200115 
  for( i = 0 ; i < _Log0441CmdSize; i++ )
      {
      Log0441Cmd[i] = 0 ;
      }
  Log0441Index = 0 ;
  /*
  for ( i = 0 ; i < _LogCommandSize ; i++ )
      {
      CommandLogData[i][0] = 0 ;
      CommandLogData[i][1] = 0 ;
      }
  */
  
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_HW_Initial(void)
{
  USART_InitTypeDef 	USART_InitStructure;
  NVIC_InitTypeDef 		NVIC_InitStructure;

  //
  NVIC_InitStructure.NVIC_IRQChannel = _JHT_COMM_IRQ;//Channel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);	
  USART_DeInit(_JHT_COMM_PORT) ;

  // Initial RxData ;
  JHTLCBComm_Initial() ;
  //RxFunc_Init() ;
  
  //----------------------------------------------------------------------------
  USART_InitStructure.USART_BaudRate = 9600 ;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  // Configure USART2 
  USART_Init(_JHT_COMM_PORT, &USART_InitStructure);
  // Enable JHTUARTPORT Receive interrupt 
  USART_ITConfig(_JHT_COMM_PORT, USART_IT_RXNE, ENABLE);
  // Enable JHTUARTPORT Transmit interrupt 
  USART_ITConfig(_JHT_COMM_PORT,USART_IT_TC,ENABLE);
  //
  USART_Cmd(_JHT_COMM_PORT, ENABLE);
  //
  // Enable the USART2 Interrupt 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
  //
  return ;
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_TxRxInterrupt(void)
{
  unsigned char RxData ;
  //----------------------------------------------------------------------------  
  if(USART_GetITStatus(_JHT_COMM_PORT, USART_IT_RXNE) != RESET) 
      {        
      RxData = USART_ReceiveData(_JHT_COMM_PORT);
      // Clear the JHTUARTPORT Receive interrupt 
      USART_ClearITPendingBit(_JHT_COMM_PORT, USART_IT_RXNE);
      //
      RxFunc_Putc(RxData) ;
      RxTimeoutCounter = 0 ;
      //
      }
  //---------------------------------------------------------------------------
  //
  if(USART_GetITStatus(_JHT_COMM_PORT, USART_IT_TC) != RESET)
      {
      // Clear the JHTUARTPORT transmit interrupt 
      USART_ClearITPendingBit(_JHT_COMM_PORT, USART_IT_TC);
      if( CommControlFlag.Bits.TxAction == 1 ) // 20130702
          {
          if( TxDataPoint < TxDataLength )
              {
              if( CommControlFlag.Bits.RIS_LCBMode == 1 )
                  {
                  USART_SendData(_JHT_COMM_PORT,RISTxData.Buffer[TxDataPoint]) ;
                  }
              else
                  {
                  USART_SendData(_JHT_COMM_PORT,TxData.Buffer[TxDataPoint]) ;
                  }
              TxDataPoint += 1 ;
              }
          else
              {
              // End of Transmit change to Receiver
              TxDataPoint = 0 ;
              TxDataLength = 0 ;
              CommControlFlag.Bits.TxAction = 0 ;
              //
              if( CommControlFlag.Bits.UCBOnline == 1 && CommControlFlag.Bits.EntryErPMode == 0 )
                  CommControlFlag.Bits.RxTimeoutCheckStart = 1 ; // Enable Receive Timeout Counter
              //
              oRS485Rx(_RXD) ;       
              CommControlFlag.Bits.DirectorRXD = 1 ;
              //
              if( CommControlFlag.Bits.CheckErPCommandReturn == 1 )
                  {
                  LCBSimulator_SetErPStatus(1) ;
                  CommControlFlag.Bits.EntryErPMode = 1 ;          // Set Erp Mode
                  CommControlFlag.Bits.CheckErPCommandReturn = 0 ;
                  }
              //
              }
          }
      else 
          {
          TxDataPoint = 0 ;
          TxDataLength = 0 ;
          if( CommControlFlag.Bits.DirectorRXD == 0 )
              {
              oRS485Rx(_RXD) ;       
              CommControlFlag.Bits.DirectorRXD = 1 ;
              }
          }
      }  

  return ;
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_RxProcess(void)
{
#ifdef   _SupportPhoenixConsole
  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
      {
      JHTLCBComm_RIS_RxProcess() ;
      }
  else
      {
      JHTLCBComm_LCBRxProcess() ;
      }
#else
  JHTLCBComm_LCBRxProcess() ;
#endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_LCBRxProcess(void)
{
  //
  unsigned char *pt1;
  unsigned short i ;
  unsigned short Length ;
  unsigned char CheckSum ;
  //
  // Error Process
  #ifdef _EnableErrorCode_
  JHTLCBComm_ErrorMessageProcess() ;
  #endif
  //
  if( CommControlFlag.Bits.RxEndWait5msReturn == 0 )
      {
      //---------------------------------------------------------------------------
      // Check Receive Buffer
      if(RxFunc_IsEmpty())
          {
          return ;
          }
        
      // Check Start Byte
      pt1 = RxStartPoint;
      if( *pt1 != 0x00 )    // UCB_START_BYTE
          {
          RxFunc_Getc();    // drop byte	
          return ;
          }
      //
      if( RxFunc_Length() > 4 )
          {
          //----------------------------------------------------------------
          RxAnalsys.member.Start = *pt1 ;
          pt1 = RxFunc_NextPt(pt1) ;
          //
          RxAnalsys.member.Address = *pt1 ;
          // 20210602 Add ITC Load Cell
          /*
          if( RxAnalsys.member.Address != 0xFF ) // UCB_ADDRESS_BYTE
              {
              RxFunc_Getc();    // drop byte	
              if( RxAnalsys.member.Address != 0x00 )
                  RxFunc_Getc();    // drop byte	  
              return ;
              }
          */
          if( LCBParameter.LCBSimulatorType == _ITC )
              {
              if( RxAnalsys.member.Address != 0x00 ) 
                  {
                  RxFunc_Getc();    // drop byte	
                  RxFunc_Getc();    // drop byte	  
                  return ;
                  }
              }
          else
              {
              if( RxAnalsys.member.Address != 0xFF ) // UCB_ADDRESS_BYTE
                  {
                  RxFunc_Getc();    // drop byte	
                  if( RxAnalsys.member.Address != 0x00 )
                      RxFunc_Getc();    // drop byte	  
                  return ;
                  }
              }
          //--------------------------------------------------------------------
          pt1 = RxFunc_NextPt(pt1) ;
          RxAnalsys.member.Command = *pt1 ;
          pt1 = RxFunc_NextPt(pt1) ;                  
          RxAnalsys.member.Length = *pt1 ;
          pt1 = RxFunc_NextPt(pt1) ;
          Length = RxAnalsys.member.Length ;
          //------------------------------------------------------------
          // 20130819 UCB Send data error
          // Delete Command = 0
          if( RxAnalsys.member.Command == 0x00 )
              {
              //20210602 Modify drop Start code , The ITC address is 0x00, This only drop start byte
              /*  
              RxFunc_Getc();    // drop StartCode byte	
              RxFunc_Getc();    // drop Address byte	
              */
              RxFunc_Getc();      // drop StartCode byte	
              if( LCBParameter.LCBSimulatorType != _ITC )
                  RxFunc_Getc();    // drop Address byte	
              return ; 
              }
#ifndef  _SupportSerialNumberCommand           
          else
              {
              // Check Length
              if( RxAnalsys.member.Length > 10 )
                  {
                  RxFunc_Getc();    // drop byte	
                  RxFunc_Getc();    // drop byte	
                  RxFunc_Getc();    // drop byte	
                  RxFunc_Getc();    // drop byte	
                  return ; 
                  }
              }
#endif             
          //----------------------------------------------------------------
          if( RxFunc_Length() >= (Length+5))
              {
              // Move Data to Buffer  
              for( i = 0 ; i < Length ; i++ )
                  {
                  RxAnalsys.member.Data[i] = *pt1 ;
                  pt1 = RxFunc_NextPt(pt1) ;
                  }
              //------------------------------------------------------------
              Length += 4 ;
              CheckSum = *pt1 ;
              if( CheckSum == JHTLCBComm_CRC8( &RxAnalsys.Buffer[0],Length ) )
                  {
                  //--------------------------------------------------------
                  CommControlFlag.Bits.RxTimeoutCheckStart = 0 ;
                  CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 0 ;                            
                  UCBOfflineCounter = 0  ; // Clear UCB Offline Counter
                  RxTimeoutCounter = 0 ;
                  RxDisconnectCounter = 0 ;
                  CommControlFlag.Bits.UCBOnline = 1 ;
                  CommControlFlag.Bits.UCBOffline = 0 ;
                  //20130816
                  if( ErrorCodeStatus.bit.EC04A0 == 1 )
                      {
                      ErrorCodeStatus.bit.EC04A0 = 0 ;
                      //JHTLCBComm_SkipErrorCode04A0() ;
                      }
                  //----------------------------------------------------------
                  if( CommControlFlag.Bits.DisableErPOnCmd == 0 )
                      CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 1 ;
                  
                  //---------------------------------------------------------
                  JHTLCBComm_CommandDecoder( RxAnalsys.member.Command ) ;
                  // Clear buffer // modify 20210602
                  for( i = 0 ; i < (Length+1) ; i++ )
                      {
                      RxFunc_Getc();    
                      }
                  //
                  CommControlFlag.Bits.RxEndWait5msReturn = 1 ;
                  //CommControlFlag.Bits.TxAction = 1 ; // 20130618
                  //--------------------------------------------------------
                  // Set Status LED Blink
                  CommControlFlag.Bits.StatusLED = ~CommControlFlag.Bits.StatusLED ;
                  //--------------------------------------------------------
                  }
              else
                  {
                  RxFunc_Getc();    // drop byte
                  CommControlFlag.Bits.RxEndWait5msReturn = 0 ;
                  CommControlFlag.Bits.TxAction = 0 ;
                  }
              }
          }
      }
  else
      {
      // Delay 5ms to Response to UCB
      if( TxDelayTimeCounter > 5 ) 
          {
          CommControlFlag.Bits.RxEndWait5msReturn = 0 ;
          CommControlFlag.Bits.TxAction = 1 ; // 20130618
          oRS485Rx(_TXD) ;
          CommControlFlag.Bits.DirectorRXD = 0 ;
          TxDelayTimeCounter = 0 ;
          TxDataPoint = 1 ;
          USART_SendData(_JHT_COMM_PORT,TxData.Buffer[0]) ;
          }
      }
  //----------------------------------------------------------------------------
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_CRC8( unsigned char *ptr, unsigned short DATALENGTH )
{
  unsigned char crchecktemp,crchalf,CHECKDATA=0;
  unsigned short crcheckcnt = 0 ;
  //
  for(crcheckcnt =0; crcheckcnt < DATALENGTH; crcheckcnt ++)
      {
      crchecktemp = *(ptr+crcheckcnt);
      crchalf =(CHECKDATA/16);
      CHECKDATA<<=4;
      CHECKDATA^=JHTCrcTab[crchalf ^( crchecktemp /16)];
      crchalf =(CHECKDATA/16);
      CHECKDATA<<=4;
      CHECKDATA^=JHTCrcTab[crchalf ^( crchecktemp &0x0f)];
      }
  //
  return CHECKDATA;
}


#ifdef  _LogUCBCommandData_
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCMComm_LogUCBCommand(unsigned char iCmd,unsigned char sCmd)
{
  unsigned char i ;
  for( i = 0 ; i < _LogCommandSize ; i++ )
      {
      if( CommandLogData[i][0] != iCmd  )
          {
          if( CommandLogData[i][0] == 0x00 )
              {
              CommandLogData[i][0] = iCmd ;
              CommandLogData[i][1] = sCmd ;
              return ;
              }
          }
      else
          {
          if( sCmd == 0 && iCmd != CmdSetClimbmillStatus ) 
              return ;
          //
          if( CommandLogData[i][1] != sCmd  )
              {
              CommandLogData[i][1] = sCmd ;
              break ;
              }
          //
          }
      }
  return ;
}
#endif

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_CommandDecoder(unsigned char ECmd )
{
  unsigned char RetDataLength ;
  unsigned char RetData[255] ;
  unsigned char Command ;
  unsigned char SubCommand ;
  union {
    struct {
      unsigned short Low:8 ;
      unsigned short High:8 ;
    } By ;
    unsigned short All ;
  } TempData ;
  
  union {
    struct {
      unsigned long LL:8 ;
      unsigned long LH:8 ;
      unsigned long HL:8 ;
      unsigned long HH:8 ;
    } By ;
    unsigned long All ;
  } StartAddr ;
  
  unsigned short WLength ;
  unsigned short Size ;
  unsigned char  *cptr ;
  // Athena
  AthenaStepperMotor Temp ;
  // Default 0
  RetDataLength = 0 ;
  RetData[0] = 0 ;
  //
  Command = ECmd ;
  SubCommand = RxAnalsys.member.Data[0] ;
  //----------------------------------------------------------------------------
  if( Command != CmdLCBDeviceData && Command != CmdEUPsMode && Command != CmdSetInclineAction && Command != CmdSetClimbmillStatus && Command != CmdSerialNumber )
      SubCommand = 0 ;    
#ifdef  _LogUCBCommandData_  
  JHTLCMComm_LogUCBCommand(Command,SubCommand) ;
#endif  
  //----------------------------------------------------------------------------
  if( JHTLCBComm_CheckCommandAndLength(Command,SubCommand,RxAnalsys.member.Length) == 1 )
      {
      //------------------------------------------------------------------------
      // When not in Erp mode then if DisableErPOnCmd equal 1 and the console power is on then clear the DisableErPOnCmd to zero
      if( Command != CmdEUPsMode )
          {
          if( CommControlFlag.Bits.DisableErPOnCmd == 1 )
              {
              if( LCBSimulator_GetConsolePowerStatus() == 1 )
                  {
                  CommControlFlag.Bits.DisableErPOnCmd = 0 ;
                  }
              }
          }
      //
      TempData.By.High = RxAnalsys.member.Data[0] ; // High byte
      TempData.By.Low = RxAnalsys.member.Data[1] ; // Low byte
      //------------------------------------------------------------------------
      // Add by Kunlung 20180402 Relase ESTOP Check
      if( LCBSystemProcessStatus.B.sTreadMillEStop == 1 )
          {
          if( (TMReleaseEstopStatus == _RelaseESTOP70 && Command != CmdSetWorkStatus )||(TMReleaseEstopStatus == _RelaseESTOPF7 && Command != CmdSetWorkStatus ))
              {
              if( TMReleaseEstopStatus != 0 )
                  ErrorCodeStatus.bit.EC02B2 = 1 ;
              TMReleaseEstopStatus = 0x00 ;              
              }
          }
      //------------------------------------------------------------------------
      switch( Command )
          {
          case 	CmdInitial      		            ://0x70	
                                                    // 20130816
                                                    ErrorCodeStatus.bit.EC01AC = 0 ;
                                                    ErrorCodeStatus.bit.EC01AF = 0 ;
                                                    ErrorCodeStatus.bit.EC02AB = 0 ;
                                                    ErrorCodeStatus.bit.EC02B4 = 0 ;
                                                    JHTLCBComm_ClearAllErrorMessage(0) ;
                                                    break ;
          case 	CmdGetStatus    		            ://0x71	
                                                    break ;
          case	CmdGetErrorCode 		            ://0x72
                                                    TempData.All = JHTLCBComm_GetErrorCode() ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ;
                                                    RetDataLength = 2 ;
                                                    // log Upload Error
                                                    OldRetErrType = RetErrType ;
                                                    OldRetErrorCode = RetErrorCode ;
                                                    //
                                                    break ;
          case	CmdSkipErrorCode 		            ://0x76	
                                                    JHTLCBComm_SkipErrorCode() ;
                                                    //
                                                    //LCBStatus.bit.McbErrorStatus = 1 ; // for test
                                                    //
                                                    break ;                                                
          case	CmdGetVersion   		            ://0x73	
                                                    // Modify by Kunlung 20200309
                                                    RetData[0] = LCBParameter.LCBSimulatorType; // High byte
                                                    //RetData[1] = 0x04 ;
                                                    switch(LCBParameter.LCBSimulatorType)
                                                        {
                                                        case  _Athena       :
                                                        // 20230314 Add ICR70
                                                        case  _JISICR70LCB  :   
                                                                            RetData[0] = 0xFF; // High byte
                                                                            RetData[1] = 0x00 ;
                                                                            break ;
                                                        case  _JISECB_I     :
                                                                            RetData[0] = 0x0B ; // High byte
                                                                            RetData[1] = 0x04 ;                                                          
                                                                            break ;  
                                                        // Add 20210602
                                                        case  _ITC          :
                                                                            RetData[0] = _ITC ; // High byte
                                                                            RetData[1] = 22 ;                                                          
                                                                            break;
                                                        // Add 20211201
                                                        case  _JISDCLCB     :
                                                                            RetData[0] = 0x04 ; // High byte
                                                                            RetData[1] = 0x01 ;                                                          
                                                                            break ;                                                          
                                                        //-------------------------------------------------------------------------   
                                                        /*default             :  
                                                                            RetData[0] = LCBParameter.LCBSimulatorType; // High byte
                                                                            RetData[1] = 0x04 ;
                                                                            switch(LCBParameter.LCBSimulatorType)
                                                                                {*/
                                                        case  _Delta_I      :  
                                                                            RetData[1] = 10 ;
                                                                            break ;
                                                        case  _Delta_H      :  
                                                                            RetData[1] = 100 ;
                                                                            break ;                                                                                 
                                                        case  _Liteon       :
                                                                            RetData[1] = 108 ;
                                                                            break ;                                                                               
                                                        case  _Delta_I4     :  
                                                                            RetData[1] = 100 ;
                                                                            break ;   
                                                        case  _Delta_I2     :
                                                                            RetData[1] = 3 ;
                                                                            break ;
                                                        case  _LCB1_        :
                                                                            RetData[1] = 103 ;
                                                                            break ;                                                                                                
                                                        case  _LCB2_        :
                                                                            RetData[1] = 104 ;
                                                                            break ;
                                                        case  _Climbmill_   :
                                                                            RetData[1] = 13 ;
                                                                            break ;
                                                        case  _LCBA_        :
                                                                            RetData[1] = 5 ;
                                                                            break ;
                                                        case  _ClimbmillEN_ :
                                                                            RetData[1] = 3 ;
                                                                            break ;
                                                        case  _LiteonJHT    ://Add 20210223
                                                        case  _VAVE_A1      ://Add 20210322  
                                                        case  _VAVE_A2      ://Add 20210322 
                                                        case  _VAVE_A3      ://Add 20210322   
                                                        case  _VAVE_F1      ://Add 20210322   
                                                        case  _ATLCB2_2026  :// 20260126
                                                        case  _CMLCB_2026   :// 20260126
                                                        case  _BKLCBA_2026  :// 20260126  
                                                        case  _PS_PMSM      :// 20260126
                                                        case  _PP_PMSM      :// 20260126
                                                        case  _IMPULSE_LCB  :
                                                                            RetData[1] = 1 ;
                                                                            break ;                                                                                                
                                                       /*                         }
                                                                            break ;*/
                                                        default             :
                                                                            RetData[1] = 0x04 ;
                                                                            break;
                                                        }
                                                    RetDataLength = 2 ;
                                                    //
                                                    break ;
          case	CmdGetRpm         		              ://0x63
                                                    // 20230314 Add ICR70
                                                    TempData.All = LCBParameter.StepRPM ;
                                                    switch( LCBParameter.LCBSimulatorType )
                                                        {
                                                        case  _Climbmill_   :
                                                                            LCBParameter.SPM = (LCBParameter.StepRPM*24)/10 ;
                                                                            break ;
                                                        case  _LCB1_        :
                                                                            if( LCBParameter.MachineType == _MSetpper_ )
                                                                                LCBParameter.SPM = ((LCBParameter.StepRPM*16)+5)/10 ;
                                                                            break ;
                                                        case  _JISICR70LCB  :
                                                                            TempData.All = LCBParameter.CrankRPM ;
                                                                            break ;
                                                        default             :
                                                                            break; 
                                                        }
                                                    /*
                                                    if( LCBParameter.LCBSimulatorType == _Climbmill_ )
                                                        {
                                                        LCBParameter.SPM = (LCBParameter.StepRPM*24)/10 ;
                                                        }
                                                    else
                                                        {
                                                        if( LCBParameter.LCBSimulatorType == _LCB1_ && LCBParameter.MachineType == _MSetpper_ )
                                                            {
                                                            LCBParameter.SPM = ((LCBParameter.StepRPM*16)+5)/10 ;
                                                            }
                                                        }

                                                    TempData.All = LCBParameter.StepRPM ;
                                                    */
                                                    
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ;
                                                    RetDataLength = 2 ;
                                                    break ;         
          case	CmdSetRpmGearRatio    		          ://0x68
                                                    LCBParameter.GearRate = TempData.All ;
                                                    break ;
          case	CmdSetGenMegPolePair  		          :
                                                    LCBParameter.GenMegPolePair = TempData.By.High ;
                                                    break ;
          case	CmdSetLimitRpmForResis 		          :
                                                    // for Climbmill and Stepper
                                                    LCBParameter.LimitRpmForResistance = TempData.All ; 
                                                    if( LCBParameter.LCBSimulatorType == _Climbmill_ )
                                                        {
                                                        LCBParameter.StepRPM = LCBParameter.LimitRpmForResistance ;
                                                        LCBParameter.SPM = (LCBParameter.LimitRpmForResistance*24)/10 ;
                                                        }
                                                    else if( LCBParameter.LCBSimulatorType == _LCB1_ && LCBParameter.MachineType == _MSetpper_ )
                                                        {
                                                        LCBParameter.StepRPM = LCBParameter.LimitRpmForResistance ;
                                                        LCBParameter.SPM = ((LCBParameter.LimitRpmForResistance*16)+5)/10 ;
                                                        }
                                                    break ;
          case	CmdSetLimitRpmForCharge 	          :	     
                                                    LCBParameter.LimitRpmForCharge = TempData.All ; 
                                                    break ;
          case	CmdSetMachineType  		              :
                                                    LCBParameter.MachineType = TempData.By.High ;
                                                    LCBSystemProcessStatus.B.sSetMachineType = 1 ;
                                                    /*
                                                    #define		_MBikeEP_														0
                                                    #define		_MSetpper_													1
                                                    #define		_MRamp_IncluneEP_										2
                                                    #define		_MSwingInclineEP_										3
                                                    #define		_MTreadmill_												4
                                                    #define		_MClimbmill_												5
                                                    */
                                                    switch(LCBParameter.LCBSimulatorType) 
                                                        {
                                                        case _LCB1_       :
                                                                          // Add by Kunlung to reslove 02AB issue
                                                                          if( LCBParameter.MachineType != _MBikeEP_ && LCBParameter.MachineType != _MSetpper_ )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;
                                                                          //
                                                        case _LCBA_       :
                                                        case _LCBA_M_     :
                                                        case _LCB1x_      :
                                                        case _BKLCBA_2026 :// 20260126   
                                                                          if( LCBParameter.MachineType != _MBikeEP_ )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;        
                                                        case _LCB3_       :
                                                                          if( LCBParameter.MachineType != _MSwingInclineEP_ )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;
                                                        case _LCB2_       :
                                                        case _ATLCB2_2026 :// 20260126  
                                                                          if( LCBParameter.MachineType != _MRamp_IncluneEP_ && LCBParameter.MachineType != _MSwingInclineEP_ )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;
                                                        case _Climbmill_  :
                                                        case _ClimbmillEN_:
                                                        case _ClimbmillR_ :
                                                        case _CMLCB_2026  :// 20260126  
                                                        case _IMPULSE_LCB :  
                                                                          if( LCBParameter.MachineType != _MClimbmill_ )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;
                                                        case _DCI_        :
                                                        case _TopTek_     :
                                                        case _Delta_I     :
                                                        case _Delta_H     :
                                                        case _Delta_I1    :
                                                        case _Delta_I2    :
                                                        case _Delta_I3    :
                                                        case _Delta_I4    :
                                                        case _Liteon      :
                                                        case _LiteonJHT   ://Add 20210223
                                                        case _VAVE_A1     ://Add 20210322  
                                                        case _VAVE_A2     ://Add 20210322 
                                                        case _VAVE_A3     ://Add 20210322   
                                                        case _VAVE_F1     ://Add 20210322   
                                                        case _PS_PMSM     :// 20260126  
                                                        case _PP_PMSM     :// 20260126    
                                                                          if( LCBParameter.MachineType != _MTreadmill_ )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;
                                                        // Add by Kunlung 20200309                                                                          
                                                        case _Athena      :
                                                        // Add by 20230314
                                                        //case _JISICR70LCB :
                                                                          if( LCBParameter.MachineType != _MAtheraIC )
                                                                              ErrorCodeStatus.bit.EC02AB = 1 ;
                                                                          break ;           
                                                        //
                                                        default           :
                                                                          break ;
                                                        }
                                                    break ;
          case	CmdSetResistanceTypeAndResistance   :	
                                                    LCBParameter.ResistanceType.Full = TempData.All ;
                                                    break ;                                              
          case	CmdUpdateProgram		                ://0x75	
                                                    //
                                                    RetData[0] = 'N' ;
                                                    RetDataLength = 1 ;
                                                    //	                                                    
                                                    break ;                                                
          case	CmdLCBDeviceData		                ://0x50
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 Load Cell Get Version
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetData[0] = 0 ;      //undefined data
                                                        RetData[1] = 1 ;      //TBD-What is this
                                                        RetData[2] = _ITC ;   //LCB Type
                                                        RetData[3] = 0 ;      //TBD-What is this
                                                        RetData[4] = 22 ;     //Major Version
                                                        RetData[5] = 1 ;      //Minor Version
                                                        RetDataLength = 6 ;
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
            
                                                    RetData[0]=SubCommand ;
                                                    switch( SubCommand )
                                                        {
                                                                                   // R   T
                                                        case GetEEPromMemorySizes :// 3		2
                                                                                  TempData.All = 0x0800 ;
                                                                                  RetData[1] = TempData.By.High ; // High byte
                                                                                  RetData[2] = TempData.By.Low ;
                                                                                  RetDataLength = 3 ;
                                                                                  break ;
                                                        case GetEEPromMemoryData  ://	3		2
                                                                                  TempData.By.High = RxAnalsys.member.Data[1] ; // High byte
                                                                                  TempData.By.Low = RxAnalsys.member.Data[2] ; // Low byte
                                                                                  //EE93CXX_ReadDataFromEeprom(TempData.All,16,&RetData[1]) ;
                                                                                  RetDataLength = 17 ;
                                                                                  break ;
                                                        case GetECBCurrent	  :// 3   4
                                                                                  //TempData.All = FeedBackAdcData.ElectroMagnetCurrent ;
                                                                                  RetData[1] = TempData.By.High ; // High byte
                                                                                  RetData[2] = TempData.By.Low ;
                                                                                  //TempData.All = FeedBackAdcData.ADC_ElectroMagnetCurrent ;
                                                                                  RetData[3] = TempData.By.High ; // High byte
                                                                                  RetData[4] = TempData.By.Low ;
                                                                                  RetDataLength = 5 ;
                                                                                  break ;
                                                        case GetDCBusStatus       :// 3   4
                                                                                  //TempData.All = FeedBackAdcData.GeneratorVoltage ;
                                                                                  RetData[1] = TempData.By.High ; // High byte
                                                                                  RetData[2] = TempData.By.Low ;
                                                                                  //TempData.All = FeedBackAdcData.GeneratorCurrent ;
                                                                                  RetData[3] = TempData.By.High ; // High byte
                                                                                  RetData[4] = TempData.By.Low ;
                                                                                  RetDataLength = 5 ;
                                                                                  break ;
                                                        case GetLCBVersion        :
                                                                                  for( RetDataLength = 0 ; RetDataLength < 8 ; RetDataLength++ )
                                                                                      {
                                                                                      RetData[RetDataLength+1] = 0 ;//LCBVersion.VerData[RetDataLength] ;
                                                                                      }
                                                                                  /*
                                                                                  //Add 20210223
                                                                                  if( LCBParameter.LCBSimulatorType == _LiteonJHT )
                                                                                      {
                                                                                      LCBVersion.Ver.MCU_A_Loader = 1 ;
                                                                                      LCBVersion.Ver.MCU_A_API = 666 ;
                                                                                      RetData[1] = LCBVersion.VerData[1] ;
                                                                                      RetData[2] = LCBVersion.VerData[0] ;                                                                                      
                                                                                      RetData[3] = LCBVersion.VerData[3] ;
                                                                                      RetData[4] = LCBVersion.VerData[2] ;
                                                                                      }
                                                                                  */
                                                                                  //20210322
                                                                                  switch(LCBParameter.LCBSimulatorType)
                                                                                      {
                                                                                      case  _LiteonJHT  :
                                                                                      case  _VAVE_A1    :
                                                                                      case  _VAVE_A2    :
                                                                                      case  _VAVE_A3    :
                                                                                      case  _VAVE_F1    :  
                                                                                                        LCBVersion.Ver.MCU_A_Loader = 1 ;
                                                                                                        LCBVersion.Ver.MCU_A_API = 666 ;
                                                                                                        RetData[1] = LCBVersion.VerData[1] ;
                                                                                                        RetData[2] = LCBVersion.VerData[0] ;                                                                                      
                                                                                                        RetData[3] = LCBVersion.VerData[3] ;
                                                                                                        RetData[4] = LCBVersion.VerData[2] ;
                                                                                                        break ;
                                                                                      }
                                                                                  //
                                                                                  //
                                                                                  RetDataLength = 9 ;
                                                                                  break ;
                                                        case GetEStopCapacitance  :// 3   4
                                                                                  //TempData.All = FeedBackAdcData.GeneratorVoltage ;
                                                                                  RetData[1] = TempData.By.High ; // High byte
                                                                                  RetData[2] = TempData.By.Low ;
                                                                                  //TempData.All = LCBParameter.ClimbmillBrakePressureValue ;
                                                                                  RetData[3] = TempData.By.High ; // High byte
                                                                                  RetData[4] = TempData.By.Low ;
                                                                                  RetDataLength = 5 ;
                                                                                  break ;
                                                        case ReadByteFlashData    :
                                                                                  // 6 + 50
                                                                                  ///*
                                                                                  //
                                                                                  StartAddr.By.LL = RxAnalsys.member.Data[1] ;
                                                                                  StartAddr.By.LH = RxAnalsys.member.Data[2] ;
                                                                                  StartAddr.By.HL = RxAnalsys.member.Data[3] ;
                                                                                  StartAddr.By.HH = RxAnalsys.member.Data[4] ;
                                                                                  //
                                                                                  RetData[1] = 0 ;
                                                                                  RetData[2] = RxAnalsys.member.Data[5] ;
                                                                                  cptr = (unsigned char  *)StartAddr.All ;
                                                                                  for( WLength = 0 ; WLength < RxAnalsys.member.Data[5] ; WLength++ )
                                                                                      {
                                                                                      if( (unsigned long)(cptr+WLength) >= 0x08000000 && (unsigned long)(cptr+WLength) <= 0x0801FFFF )
                                                                                          RetData[3+WLength] = *(cptr+WLength) ;
                                                                                      else
                                                                                          {
                                                                                          RetData[1] = 2 ;
                                                                                          RetData[2] = WLength ;
                                                                                          break ;
                                                                                          }
                                                                                      }
                                                                                  RetDataLength = 3 + WLength ;
                                                                                  //*/
                                                                                  break ;
                                                        // Add Athera Version Command                                                                                  
                                                        case  GetAthenaVerion     :
                                                                                  // 20230314 Add ICR70
                                                                                  /*
                                                                                  RetData[1]=0x01 ;
                                                                                  RetData[2]=_Athena ;
                                                                                  RetData[3]=0x00 ;
                                                                                  RetData[4]=0x01 ;
                                                                                  RetData[5]=0x03 ;
                                                                                  RetDataLength = 6 ;
                                                                                  */
                                                                                  if( LCBParameter.LCBSimulatorType == _Athena || LCBParameter.LCBSimulatorType == _JISICR70LCB )
                                                                                      {
                                                                                      RetData[1]=0x01 ;
                                                                                      RetData[2]=LCBParameter.LCBSimulatorType ; 
                                                                                      RetData[3]=0x00 ;
                                                                                      RetData[4]=0x01 ;
                                                                                      RetData[5]=0x03 ;
                                                                                      RetDataLength = 6 ;
                                                                                      }
                                                                                  else
                                                                                      ErrorCodeStatus.bit.EB0441 = 1 ;
                                                                                  break;
                                                        // Add 20210317 for JIS LCB Type 0C  
                                                        case SavePWM              :
                                                                                  TempData.By.High = RxAnalsys.member.Data[2] ;
                                                                                  TempData.By.Low = RxAnalsys.member.Data[3] ;
                                                                                  if( RxAnalsys.member.Data[1] > 0 && RxAnalsys.member.Data[1] < 5 )
                                                                                      {
                                                                                      LCBParameter.JISLCBPWM[(RxAnalsys.member.Data[1]-1)] = TempData.All ;
                                                                                      RetData[1]=0x00 ; //OK
                                                                                      }
                                                                                  else
                                                                                      RetData[1]=0x01 ; // Fail
                                                                                  
                                                                                  RetDataLength = 2 ;
                                                                                  break ;
                                                        case ReadPWM              :
                                                                                  if( RxAnalsys.member.Data[1] > 0 && RxAnalsys.member.Data[1] < 5 )
                                                                                      TempData.All = LCBParameter.JISLCBPWM[(RxAnalsys.member.Data[1]-1)] ;
                                                                                  else
                                                                                      TempData.All = LCBParameter.JISLCBPWM[0] ;
                                                                                  
                                                                                  RetData[1]=TempData.By.High ;
                                                                                  RetData[2]=TempData.By.Low ;
                                                                                  RetDataLength = 3 ;
                                                                                  break ;
                                                        //------------------------------------------------
#if 0                                                   // 20230522
                                                        case SetMCBParameter      ://0x90  //  3+n   2
                                                                                  break ;
                                                        case GetMCBParameter      ://0x91  //  2     3+n
                                                                                  break ;
#endif                                                                                  
                                                        //------------------------------------------------
                                                        }
                                                    break ;                                                
          case	CmdEUPsMode			                    :
                                                    RetData[0] = 0 ;

                                                    if( LCBSimulator_GetDCPluginStatus() == 1 )
                                                        {
                                                        if( RxAnalsys.member.Data[0] == 0xFF )
                                                            {
                                                            if( CommControlFlag.Bits.DisableErPOnCmd == 1 )
                                                                RetData[0] = 0 ;  
                                                            else
                                                                {
                                                                CommControlFlag.Bits.CheckErPCommandReturn = 1 ;
                                                                RetData[0] = 1 ;
                                                                }
                                                            }
                                                        else
                                                            {
                                                            if( RxAnalsys.member.Data[0] == 0x00 )
                                                                {
                                                                LCBSimulator_SetErPStatus(0) ;
                                                                RetData[0] = 1 ;
                                                                }
                                                            }
                                                        }
                                                    else 
                                                        ErrorCodeStatus.bit.EB0441 = 1 ;

                                                    RetDataLength = 1 ;
                                                    //
                                                    CommControlFlag.Bits.DisableErPOnCmd = 1 ;
                                                    JHTLCBComm_ResetTimeoutStatus() ;
                                                    //
                                                    break ;                                                
          case	CmdGetBatteryStatus		              :
                                                    RetData[0] = LCBParameter.DCBusVoltage	;
                                                    RetData[1] = LCBParameter.BatteryVoltage ; 
                                                    RetDataLength = 2 ;
                                                    break ;                                                
          case	CmdSetPowerOff 			                :			
                                                    LCBParameter.PowerOffTime = TempData.By.High ;
                                                    break ;                                                
          case	CmdSetWatts			                    :
                                                    LCBParameter.TargetWatts = TempData.All ;
                                                    break ;  
          case	CmdSetBatteryCharge  		            :			
                                                    LCBParameter.BatteryChargePercent = TempData.By.High ;
                                                    break ;  
          case	CmdCalibrate 			                  :
                                                    LCBParameter.InclineControl.Bits.AutoCalibrate = 1 ;
                                                    break ;                                                      
          case	CmdSetInclineStroke		              :
                                                    LCBParameter.InclineStroke = TempData.All ;
                                                    break ;                                                      
          case 	CmdSetInclinePercent  		          :
                                                    LCBParameter.InclineTargetPercent = TempData.All ;
                                                    break ;                                                
          case	CmdGetInclinePercent  		          :
                                                    TempData.All = LCBParameter.InclinePercent ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ;
                                                    RetDataLength = 2 ;
                                                    break ;                                                      
          case	CmdSetInclineAction		              :
                                                    switch(RxAnalsys.member.Data[0])
                                                        {
                                                        case  ManualInclineUp   :
                                                                                LCBParameter.InclineControl.Bits.ManualUp = 1 ;
                                                                                break ;
                                                        case  ManualInclineDown :
                                                                                LCBParameter.InclineControl.Bits.ManualDown = 1 ;
                                                                                break ;
                                                        case  ManualInclineStop :
                                                                                LCBParameter.InclineControl.Bits.ManualStop = 1 ;                                                                                
                                                                                break ;
                                                        default                 :
                                                                                ErrorCodeStatus.bit.EB0442 = 1 ;
                                                                                break ;
                                                        }
                                                    break ;  
          case 	CmdSetInclineLocation 		          ://0xF6  // 	2		0		
                                                    if( LCBParameter.InclineControl.Bits.Manual == 0 )
                                                        LCBParameter.InclineTargetLocation.Full = TempData.All ;
                                                    else // Recoder Incline Control Flow Error
                                                        {
                                                        }
                                                    break ;                                                     
          case 	CmdGetInclineLocation 		          ://0xFA  // 	0		2
                                                    TempData.All = LCBParameter.InclineLocation.Position ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ;
                                                    RetDataLength = 2 ;            
                                                    break ;            
          case	CmdSetGapVrCalibrateIncline	        :
                                                    LCBParameter.InclineGap1.Full = TempData.All ;
                                                    break ;            
          case 	CmdSetGapVrCalibrateIncline2 	      :
                                                    LCBParameter.InclineGap1.Full = TempData.All ;
                                                    break ;                                                      
          case  CmdSetPwm                           :
                                                    LCBParameter.TargetPWM = TempData.All ;
                                                    break ;
          case  CmdSetEMagnetCurrent                :
                                                    LCBParameter.TargetEMCurrent = TempData.All ;
                                                    break ;
          case  CmdSetBeginBatteryCharge            :
                                                    // Set the battery charge current of no resistance
                                                    LCBParameter.ChargeCurrnetForNoResistance = TempData.All ;
                                                    break ;
          case  CmdGetBatteryCapacity               :
                                                    RetData[0] = 0 ;
                                                    RetData[1] = LCBParameter.BatteryCapacity ; 
                                                    RetDataLength = 2 ;
                                                    break ;
          case  CmdSetClimbmillStatus	              :
                                                    switch(RxAnalsys.member.Data[0])
                                                        {
                                                        case  LCB_Workout_Start         :
                                                                                        CommControlFlag.Bits.ClimbmillStart = 1 ;
                                                                                        break ;
                                                        case  LCB_Workout_Stop          :
                                                                                        CommControlFlag.Bits.ClimbmillStop = 1 ;
                                                                                        break ;
                                                        case  LCB_EStopAndIR_Disable    :
                                                                                        LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable = 0 ;
                                                                                        LCBParameter.ClimbmillStatus.Bits.IRSensorEnable = 0 ;
                                                                                        LCBSimulator_SetsSaveEEPROMStatus(1) ;
                                                                                        break ;
                                                        case  LCB_EStopAndIR_Enable     :
                                                                                        LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable = 1 ;
                                                                                        LCBParameter.ClimbmillStatus.Bits.IRSensorEnable = 1 ;
                                                                                        LCBSimulator_SetsSaveEEPROMStatus(1) ;
                                                                                        break ;
                                                        case  LCB_EStopDisable_IREnable :
                                                                                        LCBParameter.ClimbmillStatus.Bits.IRSensorEnable = 1 ;
                                                                                        LCBSimulator_SetsSaveEEPROMStatus(1) ;
                                                                                        break ;
                                                        case  LCB_EStopEnable_IRDisable :
                                                                                        LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable = 1 ;
                                                                                        LCBSimulator_SetsSaveEEPROMStatus(1) ;
                                                                                        break ;                                                                                
                                                        case  LCB_EStop_Unlock          :
                                                                                        CommControlFlag.Bits.ClimbmillUnlockSafetyKey = 1 ;
                                                                                        break ;                        
                                                        case  LCB_NoUserDectectDisable  :
                                                                                        break ;
                                                        case  LCB_NoUserDectectEnable   :
                                                                                        break ;
                                                        default                         :
                                                                                        ErrorCodeStatus.bit.EB0442 = 1 ;
                                                                                        break ;
                                                        }                                                 
                                                    break ;
          case  CmdGetClimbmillStatus	              :                                                    
                                                    if( LCBParameter.LCBSimulatorType == _ClimbmillEN_ || LCBParameter.LCBSimulatorType == _ClimbmillR_ )
                                                        {
                                                        // Bit 8 : the status of UCB Emergency
                                                        // Bit 9 : UCB Emergency is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                        // Bit 10: the status of Handrail Emergency
                                                        // Bit 11: Handrail Emergency is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                        // Bit 12: No person during using. ( 0: No, 1: Yes )
                                                        // Bit 13: 
                                                        // Bit 14: 
                                                        // Bit 15: 
                                                        RetData[0] = LCBParameter.ClimbmillExtendStatus.Full ;
                                                        // Bit 0 : the speed control ( 1: start, 0:stop)
                                                        // Bit 1 : 
                                                        // Bit 2 : IR Sensor use ( 0: not use, 1: use)
                                                        // Bit 3 : the status of the IR Sensor is response at once
                                                        // Bit 4 : IR Sensor is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                        // Bit 5 : E-STOP use ( 0: not use, 1: use)
                                                        // Bit 6 : the status of the E-STOP is response at once
                                                        // Bit 7 : E-STOP is once touched ( 0 : not touch , 1 : touch but not unlock)                                                        
                                                        RetData[1] = LCBParameter.ClimbmillStatus.Full ;
                                                        RetDataLength = 2 ;
                                                        }
                                                    else
                                                        {
                                                        RetDataLength = 1 ;
                                                        // Bit 0 : the speed control ( 1: start, 0:stop)
                                                        // Bit 1 : 
                                                        // Bit 2 : IR Sensor use ( 0: not use, 1: use)
                                                        // Bit 3 : the status of the IR Sensor is response at once
                                                        // Bit 4 : IR Sensor is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                        // Bit 5 : E-STOP use ( 0: not use, 1: use)
                                                        // Bit 6 : the status of the E-STOP is response at once
                                                        // Bit 7 : E-STOP is once touched ( 0 : not touch , 1 : touch but not unlock)
                                                        RetData[0] = LCBParameter.ClimbmillStatus.Full ;                                                         
                                                        }
                                                    break ;
          case  CmdSetEStopActionValue              :     
                                                    LCBParameter.ClimbmillEStopValut = RxAnalsys.member.Data[0] ;
                                                    break ;
          case	CmdSetMotorRpmAndReturn		          ://0xF0	//	2		2				
                                                    LCBParameter.MotorTargetSpeed.Full = TempData.All ;
                                                    TempData.All = LCBParameter.MotorSpeed.RPM ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;
                                                    break ;            
          case	CmdSetMotorRpm			                ://0xF1	//	2		0	
                                                    LCBParameter.MotorTargetSpeed.Full = TempData.All ;
                                                    break ;            
          case	CmdSetPwmAddStep		                ://0xF2	//	2		0	
                                                    break ;            
          case	CmdSetPwmDecStep		                ://0xF3	//	2		0	
                                                    break ;            
          case	CmdSetPwmStopStep		                ://0xF4	//	1		0	
                                                    break ;        
          case 	CmdSetWorkStatus      		          ://0xF7	//	1		0			
                                                    LCBSimulator_SetTreadmillOperation(TempData.By.High) ;
                                                    // Add by Kunlung 20180402 Relase ESTOP Check
                                                    if( LCBSystemProcessStatus.B.sTreadMillEStop == 1 )
                                                        {
                                                        if( TempData.By.High == 0 && TMReleaseEstopStatus == _RelaseESTOP70 )
                                                            TMReleaseEstopStatus = _RelaseESTOPF7 ;
                                                        else
                                                            {
                                                            if( TempData.By.High == 1 && TMReleaseEstopStatus == _RelaseESTOPF7 )
                                                                TMReleaseEstopStatus = _RelaseESTOP ;    
                                                            else
                                                                TMReleaseEstopStatus = 0 ;
                                                            }
                                                        }
                                                    else
                                                        TMReleaseEstopStatus = 0 ;
                                                    //--------------------------------------------------------------------------------------------------
                                                    break ;            
          case 	CmdGetRollerRpm       		          ://0xF8	//	0		2	
                                                    TempData.All = LCBParameter.MotorSpeed.RPM ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;
                                                    break ;            
          case 	CmdGetMotorRpm        		          ://0xF9	//	0		2
                                                    TempData.All = LCBParameter.MotorSpeed.RPM ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;
                                                    break ;             
          case 	CmdSetCompensationVoltage	          ://0xFE	//	2		0     0~65535 x 0.01V
                                                    break ;            
          case	CmdSetDriveMotorHP		              ://0x90	//	2		0     0~65535 x 1Watt
                                                    LCBParameter.MotorHP = TempData.All ;
                                                    break ;            
          case  CmdGetMotorType                     ://0x91  //  0   1
                                                    RetData[0] = LCBParameter.MotorType ;
                                                    RetDataLength = 1 ;
                                                    break ;            
          case  CmdGetMaxDataLength                 ://0x92  //  0   1
                                                    break ;            
          case  CmdSetMotorPowerOff                 ://0x93  //  1   0
                                                    break ;            
          case  CmdDCIForceInclineOperation         ://0x94  //  1   0     
                                                    break ;            
          case  CmdGetTreadmillInUse                ://0x95  //  0   1     0xFF: Use, 0x55: not Use
                                                    RetDataLength = 1 ;
                                                    RetData[0] = LCBParameter.TreadmillUseStatus ;
                                                    break ;            
          case  CmdGetMainMotorInfo                 ://0x96
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 Load Cell Get Machine Data
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetDataLength = 14 ;
                                                        // Cable Tension (Newtons) Example, if the head plate assembly is 6620 grams, typically the cable tension should not be higher than 6620 grams [ 65 Newtons  ]
                                                        if( LCBParameter.CableTension == 0 )
                                                            LCBParameter.CableTension = 1000 ;    // for Default
                                                        TempData.All = LCBParameter.CableTension ;
                                                        RetData[0] = TempData.By.High ; 
                                                        RetData[1] = TempData.By.Low ;                                                         
                                                        RetData[2] = 0 ;
                                                        RetData[3] = 0 ;
                                                        RetData[4] = 0 ;
                                                        RetData[5] = 0 ;
                                                        RetData[6] = 0 ;
                                                        RetData[7] = 0 ;
                                                        RetData[8] = 0 ;
                                                        RetData[9] = 0 ;   
                                                        RetData[10] = 0 ;
                                                        RetData[11] = 0 ;
                                                        RetData[12] = 0 ;
                                                        RetData[13] = 0 ;                                                         
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    //0x96   //  0   10    0+1: Freq.(0.01Hz)
                                                             //            2+3: Current (0.1A)
                                                             //            4+5: Voltage (0.1V)
                                                             //            6+7: DC Bus Voltage (0.1V)
                                                             //            8+9: IGBT Temp (0.1C)
                                                    RetDataLength = 10 ;
                                                    RetData[0] = 0 ;
                                                    RetData[1] = 0 ;
                                                    RetData[2] = 0 ;
                                                    RetData[3] = 0 ;
                                                    RetData[4] = 0 ;
                                                    RetData[5] = 0xDC ;
                                                    RetData[6] = 0 ;
                                                    RetData[7] = 0xDC ;
                                                    RetData[8] = 0 ;
                                                    RetData[9] = 0 ;
                                                    break ;            
          case  CmdGetInclineRange                  ://0xA0 //  0   4
                                                    break ;            
          case  CmdSetInclineRange                  ://0xA1 //  4   0
                                                    break ;            
          case  CmdGetRpmSpeedRange                 ://0xA6 //  0   4
                                                    break ;            
          case  CmdSetRpmSpeedRange                 ://0xA7 //  8   0
                                                    break ;            
          case  CmdUniversal                        ://0xAA //  n   2
                                                    break ;            
          case  CmdManualCalibration                ://0xAB //  0   0                                                      
                                                    break ;
          case 	CmdSetEcbInit     		              ://0x61 // 	0   0
                                                    LCBParameter.ECBPosition = 0 ;
                                                    // 20230314
                                                    if( LCBParameter.LCBSimulatorType == _JISICR70LCB )
                                                        {
                                                        LCBParameter.ResistanceLevel = 1 ;
                                                        LCBParameter.ECBCount = ICR70ECBcount[0] ;
                                                        LCBParameter.ECBPosition = LCBParameter.ECBCount ;
                                                        }
                                                    break ;
          case 	CmdSetEcbLocation 		              ://0x62 // 	2   0
                                                    LCBParameter.ECBPosition = TempData.All ;
                                                    //20230314
                                                    if( LCBParameter.LCBSimulatorType == _JISICR70LCB )
                                                        {
                                                        if( LCBParameter.ECBPosition > 22510 )
                                                            LCBParameter.ECBPosition = 22510 ;
                                                        else if( LCBParameter.ECBPosition < 30 )
                                                            LCBParameter.ECBPosition = 30 ;
                                                        LCBParameter.ECBCount = LCBParameter.ECBPosition ;
                                                        }
                                                    break ;
          case 	CmdGetEcbLocation 		              ://0x64 // 	0   2                                                    
                                                    // 20230314
                                                    if( LCBParameter.LCBSimulatorType == _JISICR70LCB )
                                                        {
                                                        if( LCBParameter.ResistanceLevel > 25 )
                                                            LCBParameter.ResistanceLevel = 25 ;
                                                        else if( LCBParameter.ResistanceLevel < 1 )
                                                            LCBParameter.ResistanceLevel = 1 ;
                                                        LCBParameter.ECBCount = ICR70ECBcount[(LCBParameter.ResistanceLevel-1)] ;
                                                        LCBParameter.ECBPosition = LCBParameter.ECBCount ;
                                                        }
                                                    TempData.All = LCBParameter.ECBCount ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;            
                                                    break ;
          case 	CmdSetZeroResisWhenInclineWorking   ://0x6F //	1   0	
                                                    break ;
          //----- JIS A30                                                     
          case	CmdSetEcbAction			                ://0x82 // 	1   0
                                                    switch(RxAnalsys.member.Data[0])
                                                        {
                                                        case  EcbActionUP   :
                                                                            LCBParameter.ECBActionControl.Bits.Up = 1 ;
                                                                            LCBParameter.ECBActionControl.Bits.Down = 0 ;
                                                                            LCBParameter.ECBActionControl.Bits.Stop = 0 ;
                                                                            break ;
                                                        case  EcbActionDown :
                                                                            LCBParameter.ECBActionControl.Bits.Up = 0 ;
                                                                            LCBParameter.ECBActionControl.Bits.Down = 1 ;
                                                                            LCBParameter.ECBActionControl.Bits.Stop = 0 ;                                                          
                                                                            break ;
                                                        case  EcbActionStop :
                                                                            LCBParameter.ECBActionControl.Bits.Up = 0 ;
                                                                            LCBParameter.ECBActionControl.Bits.Down = 0 ;
                                                                            LCBParameter.ECBActionControl.Bits.Stop = 1 ;                                                          
                                                                            break ;
                                                        default             :
                                                                            break ; 
                                                        }
                                                    break ;
          case	CmdGetEcbStatus			                ://0x83 // 	0   1
                                                    RetData[0] = LCBParameter.ECBStatus.Full ; // High byte
                                                    RetDataLength = 1 ;             
                                                    break ;
          case	CmdGetEcbCount			                ://0x84 // 	0   2
                                                    // 20230314
                                                    if( LCBParameter.LCBSimulatorType == _JISICR70LCB )
                                                        {
                                                        if( LCBParameter.ResistanceLevel > 25 )
                                                            LCBParameter.ResistanceLevel = 25 ;
                                                        else if( LCBParameter.ResistanceLevel < 1 )
                                                            LCBParameter.ResistanceLevel = 1 ;
                                                        LCBParameter.ECBCount = ICR70ECBcount[(LCBParameter.ResistanceLevel-1)] ;
                                                        LCBParameter.ECBPosition = LCBParameter.ECBCount ;
                                                        }            
                                                    TempData.All = LCBParameter.ECBCount ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;               
                                                    break ;	
          //-------                                                    
          case	CmdSpecialExtCommand		            ://0x77 //	1   1		
                                                    break ;	
          case  CmdGetDCIVersion                    ://0x78 //  0   27
                                                    break ;	
          case  CmdGetDCIEnvironment                ://0x79 //  0   84
                                                    break ;	
          case  CmdSetConsolePower                  ://0x7B //  2   0       0~65535 x 0.1Watt
                                                    LCBParameter.ConsolePowerConsumption = TempData.All ;
                                                    break ;	
          // Climbmill EN2017  
          case CmdGetBrakePressureValue             ://0x8D  //  0     2
                                                    if( LCBParameter.ClimbmillStatus.Bits.Start == 1 )
                                                        TempData.All = LCBParameter.ClimbmillBrakeONPressureValue ;
                                                    else
                                                        TempData.All = LCBParameter.ClimbmillBrakeOFFPressureValue ;  // Default Value
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;
                                                    break ;
          case CmdGetWatts                          ://0x8E  //  0     2
                                                    // 20230314 Add ICR70
                                                    /*
                                                    LCBParameter.Watts = LCBParameter.TargetWatts ; // for test
                                                    TempData.All = LCBParameter.Watts ;
                                                    */
                                                    if( LCBParameter.LCBSimulatorType == _JISICR70LCB )
                                                        TempData.All = LCBParameter.Watts/10 ; 
                                                    else
                                                        {
                                                        LCBParameter.Watts = LCBParameter.TargetWatts ; 
                                                        TempData.All = LCBParameter.Watts ;
                                                        }
                                                    //-----------------------------------------------------------------------------
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;
                                                    break ;  
          case CmdGetSPM                            ://0x4B  //  2     0   // unit: x0.1spm , ex: 10 = 1.0 spm  
                                                    TempData.All = LCBParameter.SPM ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    RetDataLength = 2 ;
                                                    break ;            
          case CmdSetSPM                            ://0x4A
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 0x4A Get Smart Rep
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        if( ITCStatus.bit.Home == 0 && ITCStatus.bit.Quarter_rep == 0 )
                                                            {
                                                            RetDataLength = 4 ;
                                                            RetData[0] = 0 ;
                                                            RetData[1] = 0 ;
                                                            RetData[2] = LCBParameter.RepData.Data[33] ;
                                                            RetData[3] = LCBParameter.RepData.Data[32] ;
                                                            }
                                                        else
                                                            {
                                                            for( RetDataLength = 0 ; RetDataLength < 34 ; RetDataLength+=2 )
                                                                {  
                                                                RetData[RetDataLength] = LCBParameter.RepData.Data[RetDataLength+1] ;
                                                                RetData[RetDataLength+1] = LCBParameter.RepData.Data[RetDataLength] ;
                                                                }
                                                            }
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    //0x4A  //  0     2   // unit: x0.1spm , ex: 100 = 10.0 spm   
                                                    LCBParameter.TargetSPM = TempData.All ;
                                                    LCBParameter.SPM = LCBParameter.TargetSPM ; // for Test
                                                    break ;
          //------------------------------------------------------------------------------
#ifdef  _SupportSerialNumberCommand                                                    
          case  CmdSerialNumber                     :// 0x7D  
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetDataLength = 3 ;
                                                        RetData[0] = SubCommand ; // High byte
                                                        RetData[1] = 0 ; // 0:Sucess, 1:Failure, 2:not enouh space
                                                        RetData[2] = 0 ;
                                                        switch(SubCommand)
                                                            {
                                                            case  CmdSetITCConsoleSN  ://0x01  //  3+N  3
                                                                                      if( RxAnalsys.member.Data[2] < _ITCMaxLengthOfSerialNumber )
                                                                                          {
                                                                                          for(RetDataLength = 0; RetDataLength < RxAnalsys.member.Data[2] ; RetDataLength++)
                                                                                              ITC_ConsoleSN[RetDataLength] = RxAnalsys.member.Data[3+RetDataLength]; 
                                                                                          RetDataLength = 3 ;
                                                                                          //LCBSimulator_SetSerialNumber(1) ;
                                                                                          }
                                                                                      else
                                                                                          RetData[1] = 2 ;
                                                                                      break ;
                                                            case  CmdGetITCConsoleSN  ://0x02  //  3    3+N
                                                                                      RetData[2] = _ITCMaxLengthOfSerialNumber ;
                                                                                      for(RetDataLength = 0; RetDataLength < _ITCMaxLengthOfSerialNumber ; RetDataLength++)                                                                                        
                                                                                          RetData[3+RetDataLength]=ITC_ConsoleSN[RetDataLength] ; 
                                                                                      RetDataLength = 3 + _ITCMaxLengthOfSerialNumber ;                                                                                        
                                                                                      break ;
                                                            case  CmdSetITCFrameSN    ://0x05  //  3+N  3
                                                                                      if( RxAnalsys.member.Data[2] < _ITCMaxLengthOfSerialNumber )
                                                                                          {
                                                                                          for(RetDataLength = 0; RetDataLength < RxAnalsys.member.Data[2] ; RetDataLength++)
                                                                                              ITC_FrameSN[RetDataLength] = RxAnalsys.member.Data[3+RetDataLength]; 
                                                                                          RetDataLength = 3 ;
                                                                                          //LCBSimulator_SetSerialNumber(1) ;
                                                                                          }
                                                                                      else
                                                                                          RetData[1] = 2 ;
                                                                                      break ;
                                                            case  CmdGetITCFrameSN    ://0x06  //  3    3+N
                                                                                      RetData[2] = _ITCMaxLengthOfSerialNumber ;
                                                                                      for(RetDataLength = 0; RetDataLength < _ITCMaxLengthOfSerialNumber ; RetDataLength++)                                                                                        
                                                                                          RetData[3+RetDataLength]=ITC_FrameSN[RetDataLength] ; 
                                                                                      RetDataLength = 3 + _ITCMaxLengthOfSerialNumber ;                                                                                      
                                                                                      break ;
                                                            default :
                                                                    break ;
                                                            }
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    RetDataLength = 3 ;
                                                    RetData[0]=SubCommand ;
                                                    RetData[1] = 0 ;
                                                    RetData[2] = 0 ;
                                                    switch( SubCommand )
                                                        {
                                                        case CmdEraseSerialNumber   :// 0x00  //  3     3  
                                                                                    break ;
                                                        case CmdGetMemoryLength     :// 0x03  //  3     3
                                                                                    RetData[2] = _MaxLengthOfSerialNumber ;
                                                                                    break ;                                                         
                                                        case CmdReadSerialNumber    :// 0x02  //  3     3+N     N <= 252
                                                                                    RetData[2] = MachineSerialNumberLength ;
                                                                                    for(RetDataLength = 0; RetDataLength < MachineSerialNumberLength ; RetDataLength++)
                                                                                        RetData[3+RetDataLength]=MachineSerialNumber[RetDataLength] ; 
                                                                                    RetDataLength = 3 + MachineSerialNumberLength ;
                                                                                    break ;
                                                        case CmdWriteSerialNumber   :// 0x01  //  3+N   3       N <= 252
                                                                                    MachineSerialNumberLength = RxAnalsys.member.Data[2] ;
                                                                                    for(RetDataLength = 0; RetDataLength < MachineSerialNumberLength ; RetDataLength++)
                                                                                        MachineSerialNumber[RetDataLength] = RxAnalsys.member.Data[3+RetDataLength]; 
                                                                                    RetDataLength = 3 ;
                                                                                    LCBSimulator_SetSerialNumber(1) ;
                                                                                    break ;
                                                        case CmdReadLCBSerialNumber :// 0x04  //  3     3+N     N <= 252    
                                                                                    break ;
                                                        }
                                                    break ; 
#endif                                 
          // Add 20200319 Athera
          // Add 20230314 ICR70
          case  GetSensorData                       ://0x51
                                                    // 20230314 Modify support ICR70LCB
                                                    switch( LCBParameter.LCBSimulatorType )
                                                        {
                                                        case  _Athena       :
                                                                            //----------------------------------------------------------------------------------------            
                                                                            //0x51  //  0     Varies
                                                                            RetDataLength = 19 ;
                                                                            // [0] = 0x1B (LCB Type)
                                                                            // [1] = 0x07 (Machine Type)
                                                                            // [2,3] = Crank RPM ;
                                                                            // [4] = Crank Direction , if Crank RPM = 0 then 0 else 1
                                                                            // [5,6] = FlyWheelRPM
                                                                            // [7,8] = Watts
                                                                            // [9,10] = Stepper Motor Status
                                                                            // [11] = Stepper Motor Step Mode , 0:full,1:half,2 1/4,3:1/8,4:1/16
                                                                            // [12,13,14,15,16,17] = Motor Position
                                                                            // [18] = Resistance Level
                                                                            RetData[0] = 0x1B ;
                                                                            RetData[1] = 0x07 ;
                                                                            TempData.All = LCBParameter.CrankRPM ;
                                                                            RetData[2] = TempData.By.High ; // High byte
                                                                            RetData[3] = TempData.By.Low ;                                                   
                                                                            RetData[4] = 0 ;
                                                                            if( LCBParameter.CrankRPM != 0 )
                                                                                RetData[4] = 1 ;  
                                                                            TempData.All = LCBParameter.FlywheelRPM ;
                                                                            RetData[5] = TempData.By.High ; // High byte
                                                                            RetData[6] = TempData.By.Low ; 
                                                                            TempData.All = LCBParameter.Watts ;
                                                                            RetData[7] = TempData.By.High ; // High byte
                                                                            RetData[8] = TempData.By.Low ; 
                                                                            TempData.All = LCBParameter.MotorStatus.Full ;
                                                                            RetData[9] = TempData.By.High ; // High byte
                                                                            RetData[10] = TempData.By.Low ; 
                                                                            //
                                                                            RetData[11] = LCBParameter.MotorPosition.Member.Mode ;
                                                                            RetData[12] = LCBParameter.MotorPosition.Byte[2] ;
                                                                            RetData[13] = LCBParameter.MotorPosition.Byte[1] ;
                                                                            RetData[14] = LCBParameter.MotorPosition.Byte[0] ;
                                                                            //
                                                                            Temp.Full = LCBSimulator_GetMotorPosMax(LCBParameter.MotorPosition.Member.Mode) ;
                                                                            RetData[15] = Temp.Byte[2] ;
                                                                            RetData[16] = Temp.Byte[1] ;
                                                                            RetData[17] = Temp.Byte[0] ;
                                                                            //
                                                                            RetData[18] = LCBParameter.ResistanceLevel ; 
                                                                            break ;
                                                        case  _ITC          :
                                                                            // Add 20210602 Load Cell Set Configuration(0x51), 18 byte
                                                                            for( RetDataLength = 0 ; RetDataLength < 18 ; RetDataLength+=2 )
                                                                                {
                                                                                LCBParameter.LoadCellConfig.Data[RetDataLength+1] = RxAnalsys.member.Data[RetDataLength] ;   
                                                                                LCBParameter.LoadCellConfig.Data[RetDataLength] = RxAnalsys.member.Data[RetDataLength+1] ;                                                               
                                                                                }
                                                                            RetDataLength = 0 ;
                                                                            break ;
                                                        case  _JISICR70LCB  : 
                                                                            //----------------------------------------------------------------------------------------            
                                                                            //0x51  //  0     Varies
                                                                            RetDataLength = 19 ;
                                                                            // [0] = 0x1D (LCB Type)
                                                                            // [1] = 0x07 (Machine Type)
                                                                            // [2,3] = Crank RPM ;
                                                                            // [4] = Crank Direction , if Crank RPM = 0 then 0 else 1
                                                                            // [5,6] = 
                                                                            // [7,8] = Watts
                                                                            // [9,10] = 
                                                                            // [11] = 
                                                                            // [12,13,14,15,16,17] = 
                                                                            // [18] = Resistance Level
                                                                            RetData[0] = 0x1D ;
                                                                            RetData[1] = 0x07 ;
                                                                            TempData.All = LCBParameter.CrankRPM ;
                                                                            RetData[2] = TempData.By.High ; // High byte
                                                                            RetData[3] = TempData.By.Low ;                                                   
                                                                            RetData[4] = 0 ;
                                                                            if( LCBParameter.CrankRPM != 0 )
                                                                                RetData[4] = 1 ;  
                                                                            // Flywheel RPM ;
                                                                            RetData[5] = 0 ; // High byte
                                                                            RetData[6] = 0 ; 
                                                                            TempData.All = LCBParameter.Watts ;
                                                                            RetData[7] = TempData.By.High ; // High byte
                                                                            RetData[8] = TempData.By.Low ; 
                                                                            // Motor status
                                                                            RetData[9] = 0 ; // High byte
                                                                            RetData[10] = 0 ; 
                                                                            //
                                                                            RetData[11] = 0;
                                                                            RetData[12] = 0 ;
                                                                            RetData[13] = 0 ;
                                                                            RetData[14] = 0 ;
                                                                            //
                                                                            // Max position of motor
                                                                            RetData[15] = 0 ;
                                                                            RetData[16] = 0 ;
                                                                            RetData[17] = 0 ;
                                                                            //
                                                                            RetData[18] = LCBParameter.ResistanceLevel ;                                                           
                                                                            break ;
                                                        default             :
                                                                            ErrorCodeStatus.bit.EB0441 = 1 ;
                                                                            break ;
                                                        }          
                                                    /*
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 Load Cell Set Configuration(0x51), 18 byte
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        for( RetDataLength = 0 ; RetDataLength < 18 ; RetDataLength+=2 )
                                                            {
                                                            LCBParameter.LoadCellConfig.Data[RetDataLength+1] = RxAnalsys.member.Data[RetDataLength] ;   
                                                            LCBParameter.LoadCellConfig.Data[RetDataLength] = RxAnalsys.member.Data[RetDataLength+1] ;                                                               
                                                            }
                                                        RetDataLength = 0 ;
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    //0x51  //  0     Varies
                                                    RetDataLength = 19 ;
                                                    // [0] = 0x1B (LCB Type)
                                                    // [1] = 0x07 (Machine Type)
                                                    // [2,3] = Crank RPM ;
                                                    // [4] = Crank Direction , if Crank RPM = 0 then 0 else 1
                                                    // [5,6] = FlyWheelRPM
                                                    // [7,8] = Watts
                                                    // [9,10] = Stepper Motor Status
                                                    // [11] = Stepper Motor Step Mode , 0:full,1:half,2 1/4,3:1/8,4:1/16
                                                    // [12,13,14,15,16,17] = Motor Position
                                                    // [18] = Resistance Level
                                                    RetData[0] = 0x1B ;
                                                    RetData[1] = 0x07 ;
                                                    TempData.All = LCBParameter.CrankRPM ;
                                                    RetData[2] = TempData.By.High ; // High byte
                                                    RetData[3] = TempData.By.Low ;                                                   
                                                    RetData[4] = 0 ;
                                                    if( LCBParameter.CrankRPM != 0 )
                                                        RetData[4] = 1 ;  
                                                    TempData.All = LCBParameter.FlywheelRPM ;
                                                    RetData[5] = TempData.By.High ; // High byte
                                                    RetData[6] = TempData.By.Low ; 
                                                    TempData.All = LCBParameter.Watts ;
                                                    RetData[7] = TempData.By.High ; // High byte
                                                    RetData[8] = TempData.By.Low ; 
                                                    TempData.All = LCBParameter.MotorStatus.Full ;
                                                    RetData[9] = TempData.By.High ; // High byte
                                                    RetData[10] = TempData.By.Low ; 
                                                    //
                                                    RetData[11] = LCBParameter.MotorPosition.Member.Mode ;
                                                    RetData[12] = LCBParameter.MotorPosition.Byte[2] ;
                                                    RetData[13] = LCBParameter.MotorPosition.Byte[1] ;
                                                    RetData[14] = LCBParameter.MotorPosition.Byte[0] ;
                                                    //
                                                    Temp.Full = LCBSimulator_GetMotorPosMax(LCBParameter.MotorPosition.Member.Mode) ;
                                                    RetData[15] = Temp.Byte[2] ;
                                                    RetData[16] = Temp.Byte[1] ;
                                                    RetData[17] = Temp.Byte[0] ;
                                                    //
                                                    RetData[18] = LCBParameter.ResistanceLevel ; 
                                                    */
                                                    break ;            
          case  SetStepMotorPosition                ://0x52
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 Load Cell Get Complementary(0x52) 18 byte
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        for( RetDataLength = 0 ; RetDataLength < 18 ; RetDataLength+=2 )
                                                            {
                                                            RetData[RetDataLength] = LCBParameter.LoadCellConfig.Data[RetDataLength+1] ;
                                                            RetData[RetDataLength+1] = LCBParameter.LoadCellConfig.Data[RetDataLength] ;
                                                            }
                                                        /*
                                                        RetDataLength = 18 ;
                                                        TempData.All = 0 ; // Reserved
                                                        RetData[0] = TempData.By.High ; // High byte
                                                        RetData[1] = TempData.By.Low ;                                                        
                                                        TempData.All = LCBParameter.NoMotionTimeout ;     //  default 60 (1 minute), MAXIMUM 655 seconds (10.9 minutes)
                                                        RetData[2] = TempData.By.High ; // High byte
                                                        RetData[3] = TempData.By.Low ;                                                        
                                                        TempData.All = LCBParameter.SmartRepAccTrigger ;  //  default 60 (1 minute), MAXIMUM 655 seconds (10.9 minutes)
                                                        RetData[4] = TempData.By.High ; // High byte
                                                        RetData[5] = TempData.By.Low ; 
                                                        TempData.All = LCBParameter.mmPer1000count ;      // mm stack travel per 1000 encoder counts (DEFAULT 2283)
                                                        RetData[6] = TempData.By.High ; // High byte
                                                        RetData[7] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.BottomThreshold_mm ;  // bottom threshold (ADC's ignored below this value)
                                                        RetData[8] = TempData.By.High ; // High byte
                                                        RetData[9] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.TriggerHeight_mm ;    // height at which state machine leaves state 0 (9 MM DEFAULT)
                                                        RetData[10] = TempData.By.High ; // High byte
                                                        RetData[11] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.StrokeDown_mm ;       // minimum downward length that qualifies a rep (80mm DEFAULT)
                                                        RetData[12] = TempData.By.High ; // High byte
                                                        RetData[13] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.StrokeUp_mm ;         // minimum upward length that qualifies a partial rep (100mm DEFAULT)
                                                        RetData[14] = TempData.By.High ; // High byte
                                                        RetData[15] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.Direction ;           // LAST [ 1, 0 ] facing pcb (1)==CW is up , (-1)==CCW is up, DEFAULT)
                                                        RetData[16] = TempData.By.High ; // High byte
                                                        RetData[17] = TempData.By.Low ; 
                                                        */
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    //0x52  //  4     0
                                                    LCBParameter.MotorPosition.Byte[0]=RxAnalsys.member.Data[3] ;
                                                    LCBParameter.MotorPosition.Byte[1]=RxAnalsys.member.Data[2] ;
                                                    LCBParameter.MotorPosition.Byte[2]=RxAnalsys.member.Data[1] ;
                                                    LCBParameter.MotorPosition.Byte[3]=RxAnalsys.member.Data[0] ;
                                                    LCBParameter.ResistanceLevel = LCBSimulator_MotorPosToResistanceLevel(LCBParameter.MotorPosition.Full) ;
                                                    break ;            
          case  SetParameter                        :
                                                    // 20230314 Add ICR70
                                                    switch( LCBParameter.LCBSimulatorType )
                                                        {
                                                        case  _Athena       :
                                                                            //----------------------------------------------------------------------------------------            
                                                                            //0x53  //  1+4*N 0
                                                                            LCBParameter.Calculation.NunberOfParameter = RxAnalsys.member.Data[0] ;            
                                                                            Size = (unsigned short)LCBParameter.Calculation.NunberOfParameter*4 ;
                                                                            if( Size > _AthenaMaxSize )
                                                                                Size = _AthenaMaxSize ;
                                                                            
                                                                            for( WLength = 0 ; WLength < Size ; WLength+=4)
                                                                                {
                                                                                LCBParameter.Calculation.Parameter[WLength+0] = RxAnalsys.member.Data[1+WLength] ;
                                                                                LCBParameter.Calculation.Parameter[WLength+1] = RxAnalsys.member.Data[2+WLength] ;
                                                                                LCBParameter.Calculation.Parameter[WLength+2] = RxAnalsys.member.Data[3+WLength] ;
                                                                                LCBParameter.Calculation.Parameter[WLength+3] = RxAnalsys.member.Data[4+WLength] ;
                                                                                }
                                                                            break ;
                                                        case  _ITC          :
                                                                            //------------------------------------------------------------------------------------
                                                                            // Add 20210602 Get SSA Verion (0x53)
                                                                            if( LCBParameter.LCBSimulatorType == _ITC )
                                                                                {
                                                                                RetData[0] = 0 ;      //Reserved
                                                                                RetData[1] = _ITC ;   //LCB Type
                                                                                RetData[2] = 22 ;     //Major Version
                                                                                RetData[3] = 1 ;      //Minor Version
                                                                                RetDataLength = 4 ;                                                          
                                                                                }
                                                                            break ;
                                                        case  _JISICR70LCB  : 
                                                                            //----------------------------------------------------------------------------------------            
                                                                            //0x53  //  1+4*N 0
                                                                            LCBParameter.Calculation.NunberOfParameter = RxAnalsys.member.Data[0] ;            
                                                                            Size = (unsigned short)LCBParameter.Calculation.NunberOfParameter*4 ;
                                                                            if( Size > _AthenaMaxSize )
                                                                                Size = _AthenaMaxSize ;
                                                                            
                                                                            for( WLength = 0 ; WLength < Size ; WLength+=4)
                                                                                {
                                                                                LCBParameter.Calculation.Parameter[WLength+0] = RxAnalsys.member.Data[1+WLength] ;
                                                                                LCBParameter.Calculation.Parameter[WLength+1] = RxAnalsys.member.Data[2+WLength] ;
                                                                                LCBParameter.Calculation.Parameter[WLength+2] = RxAnalsys.member.Data[3+WLength] ;
                                                                                LCBParameter.Calculation.Parameter[WLength+3] = RxAnalsys.member.Data[4+WLength] ;
                                                                                }                                                        
                                                                            break ;
                                                        default             :
                                                                            ErrorCodeStatus.bit.EB0441 = 1 ;
                                                                            break ;
                                                        } 
                                                    /*
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 Get SSA Verion (0x53)
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetData[0] = 0 ;      //Reserved
                                                        RetData[1] = _ITC ;   //LCB Type
                                                        RetData[2] = 22 ;     //Major Version
                                                        RetData[3] = 1 ;      //Minor Version
                                                        RetDataLength = 4 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    //0x53  //  1+4*N 0
                                                    LCBParameter.Calculation.NunberOfParameter = RxAnalsys.member.Data[0] ;            
                                                    Size = (unsigned short)LCBParameter.Calculation.NunberOfParameter*4 ;
                                                    if( Size > _AthenaMaxSize )
                                                        Size = _AthenaMaxSize ;
                                                    
                                                    for( WLength = 0 ; WLength < Size ; WLength+=4)
                                                        {
                                                        LCBParameter.Calculation.Parameter[WLength+0] = RxAnalsys.member.Data[1+WLength] ;
                                                        LCBParameter.Calculation.Parameter[WLength+1] = RxAnalsys.member.Data[2+WLength] ;
                                                        LCBParameter.Calculation.Parameter[WLength+2] = RxAnalsys.member.Data[3+WLength] ;
                                                        LCBParameter.Calculation.Parameter[WLength+3] = RxAnalsys.member.Data[4+WLength] ;
                                                        }
                                                    */
                                                    break ;            
          case  GetParameter                        ://0x54  //  0     1+4*N
                                                    // 20200707
                                                    if( LCBParameter.Calculation.NunberOfParameter == 0 )
                                                        LCBParameter.Calculation.NunberOfParameter = 21 ;
                                                    //
                                                    Size = (unsigned short)LCBParameter.Calculation.NunberOfParameter*4 ;
                                                    RetData[0] = LCBParameter.Calculation.NunberOfParameter ;
                                                    for( WLength = 0 ; WLength < Size ; WLength++ )
                                                        RetData[1+WLength] = LCBParameter.Calculation.Parameter[WLength] ;
                                                    
                                                    RetDataLength = 1+Size ;
                                                    break ;            
          case  SetActionSpinDown                   ://0x55  //  1+N   1
                                                    RetData[0] = 0 ; // High byte
                                                    RetDataLength = 1 ;              
                                                    break ;            
          case  GetActionSpinDown                   ://0x56  //  0     15
                                                    for( WLength = 0 ; WLength < 15 ; WLength++ )
                                                        RetData[WLength] = 0 ;
                                                    RetDataLength = WLength ;
                                                    break ;            
          case  SetErgModeSettings                  ://0x57  //  1+4*N 0
                                                    LCBParameter.Erg.NunberOfParameter = RxAnalsys.member.Data[0] ;
                                                    Size = (unsigned short)LCBParameter.Erg.NunberOfParameter*4 ;
                                                    if( Size > _AthenaMaxSize )
                                                        Size = _AthenaMaxSize ;
                                                    
                                                    for( WLength = 0 ; WLength < Size ; WLength+=4)
                                                        {
                                                        LCBParameter.Erg.Parameter[WLength+0] = RxAnalsys.member.Data[1+WLength] ;
                                                        LCBParameter.Erg.Parameter[WLength+1] = RxAnalsys.member.Data[2+WLength] ;
                                                        LCBParameter.Erg.Parameter[WLength+2] = RxAnalsys.member.Data[3+WLength] ;
                                                        LCBParameter.Erg.Parameter[WLength+3] = RxAnalsys.member.Data[4+WLength] ;
                                                        }                                                    
                                                    break ;            
          case  GetErgModeSettings                  ://0x59  //  0     1+4*N
                                                    // 20200707
                                                    if( LCBParameter.Erg.NunberOfParameter == 0 )
                                                        LCBParameter.Erg.NunberOfParameter = 7 ;
                                                    //
                                                    Size = (unsigned short)LCBParameter.Erg.NunberOfParameter*4 ;
                                                    RetData[0] = LCBParameter.Erg.NunberOfParameter ;
                                                    for( WLength = 0 ; WLength < Size ; WLength++ )
                                                        RetData[1+WLength] = LCBParameter.Erg.Parameter[WLength] ;
                                                    
                                                    RetDataLength = 1+Size ;            
                                                    break ;            
          case  SetResistanceLevel                  ://0x58  //  1     0     
                                                    LCBParameter.ResistanceLevel = RxAnalsys.member.Data[0] ;
                                                    // 20230314
                                                    switch( LCBParameter.LCBSimulatorType )
                                                        {
                                                        case  _Athena       :
                                                                            // for Athena
                                                                            if( LCBParameter.ResistanceLevel >= 26 )
                                                                                LCBParameter.ResistanceLevel = 25 ;
                                                                            LCBParameter.MotorPosition.Member.Position = ResistanceTable[LCBParameter.ResistanceLevel] ;
                                                                            LCBParameter.MotorPosition.Member.Mode = 4 ; // 1/16
                                                                            //
                                                                            break ;
                                                        case  _JISICR70LCB  : 
                                                                            if( LCBParameter.ResistanceLevel > 25 )
                                                                                LCBParameter.ResistanceLevel = 25 ;
                                                                            else if( LCBParameter.ResistanceLevel < 1 )
                                                                                LCBParameter.ResistanceLevel = 1 ;
                                                                            LCBParameter.ECBCount = ICR70ECBcount[(LCBParameter.ResistanceLevel-1)] ;
                                                                            LCBParameter.ECBPosition = LCBParameter.ECBCount ;
                                                                            break ;
                                                        default             :
                                                                            ErrorCodeStatus.bit.EB0441 = 1 ;
                                                                            break ;
                                                        }
                                                    
                                                    break ;
          //------------------------------------------------------------------------ 
          //20210602 Load Cell
          case  CmdGetRawData                       ://  0x40  //  0     4   //Get Raw Data, ADC & Encoder
                                                    TempData.All = LCBParameter.LoadCellADC ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ; 
                                                    TempData.All = LCBParameter.LoadCellEncoder ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ;                                                    
                                                    RetDataLength = 4 ;            
                                                    break ;            
          case  CmdSetLoadCellOffset                ://  0x41  //  2     0   //Set the offset for the load cell.
                                                    LCBParameter.LoadCellOffset = TempData.All ;
                                                    break ;
          case  CmdGetLoadCellOffset                ://  0x42  //  0     2   //Get the offset for the load cell.
                                                    TempData.All = LCBParameter.LoadCellOffset ;
                                                    RetData[0] = TempData.By.High ; // High byte
                                                    RetData[1] = TempData.By.Low ;                                                    
                                                    RetDataLength = 2 ; 
                                                    break ;                                                    
          case  CmdSetComplementary                 ://  0x47  //  30    0
                                                    //    
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        LCBParameter.LeadCellCablibration.Cfg.Version = RxAnalsys.member.Data[0] ;
                                                        TempData.By.High = RxAnalsys.member.Data[1];
                                                        TempData.By.Low = RxAnalsys.member.Data[2];
                                                        LCBParameter.LeadCellCablibration.Cfg.counts_at_0v = TempData.All;
                                                        TempData.By.High = RxAnalsys.member.Data[3];
                                                        TempData.By.Low = RxAnalsys.member.Data[4];
                                                        LCBParameter.LeadCellCablibration.Cfg.counts_per_mV_per_V = TempData.All ;
                                                        StartAddr.By.HH = RxAnalsys.member.Data[5];
                                                        StartAddr.By.HL = RxAnalsys.member.Data[6];
                                                        StartAddr.By.LH = RxAnalsys.member.Data[7];
                                                        StartAddr.By.LL = RxAnalsys.member.Data[8];
                                                        LCBParameter.LeadCellCablibration.Cfg.gage_offset_nV_per_V = StartAddr.All ;
                                                        TempData.By.High = RxAnalsys.member.Data[9];
                                                        TempData.By.Low = RxAnalsys.member.Data[10];
                                                        LCBParameter.LeadCellCablibration.Cfg.loadcell_tare_mN = TempData.All;
                                                        TempData.By.High = RxAnalsys.member.Data[11];
                                                        TempData.By.Low = RxAnalsys.member.Data[12];
                                                        LCBParameter.LeadCellCablibration.Cfg.standard_N_per_mV_per_V = TempData.All ;
                                                        LCBParameter.LeadCellCablibration.Cfg.angle_loadcell_45_or_180 = RxAnalsys.member.Data[13] ;
                                                        TempData.By.High = RxAnalsys.member.Data[14];
                                                        TempData.By.Low = RxAnalsys.member.Data[15];
                                                        LCBParameter.LeadCellCablibration.Cfg.nominal_plate_mass_g = TempData.All ;
                                                        TempData.By.High = RxAnalsys.member.Data[16];
                                                        TempData.By.Low = RxAnalsys.member.Data[17];
                                                        LCBParameter.LeadCellCablibration.Cfg.head_plate_assembly_mass_g = TempData.All ;
                                                        TempData.By.High = RxAnalsys.member.Data[18];
                                                        TempData.By.Low = RxAnalsys.member.Data[19];
                                                        LCBParameter.LeadCellCablibration.Cfg.plate_tolerance_scale_1000 = TempData.All ;
                                                        TempData.By.High = RxAnalsys.member.Data[20];
                                                        TempData.By.Low = RxAnalsys.member.Data[21];
                                                        LCBParameter.LeadCellCablibration.Cfg.offset_counts = TempData.All ;
                                                        StartAddr.By.HH = RxAnalsys.member.Data[22];
                                                        StartAddr.By.HL = RxAnalsys.member.Data[23];
                                                        StartAddr.By.LH = RxAnalsys.member.Data[24];
                                                        StartAddr.By.LL = RxAnalsys.member.Data[25];
                                                        LCBParameter.LeadCellCablibration.Cfg.reserved = StartAddr.All ;
                                                        StartAddr.By.HH = RxAnalsys.member.Data[26];
                                                        StartAddr.By.HL = RxAnalsys.member.Data[27];
                                                        StartAddr.By.LH = RxAnalsys.member.Data[28];
                                                        StartAddr.By.LL = RxAnalsys.member.Data[29];
                                                        LCBParameter.LeadCellCablibration.Cfg.time_stamp = StartAddr.All ;
                                                        }
                                                    //
                                                    RetDataLength = 0 ;            
                                                    break ;            
          case  CmdGetCalibration                   ://  0x48  //  0     40  //Get the current in-use calibration table from the LCB.  Note, this table persists between software updates
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        // 10 bytes unique ID of the NXP microchip on the LCB
                                                        for( RetDataLength = 0 ; RetDataLength < 10 ; RetDataLength++ )
                                                            RetData[RetDataLength] = 3 ;  
                                                        //
                                                        RetData[10] = LCBParameter.LeadCellCablibration.Cfg.Version ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.counts_at_0v ;
                                                        RetData[11] = TempData.By.High ;
                                                        RetData[12] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.counts_per_mV_per_V ;
                                                        RetData[13] = TempData.By.High ;
                                                        RetData[14] = TempData.By.Low ;
                                                        StartAddr.All = LCBParameter.LeadCellCablibration.Cfg.gage_offset_nV_per_V ;
                                                        RetData[15] = StartAddr.By.HH ;
                                                        RetData[16] = StartAddr.By.HL ;
                                                        RetData[17] = StartAddr.By.LH ;
                                                        RetData[18] = StartAddr.By.LL ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.loadcell_tare_mN ;
                                                        RetData[19] = TempData.By.High ;
                                                        RetData[20] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.standard_N_per_mV_per_V ;
                                                        RetData[21] = TempData.By.High ;
                                                        RetData[22] = TempData.By.Low ;
                                                        RetData[23] = LCBParameter.LeadCellCablibration.Cfg.angle_loadcell_45_or_180 ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.nominal_plate_mass_g ;
                                                        RetData[24] = TempData.By.High ;
                                                        RetData[25] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.head_plate_assembly_mass_g ;
                                                        RetData[26] = TempData.By.High ;
                                                        RetData[27] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.plate_tolerance_scale_1000 ;
                                                        RetData[28] = TempData.By.High ;
                                                        RetData[29] = TempData.By.Low ;
                                                        TempData.All = LCBParameter.LeadCellCablibration.Cfg.offset_counts ;
                                                        RetData[30] = TempData.By.High ;
                                                        RetData[31] = TempData.By.Low ;
                                                        StartAddr.All = LCBParameter.LeadCellCablibration.Cfg.reserved ;
                                                        RetData[32] = StartAddr.By.HH ;
                                                        RetData[33] = StartAddr.By.HL ;
                                                        RetData[34] = StartAddr.By.LH ;
                                                        RetData[35] = StartAddr.By.LL ;
                                                        StartAddr.All = LCBParameter.LeadCellCablibration.Cfg.time_stamp ;
                                                        RetData[36] = StartAddr.By.HH ;
                                                        RetData[37] = StartAddr.By.HL ;
                                                        RetData[38] = StartAddr.By.LH ;
                                                        RetData[39] = StartAddr.By.LL ;
                                                        //
                                                        RetDataLength = 40 ;
                                                        break ;
                                                        }
                                                    RetDataLength = 0 ;
                                                    break ;
          case  CmdFlush                            ://  0x49  
                                                    //  0     2   //Flush the load cell of any cached reps or any incoming reps prior to the flush call.
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        TempData.All = 100 ;
                                                        RetData[0] = TempData.By.High ; // High byte
                                                        RetData[1] = TempData.By.Low ; 
                                                        RetDataLength = 2;
                                                        }
                                                    break; 
          case  CmdSetLoadCellScale                 ://  0x45 
                                                    //  2     0   //Set the scale of the load cell.                                                    
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        LCBParameter.LoadCellScale = TempData.All ;
                                                    break ;
          case  CmdGetLoadCellScale                 : //0x46  //  0     2   //Get the scale of the load cell.
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        TempData.All = LCBParameter.LoadCellScale ;
                                                        RetData[0] = TempData.By.High ; // High byte
                                                        RetData[1] = TempData.By.Low ;
                                                        RetDataLength = 2 ;
                                                        }
                                                    break ;
          //-------------------------------------------------------------------------                                                    
          default				                            :
                                                    break ;
          }
      //----------------------------------------------------------------------------
      }
  //----------------------------------------------------------------------------
  // Check Command Error 
  #ifdef _EnableErrorCode_
  if( ErrorCodeStatus.bit.EB0441 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_,0x0441 ) ; 
      ErrorCodeStatus.bit.EB0441 = 0 ;
      LCBStatus.bit.CommandErrorStatus = 1 ;
      // 20200115 Add Log Command
      JHTLCBComm_log0441Command(Command,SubCommand) ;
      //
      //
      }
  if( ErrorCodeStatus.bit.EB0442 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_,0x0442 ) ;  
      ErrorCodeStatus.bit.EB0442 = 0 ;
      LCBStatus.bit.CommandErrorStatus = 1 ;
      }
  #endif
  //----------------------------------------------------------------------------
  JHTLCBComm_ReturnData( Command, &RetData[0], RetDataLength ) ;
  //----------------------------------------------------------------------------
  return ;
}







/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_CheckSupportCommand(unsigned char CheckCmd,unsigned char SubCmd)
{
    
  
  
  
  return 1;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_CheckCommandAndLength(unsigned char CheckCmd,unsigned char SubCmd ,unsigned char CheckLen )
{
  unsigned char RetStatus = 2 ;
  //
  if( JHTLCBComm_CheckSupportCommand(CheckCmd,SubCmd) == 1 )
      {  
      //
      switch( CheckCmd )
          {
          // 0
          case CmdInitial      			                :	
          case CmdGetStatus    			                :	
          case CmdGetErrorCode 			                :
          case CmdGetVersion   			                :	
          case CmdSkipErrorCode 		                :	
          case CmdGetRpm         		                ://0x63 //	0     2           // ICR70 
          case CmdGetBatteryStatus		              :			
          case CmdGetBatteryCapacity                :
          case CmdCalibrate                         :   
          case CmdGetInclinePercent                 :  
          case CmdGetClimbmillStatus                :  	
          // TM
          case CmdGetMainMotorInfo                  ://0x96
          case CmdGetInclineLocation                :     
          case CmdGetRollerRpm                      :
          case CmdGetMotorRpm        		            :  
          case CmdGetTreadmillInUse                 :
          // Climbmill EN2017  
          case CmdGetBrakePressureValue             :
          case CmdGetWatts                          ://0x8E   //	0     2           // ICR70 
          case CmdGetSPM                            :  
          //JIS LCB                             
          case CmdGetEcbStatus		                  ://0x83  // 	0     1
          case CmdGetEcbCount		                    ://0x84  // 	0     2   //ICR70 same as 0x64 
          case CmdSetEcbInit                        : //JIS BIKE                  //ICR70 set to ECB Level 1
          case CmdGetEcbLocation                    : //0x64 // 	0     2   //ICR70 same as 0x84
          // Athena
          case GetSensorData                        : //0x51  //  0     Varies
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetStatus = 1 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
          case GetParameter                         : //0x54  //  0     1+4*N  
          case GetActionSpinDown                    : //0x56  //  0     15  
          case GetErgModeSettings                   : //0x59  //  0     1+4*N   
          // ITC
          case CmdGetLoadCellOffset                 : // 0x42  //  0     2   //Get the offset for the load cell.  
          case CmdGetLoadCellScale                  : //0x46  //  0     2   //Get the scale of the load cell.  
                                                    if( CheckLen == 0 )
                                                        RetStatus = 1 ;
                                                    break ;
          // 1 byte                                                
          case CmdSetGenMegPolePair  	              :
          case CmdSetMachineType  		              :	
          case CmdSetPowerOff 	                    :	
          case CmdEUPsMode                          :        
          case CmdSetInclineAction                  :  
          case CmdSetEStopActionValue               :
          case CmdSetClimbmillStatus                :     
          case CmdSetWorkStatus                     :
          // JIS LCB
          case CmdSetEcbAction			                ://0x82 // 	1   0   //JIS  
          // Athena & ICR70            
          case SetResistanceLevel                   : //0x58  //  1     0             
                                                    if( CheckLen == 1 )
                                                        RetStatus = 1 ;                                                
                                                    break ;		
          // 2 byte                                     
          // Climbmill EN2017
          case CmdSetSPM                            ://0x4A
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602 0x4A   0    4/34  , no rep = 4, rep = 34
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetStatus = 1 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
            
          case CmdUpdateProgram	                    :	
          case CmdSetRpmGearRatio                   :
          case CmdSetLimitRpmForCharge              :	
          case CmdSetLimitRpmForResis               :
          case CmdSetResistanceTypeAndResistance    :
          case CmdSetWatts                          :   
          case CmdSetPwm                            :
          case CmdSetEMagnetCurrent	                :	      
          case CmdSetBeginBatteryCharge             :
          case CmdSetGapVrCalibrateIncline          :   
          case CmdSetInclinePercent                 :      
          case CmdSetInclineLocation                :
          case CmdSetInclineStroke                  :
          case CmdSetMotorRpm                       :
          case CmdSetMotorRpmAndReturn              :
          // JIS
          case CmdSetEcbLocation                    :  //JIS BIKE // ICR70 
          case CmdSetDriveMotorHP		                :  //0x90  //	2     0     0~65535 x 1Watt            
          // ITC
          case CmdSetLoadCellScale                  :  //0x45  //  2     0     Set the scale of the load cell    
          case CmdSetLoadCellOffset                 ://  0x41  //  2     0   //Set the offset for the load cell.
            
                                                    if( CheckLen == 2 )
                                                        RetStatus = 1 ;	      
                                                    break ;	
          // Athena ------------------------------------------------------------                                                   
          case SetStepMotorPosition                 : 
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetStatus = 1 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    //0x52  //  4     0    
                                                    if( CheckLen == 4 )
                                                        RetStatus = 1 ;
                                                    break ;
          case SetParameter                         : //0x53  //  1+4*N 0  
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetStatus = 1 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
          case SetErgModeSettings                   : //0x57  //  1+4*N 0 
          case SetActionSpinDown                    : //0x55  //  1+N   1
                                                    RetStatus = 1 ;
                                                    break ;
          //--------------------------------------------------------------------                                                    
          case  CmdLCBDeviceData                    :
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        if( CheckLen == 0 )
                                                            RetStatus = 1 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    switch( SubCmd )
                                                        {
                                                                                   // R   T
                                                        case GetEEPromMemorySizes :// 3   2
                                                        case GetEEPromMemoryData  :// 3   2
                                                        case GetECBCurrent        :// 3   4
                                                        case GetDCBusStatus       :// 3   4
                                                        case GetLCBVersion        :// 3   8
                                                                                  if( CheckLen == 3 )
                                                                                      RetStatus = 1 ;	
                                                                                  break ;
                                                        case ReadByteFlashData    :
                                                                                  if( CheckLen == 6 )
                                                                                      RetStatus = 1 ;
                                                                                  break ;
                                                        // Add 20200309                          
                                                        case GetAthenaVerion      :// 1   6   
                                                                                  if( CheckLen == 1 )
                                                                                      RetStatus = 1 ;
                                                                                  break ;
                                                        // Add 20210317 for JIS LCB Type 0C  
                                                        case SavePWM              :
                                                                                  if( CheckLen == 4 )
                                                                                      RetStatus = 1 ;
                                                                                  break ;
                                                        case ReadPWM              :
                                                                                  if( CheckLen == 2 )
                                                                                      RetStatus = 1 ;
                                                                                  break ;
                                                        //------------------------------------------------
                                                        }
                                                    break ;
#ifdef  _SupportSerialNumberCommand                                                     
            case  CmdSerialNumber                   :  
                                                    //------------------------------------------------------------------------------------
                                                    // Add 20210602
                                                    if( LCBParameter.LCBSimulatorType == _ITC )
                                                        {
                                                        RetStatus = 1 ;                                                          
                                                        break ;
                                                        }
                                                    //----------------------------------------------------------------------------------------            
                                                    switch( SubCmd )
                                                        {
                                                        case CmdEraseSerialNumber   :// 0x00  //  3     3
                                                        case CmdReadSerialNumber    :// 0x02  //  3     3+N     N <= 252
                                                        case CmdGetMemoryLength     :// 0x03  //  3     3
                                                                                    if( CheckLen == 3 )
                                                                                        RetStatus = 1 ;	
                                                                                    break ;
                                                        case CmdWriteSerialNumber   :// 0x01  //  3+N   3       N <= 252
                                                                                    if( CheckLen > 3 )
                                                                                        RetStatus = 1 ;
                                                                                    break ;
                                                        case CmdReadLCBSerialNumber :// 0x04  //  3     3+N     N <= 252
                                                                                    if( CheckLen == 3 )
                                                                                        RetStatus = 1 ;
                                                                                    break ;
                                                        default                     :                                                        
                                                                                    RetStatus = 0 ;
                                                                                    ErrorCodeStatus.bit.EB0441 = 1 ;
                                                                                    break ;
                                                        }
                                                    break ;
#endif           
            //20210602 Load Cell
            case  CmdGetRawData                     ://  0x40  //  0     4   //Get Raw Data, ADC & Encoder
            case  CmdSetComplementary               ://  0x47  //  30    0
            case  CmdGetCalibration                 ://  0x48  //  0     40  //Get the current in-use calibration table from the LCB.  Note, this table persists between software updates
            case  CmdFlush                          ://  0x49  //  0     2   //Flush the load cell of any cached reps or any incoming reps prior to the flush call.
                                                    RetStatus = 1 ;
                                                    break;            
            default		                              :
                                                    RetStatus = 0 ;
                                                    ErrorCodeStatus.bit.EB0441 = 1 ;
                                                    break ;
            }	
      }
  //----------------------------------------------------------------------------  
  // Mask by Kunlung 20190507 Disable Length error 
  //if( RetStatus == 2 )
  //   ErrorCodeStatus.bit.EB0442 = 1 ;
  //----------------------------------------------------------------------------   
  return RetStatus ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_ReturnData( unsigned char TransCmd, unsigned char *DataPtr, unsigned char DataLen )
{
  unsigned short DataLength ;
  unsigned char DataIndex ;
  //
  DataLength = DataMsbLocation + DataLen  ;
  DataIndex = 0 ;
  //  
  #ifndef _EnableErrorCode_
  // 20210602 Add ITC
  //LCBStatus.bit.McbErrorStatus = 0 ;
  if( LCBParameter.LCBSimulatorType == _ITC )  
      ITCStatus.bit.McbErrorStatus = 0 ;
  else
      LCBStatus.bit.McbErrorStatus = 0 ;
  //
  LCBStatus.bit.CommandErrorStatus = 0 ;
  #endif
  if( LCBSimulator_GetResetOKStatus() == 0 )
      {
      LCBStatus.bit.InitialStatus = 1 ;
      }
  else
      {
#ifdef  _AT_SupportInclineAutoCalibrate      
      if( LCBParameter.InclineControl.Bits.AutoCalibrate == 0 ) 
          LCBStatus.bit.InitialStatus = 0 ;  
#else        
      LCBStatus.bit.InitialStatus = 0 ;
#endif      
      }  
  //--------------------------------------------------------------------
  // Check LCB Type and Set AC plugin status
  switch(LCBParameter.LCBSimulatorType) 
      {
      case _LCB1_       :
      case _LCBA_       :
      case _LCBA_M_     :
      case _LCB1x_      :
      case _LCB2_       :
                        LCBStatus.bit.ACPluginStatus = 0 ;
                        if( LCBSimulator_GetDCPluginStatus() == 1 )
                            {
                            LCBStatus.bit.ACPluginStatus = 1 ;
                            }
                        break ;  
      }
  //---------------------------------------------------------------------
  TxData.member.Start = LCBStartByte ;
  // 20210602 Add ITC
  //TxData.member.Status = LCBStatus.Full ;
  if( LCBParameter.LCBSimulatorType == _ITC )
      {
      TxData.member.Status = ITCStatus.Full ;
      if( ITCStatus.bit.Quarter_rep == 1 )
          ITCStatus.bit.Quarter_rep = 0 ;
      }
  else   
      TxData.member.Status = LCBStatus.Full ;
  //
  TxData.member.Command = TransCmd ;
  TxData.member.Length = DataLen ;
  //----------------------------------------------------------------------------
  if( DataLen > 0 )
      {
      do
          {
          TxData.member.Data[DataIndex] = *(DataPtr+DataIndex) ;
          DataIndex += 1 ;
          } while( DataIndex < DataLen ) ;
      }
  //----------------------------------------------------------------------------
  TxData.member.Data[DataIndex++] = JHTLCBComm_CRC8((unsigned char*)&TxData.Buffer[0],	DataLength) ;
  //
  TxDataLength = DataIndex + 4 ;
  TxDataPoint = 0 ;
  TxDelayTimeCounter = 0 ;
  //
  LCBStatus.bit.CommandErrorStatus = 0 ;
  return ;
}


unsigned char CheckLCB_Master_Time ;
unsigned char CheckLCB_Slave_Time ;
/*******************************************************************************
* Function Name  : 
* Description    : Must be call into the 1ms interrupt
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_Timeout(void)
{
  unsigned short TempErrorCode ;
  //----------------------------------------------------------------------------
  // Check RIS LCB Master Mode Change from UCB
  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
      {
      if( iLCM_DMS == _Master  )
          {
          CheckLCB_Slave_Time = 0 ;
          if( CommControlFlag.Bits.RIS_MasterMode == 0  )
              {
              CheckLCB_Master_Time += 1 ;
              if( CheckLCB_Master_Time > 50 ) //50ms
                  {
                  CommControlFlag.Bits.RIS_MasterMode = 1 ;
                  RIS_LCBStatus.bit.CTSStatus = 1 ;
                  }
              }
          else
              CheckLCB_Master_Time = 0 ;
          }
      else
          {
          CheckLCB_Master_Time = 0 ;
          if( CommControlFlag.Bits.RIS_MasterMode == 1  ) 
              {
              CheckLCB_Slave_Time += 1 ;
              if( CheckLCB_Slave_Time > 50 ) //50ms
                  {
                  CommControlFlag.Bits.RIS_MasterMode = 0 ;
                  RIS_LCBStatus.bit.CTSStatus = 0 ;
                  }
              }
          else
              CheckLCB_Slave_Time = 0 ;
          }
      }   
  //----------------------------------------------------------------------------
  // Transmit Delay time Calculator 
  if( CommControlFlag.Bits.RxEndWait5msReturn == 1 )
      TxDelayTimeCounter += 1 ;
  else
      TxDelayTimeCounter = 0 ;
  //----------------------------------------------------------------------------
  // Timeout and Disconnect counter 
  //if( LCBMain_GetErPStatus() == 1 || CommControlFlag.Bits.UCBOnline == 0 || LCBMain_GetResetOKStatus() == 0 || LCBMain_GetConsolePowerStatus() == 0)  // Into the ErP must be disable check rx timeout
  if( CommControlFlag.Bits.UCBOnline == 0 || LCBSimulator_GetResetOKStatus() == 0 || LCBSimulator_GetConsolePowerStatus() == 0)  // Into the ErP must be disable check rx timeout
      {
      CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 0 ;
      CommControlFlag.Bits.RxTimeoutCheckStart = 0 ;
      // 20130617
      RxDisconnectCounter = 0 ;
      UCBOfflineCounter = 0 ; 
      RxTimeoutCounter = 0 ;
      //
      }
  
  // Receive Timeout Calculator
  if( CommControlFlag.Bits.RxTimeoutCheckStart == 1 )
      {
      RxTimeoutCounter += 1 ;
      #ifdef _EnableRxTimeoutError_
      // Check Communication Disconnect
      if( CommControlFlag.Bits.ByPassTxRx == 0 )
          {
          if( RxTimeoutCounter > _RxTimeoutTime )
              {
              RxTimeoutCounter = 0 ;
              ErrorCodeStatus.bit.EB0440 = 1 ;
              }  
          }
      #endif
      }
  else
      {
      RxTimeoutCounter = 0 ;
      }
  
  if( CommControlFlag.Bits.DisconnectionTimeoutCheckStart == 1 )
      {
      if( LCBSimulator_GetConsolePowerStatus() == 1 ) // 20130611
          {
          RxDisconnectCounter += 1 ;
          #ifdef _EnableErrorCode_
          // Check Communication Disconnect
          if( RxDisconnectCounter > _RxDisconnectTime )
              {
              RxDisconnectCounter = 0 ;
              ErrorCodeStatus.bit.EC04A0 = 1 ;
              UCBOfflineCounter += 1 ;
              if( UCBOfflineCounter >= _UCBOfflineTime ) // 1.5* 40 = 60 sec.
                  {
                  UCBOfflineCounter = 0 ; // Modify by Kunlung 20130617 _UCBOfflineTime ;  
                  CommControlFlag.Bits.UCBOffline = 1 ;
                  }
              }
          #endif
          }
      // 20130611 if console power is off the disable disconnection check
      else 
          {
          CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 0 ;
          RxDisconnectCounter = 0 ;
          UCBOfflineCounter = 0 ; // 20130617
          }
      //
      }
  else
      {
      RxDisconnectCounter = 0 ;
      UCBOfflineCounter = 0 ; // 20130617
      }
   

  //----------------------------------------------------------------------------
  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
      {
      // Automatic Check Error Code ;
      if( RIS_LCBStatus.bit.LCBError == 1 )
          {
          //STATUSLED2(ON) ;
          CheckErrorCodeTimeCounter = 0 ;
          }
      else
          {
          //STATUSLED2(OFF) ;
          CheckErrorCodeTimeCounter += 1 ;
          }
      }
  else
      {
      // Automatic Check Error Code ;
      if( LCBStatus.bit.McbErrorStatus == 1 )
          {
          //STATUSLED2(ON) ;
          CheckErrorCodeTimeCounter = 0 ;
          }
      else
          {
          //STATUSLED2(OFF) ;
          CheckErrorCodeTimeCounter += 1 ;
          }
      }

  //----------------------------------------------------------------------------
  // Check Error Code occur
  if( CheckErrorCodeTimeCounter >= _CheckErrorTime  )
      {
      //------------------------------------------------------------------------
      // Add by Kunlung 20130507  
      TempErrorCode = JHTLCBComm_GetErrorCode() ; 
      if( TempErrorCode != 0 )
          {     
          // Add by KunLung 20130507  
          if( OldRetErrorCode != TempErrorCode )
              {
              if( RetErrType ==  _CLASS_C_ )
                  {
                  CheckErrorCodeDelayTimeCounter = 0 ;
                  CommControlFlag.Bits.DelayToSendErrorCode = 0 ;
                  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
                      RIS_LCBStatus.bit.LCBError = 1 ;
                  else
                      {
                      // 20210602 Add ITC
                      //LCBStatus.bit.McbErrorStatus = 1 ;
                      if( LCBParameter.LCBSimulatorType == _ITC )  
                          ITCStatus.bit.McbErrorStatus = 1 ;
                      else
                          LCBStatus.bit.McbErrorStatus = 1 ;
                      }
                  }
              else
                  {
                  if( OldRetErrorCode == 0 )
                      {
                      CheckErrorCodeDelayTimeCounter = 0 ;
                      CommControlFlag.Bits.DelayToSendErrorCode = 0 ;
                      if( CommControlFlag.Bits.RIS_LCBMode == 1 )
                          RIS_LCBStatus.bit.LCBError = 1 ;
                      else
                          {
                          // 20210602 Add ITC
                          //LCBStatus.bit.McbErrorStatus = 1 ;
                          if( LCBParameter.LCBSimulatorType == _ITC )  
                              ITCStatus.bit.McbErrorStatus = 1 ;
                          else
                              LCBStatus.bit.McbErrorStatus = 1 ;
                          }
                      }
                  else
                      {
                      if( CommControlFlag.Bits.DelayToSendErrorCode == 0 )
                          {
                          CommControlFlag.Bits.DelayToSendErrorCode = 1 ;
                          ErrorSendDelayTime = _CheckErrorSwapDelayTime ;
                          }
                      }
                  }
              }
          else
              {
              if( CommControlFlag.Bits.DelayToSendErrorCode == 0 )
                  {
                  CommControlFlag.Bits.DelayToSendErrorCode = 1 ;
                  // Modify by Kunlung 20180402
                  // ErrorSendDelayTime = _CheckErrorDelayTime ;
                  if( LCBParameter.LCBSimulatorType >= _DCI_ && LCBParameter.LCBSimulatorType <= _Delta_I2 )
                      ErrorSendDelayTime = _TMCheckErrorDelayTime ;
                  else
                      ErrorSendDelayTime = _CheckErrorDelayTime ;
                  //
                  }
              }  
          //
          if( CommControlFlag.Bits.DelayToSendErrorCode == 1 )
              {
              CheckErrorCodeDelayTimeCounter += 1 ;
              if( CheckErrorCodeDelayTimeCounter >= ErrorSendDelayTime )
                  {
                  CheckErrorCodeDelayTimeCounter = 0 ;
                  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
                      RIS_LCBStatus.bit.LCBError = 1 ;
                  else
                      {
                      // 20210602 Add ITC
                      //LCBStatus.bit.McbErrorStatus = 1 ;
                      if( LCBParameter.LCBSimulatorType == _ITC )  
                          ITCStatus.bit.McbErrorStatus = 1 ;
                      else
                          LCBStatus.bit.McbErrorStatus = 1 ;
                      }
                  CommControlFlag.Bits.DelayToSendErrorCode = 0 ;
                  }
              }
          }
      else
          {    
          // 20210602 Add ITC
          //LCBStatus.bit.McbErrorStatus = 1 ;
          if( LCBParameter.LCBSimulatorType == _ITC )  
              ITCStatus.bit.McbErrorStatus = 0 ;
          else
              LCBStatus.bit.McbErrorStatus = 0 ;
          //
          CheckErrorCodeDelayTimeCounter = 0 ;
          }
      CheckErrorCodeTimeCounter = 0 ;
      }	
  //----------------------------------------------------------------------------
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_ResetTimeoutStatus(void)
{
  if( CommControlFlag.Bits.UCBOnline == 1 )
      CommControlFlag.Bits.UCBOnline = 0  ;
  
  if( CommControlFlag.Bits.DisconnectionTimeoutCheckStart == 1 )
      {
      CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 0 ;
      RxDisconnectCounter = 0 ;
      }
  
  if( CommControlFlag.Bits.RxTimeoutCheckStart == 1 )
      {
      CommControlFlag.Bits.RxTimeoutCheckStart = 0 ;
      RxTimeoutCounter = 0 ;
      }

  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_SaveErrorCode( unsigned short ErrType , unsigned short ErrCode )
{
  unsigned short	ErrIndex ;
  unsigned short	TempMsg ;
  unsigned short	ErrClassType ;
  unsigned short	Status ;
  //
  ErrClassType = ErrType ;
  Status = 0 ;
  //--------------------------------------------------------------------------
  for( ErrIndex = 0 ; ErrIndex < _MaxErrorBufferSize ; ErrIndex++ )
      {
      if( ErrorMessage[ErrClassType][ErrIndex] == 0 )
          {
          ErrorMessage[ErrClassType][ErrIndex] = ErrCode ;
          break ;
          }
      else
          {
          if( ErrorMessage[ErrClassType][ErrIndex] == ErrCode )
              {
              Status = 1 ;
              break ;
              }
          }
      }
  //--------------------------------------------------------------------------
  if( ErrIndex >= _MaxErrorBufferSize && Status == 0 )
      {
      for( ErrIndex = 0 ; ErrIndex < (_MaxErrorBufferSize-1) ; ErrIndex++ )
          {
          TempMsg = ErrorMessage[ErrClassType][ErrIndex+1] ;
          ErrorMessage[ErrClassType][ErrIndex] = TempMsg ;
          }
      ErrorMessage[ErrClassType][4] = ErrCode ;
      }
  //-------------------------------------------------------------------------
  // 20130507 Mask
  //LCBStatus.bit.McbErrorStatus = 1 ;
  // 20130522 add
  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
      RIS_LCBStatus.bit.LCBError = 1 ;
  else
      {
      if( OldRetErrorCode == 0 || ErrCode == 0x0441 )
          {
          CommControlFlag.Bits.DelayToSendErrorCode = 0 ;
          // 20210602 Add ITC
          //LCBStatus.bit.McbErrorStatus = 1 ;
          if( LCBParameter.LCBSimulatorType == _ITC )  
              ITCStatus.bit.McbErrorStatus = 1 ;
          else
              LCBStatus.bit.McbErrorStatus = 1 ;
          //
          }
      }
  //
  if( ErrType == _CLASS_C_ )
      {
      //20121121 Add to set resistance to zero
      //SendData.EM_CurrentCommand = 0 ;
      //SendData.EM_TestPWM = 0 ;
      //UCBData.SetWatts = 0 ;
      //UCBData.SetEMCurrent = 0 ;
      //UCBData.SetEMPWM = 0 ;
      //LCBMain_SetCutOffResistance() ;
      //
      LCBParameter.TargetPWM = 0 ;
      LCBParameter.TargetWatts = 0 ;
      LCBParameter.TargetEMCurrent = 0 ;
      LCBParameter.LimitRpmForResistance = 0 ;
      LCBParameter.ClimbmillStatus.Bits.Start = 0 ;
      CommControlFlag.Bits.ClimbmillStart = 0 ;
      CommControlFlag.Bits.ClimbmillStop = 0 ;
      /*
      if( Status == 0 )
          {
          ErrorReportData.Para.ErrorCode = ErrCode ;
          ErrorReportData.Para.LCB_Model = _BikeEP_ ;
          //
          if( LCBStatus.bit.ACPluginStatus == 1 )
              ErrorReportData.Para.LCB_ACIn = 1 ;
          else
              ErrorReportData.Para.LCB_ACIn = 0 ;
          //
          ErrorReportData.Para.Battery_Voltage = LCBADC.BatteryVoltage ;
          ErrorReportData.Para.DCBus_Voltage = LCBADC.GeneratorVoltage ;
          ErrorReportData.Para.DCBus_Current = LCBADC.GeneratorCurrent ;
          EEPROM_ErrorReportSave() ;
          }
      */
      }
  //--------------------------------------------------------------------------
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short JHTLCBComm_GetErrorCode(void)
{
  unsigned char i ;
  //--------------------------------------------------------------------------
  if( ErrorMessage[_CLASS_C_][0] != 0 )
      {
      RetErrType = _CLASS_C_ ;
      if( ErrorMessage[_CLASS_C_][GetClassCErr] != 0 )
          {
          if( GetClassCErr < (_MaxErrorBufferSize-1) )
              {
              for( i = 0 ; i < _MaxErrorBufferSize ;i++ )
                  {	
                  if( ErrorMessage[_CLASS_C_][GetClassCErr] != 0 )
                      {
                      RetErrorCode = ErrorMessage[_CLASS_C_][GetClassCErr] ;
                      break ;
                      }
                  else
                      {
                      if( GetClassCErr != 0 )
                          GetClassCErr -= 1 ;
                      }		
                  }		
              //
              if( GetClassCErr != (_MaxErrorBufferSize-1) && ErrorMessage[_CLASS_C_][GetClassCErr+1] != 0 )
                  GetClassCErr += 1 ;	
              // Add by Kunlung 20180503
              else
                  GetClassCErr = 0 ; 
              //----------------------------------------------------------------
              }
          else
              {
              GetClassCErr = 0 ;		
              RetErrorCode = ErrorMessage[_CLASS_C_][GetClassCErr] ;
              }
          }
      }
  else
      {
      GetClassCErr = 0 ; // add by  20110713
      if( ErrorMessage[_CLASS_B_][0] != 0 )
          {
          RetErrType = _CLASS_B_ ;
          RetErrorCode = ErrorMessage[_CLASS_B_][0] ;
          }
      else
          {
          if( ErrorMessage[_CLASS_A_][0] != 0 )
              {
              RetErrType = _CLASS_A_ ;
              RetErrorCode = ErrorMessage[_CLASS_A_][0] ;
              }
          else
              {
              RetErrType = _NoErrorCode_ ;
              RetErrorCode = 0 ;
              }
          }
      }
  //--------------------------------------------------------------------------
  return RetErrorCode ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_SkipErrorCode(void)
{
  unsigned char	ErrIndex ;
  unsigned short TempMsg ;
  //--------------------------------------------------------------------------
  //
  if( RetErrType < _NoErrorCode_ ) // Add by Kunlung Fixed Bug
      {
      if( RetErrType <  _CLASS_C_ || RetErrorCode == 0x04A0 ) 
          {
          for( ErrIndex = 0 ; ErrIndex < (_MaxErrorBufferSize-1) ; ErrIndex++ )
              {
              TempMsg = ErrorMessage[RetErrType][ErrIndex+1] ;
              ErrorMessage[RetErrType][ErrIndex] = TempMsg ;
              }
          ErrorMessage[RetErrType][4] = 0 ;
          }
      }
  //
  if( CommControlFlag.Bits.RIS_LCBMode == 1 )
      RIS_LCBStatus.bit.LCBError = 0 ;
  else
      {
      // 20210602 Add ITC
      //LCBStatus.bit.McbErrorStatus = 0 ;
      if( LCBParameter.LCBSimulatorType == _ITC )  
          ITCStatus.bit.McbErrorStatus = 0 ;
      else
          LCBStatus.bit.McbErrorStatus = 0 ;
      }
  //--------------------------------------------------------------------------
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_SkipAnyErrorCode(unsigned short ECode)
{
  unsigned char	ErrIndex ;
  unsigned short TempMsg ;
  //--------------------------------------------------------------------------
  //
  for( ErrIndex = 0 ; ErrIndex < (_MaxErrorBufferSize-1) ; ErrIndex++ )
      {
      if( ErrorMessage[_CLASS_C_][ErrIndex] == ECode )
          {
          TempMsg = ErrorMessage[_CLASS_C_][ErrIndex+1] ;
          ErrorMessage[_CLASS_C_][ErrIndex] = TempMsg ;
          }
      }
  ErrorMessage[_CLASS_C_][4] = 0 ;
  //
  //--------------------------------------------------------------------------
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_ErrorMessageProcess(void)
{	
        
  //----------------------------------------------------------------------------
  // Class C
  if( ErrorCodeStatus.bit.EC04A0 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x04A0 ) ; 
      }
  //----------------------------------------------------------------------------
  // Treadmill EStop Action
  if( ErrorCodeStatus.bit.EC02B2 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x02B2 ) ; 
      //ErrorCodeStatus.bit.EC02B2 = 0 ;
      }    
  #ifndef   _DisableMachineSetError_
  if( ErrorCodeStatus.bit.EC02AB == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x02AB ) ; 
      }
  
  if( ErrorCodeStatus.bit.EC02B4 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x02B4 ) ; 
      }
  #endif
  //Battery connector reverse
  if( ErrorCodeStatus.bit.EC01B4 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x01B4 ) ; 
      }
  
  if( ErrorCodeStatus.bit.EC01AC == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x01AC ) ; 
      }
  
  if( ErrorCodeStatus.bit.EC01AF == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_C_, 0x01AF ) ; 
      }
  //----------------------------------------------------------------------------
  // Class B
  if( ErrorCodeStatus.bit.EB014A == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_, 0x014A ) ; 
      ErrorCodeStatus.bit.EB014A = 0 ;
      }
  
  if( ErrorCodeStatus.bit.EB0248 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_, 0x0248 ) ; 
      ErrorCodeStatus.bit.EB0248 = 0 ;
      }
  
  if( ErrorCodeStatus.bit.EB0247 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_, 0x0247 ) ; 
      ErrorCodeStatus.bit.EB0247 = 0 ;
      }
  
  if( ErrorCodeStatus.bit.EB0140 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_, 0x0140 ) ;
      ErrorCodeStatus.bit.EB0140 = 0 ;
      }  
  /*
  if( ErrorCodeStatus.bit.EB0441 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_,0x0441 ) ; 
      ErrorCodeStatus.bit.EB0441 = 0 ;
      }
  if( ErrorCodeStatus.bit.EB0442 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_,0x0442 ) ;  
      ErrorCodeStatus.bit.EB0442 = 0 ;
      }
  */
  if( ErrorCodeStatus.bit.EB0144 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_, 0x0144 ) ;
      ErrorCodeStatus.bit.EB0144 = 0 ;
      }
  if( ErrorCodeStatus.bit.EB0146 == 1 )
      {
      JHTLCBComm_SaveErrorCode( _CLASS_B_, 0x0146 ) ;
      ErrorCodeStatus.bit.EB0146 = 0 ;
      }  
  //----------------------------------------------------------------------------
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_ClearAllErrorMessage(unsigned char ClearB)
{
  unsigned char Idx ;
  for( Idx = 0 ; Idx < _MaxErrorBufferSize ; Idx++ )
      {
      ErrorMessage[_CLASS_A_][Idx] = 0 ;
      ErrorMessage[_CLASS_B_][Idx] = 0 ;
      ErrorMessage[_CLASS_C_][Idx] = 0 ;
      }
  // Clear Status Bit
  ErrorCodeStatus.bit.EC04A0 = 0 ;
  // Modify by Kunlung 20180402 Simulator Real E-Stop Release flowchart
  //ErrorCodeStatus.bit.EC02B2 = 0 ;
  if( LCBSystemProcessStatus.B.sTreadMillEStop == 1 )
      TMReleaseEstopStatus = _RelaseESTOP70 ;
  else
      ErrorCodeStatus.bit.EC02B2 = 0 ;
  //
  //---------------------------------------------------------------------------
  //LCBMain_ClearCutOffResistance() ; // 20130724 to Cut off resistance flag
  // Modify by Kunlung 20130507
  if( ClearB == 1 )
      {
      ErrorCodeStatus.bit.EC01B4 = 0 ;
      ErrorCodeStatus.bit.EB014A = 0 ;
      ErrorCodeStatus.bit.EB0248 = 0 ;
      ErrorCodeStatus.bit.EB0247 = 0 ;
      }
  //
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_GetUCBOfflineStatus(void)   
{
  if( CommControlFlag.Bits.UCBOffline == 1 )
      return 1 ;
  return 0 ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_GetUCBOnlineStatus(void)   
{
  if( CommControlFlag.Bits.UCBOnline == 1 )
      return 1 ;
  return 0 ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void RxFunc_Init(void)
{
  RxStartPoint = RxEndPoint = &RxBuffer[0] ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char* RxFunc_NextPt(unsigned char *pt)
{
  return ((pt - &RxBuffer[0]) < (RxSize-1))?(pt+1):&RxBuffer[0] ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char RxFunc_IsEmpty(void)
{
  return (RxStartPoint == RxEndPoint)?1:0;
} 


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char RxFunc_IsFull(void)
{
  return (RxStartPoint == RxFunc_NextPt(RxEndPoint))?1:0 ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short RxFunc_Length(void)
{ 
  if(RxStartPoint <= RxEndPoint ) 
      return RxEndPoint - RxStartPoint ; 
  else 
      return RxSize - (RxStartPoint - RxEndPoint); 
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void RxFunc_Putc(unsigned char c)
{ 
  if(RxStartPoint == RxFunc_NextPt(RxEndPoint)) 
      return ; 
  *RxEndPoint = c; 
  RxEndPoint = RxFunc_NextPt(RxEndPoint) ;
} 


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char RxFunc_Getc(void)
{ 
  unsigned char result=0; 
  
  if(RxEndPoint != RxStartPoint)
      { 
      result = *RxStartPoint; 
      RxStartPoint = RxFunc_NextPt(RxStartPoint) ;
      } 
  
  return result;
} 


// 20120204 Add Tx FIFO Control
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TxFunc_Init(void)
{
  TxStartPoint = TxEndPoint = &TxBuffer[0] ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char* TxFunc_NextPt(unsigned char *pt)
{
  return ((pt - &TxBuffer[0]) < (TxSize-1))?(pt+1):&TxBuffer[0] ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char TxFunc_IsEmpty(void)
{
  return (TxStartPoint == TxEndPoint)?1:0;
} 


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char TxFunc_IsFull(void)
{
  return (TxStartPoint == TxFunc_NextPt(TxEndPoint))?1:0 ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short TxFunc_Length(void)
{ 
  if(TxStartPoint <= TxEndPoint ) 
      return TxEndPoint - TxStartPoint ; 
  else 
      return TxSize - (TxStartPoint - TxEndPoint); 
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TxFunc_Putc(unsigned char c)
{ 
  if(TxStartPoint == TxFunc_NextPt(TxEndPoint)) 
      return ; 
  *TxEndPoint = c; 
  TxEndPoint = TxFunc_NextPt(TxEndPoint) ;
} 


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char TxFunc_Getc(void)
{ 
  unsigned char result=0; 
  
  if(TxEndPoint != TxStartPoint)
      { 
      result = *TxStartPoint; 
      TxStartPoint = TxFunc_NextPt(TxStartPoint) ;
      } 
  
  return result;
} 



// Add for 2017 Indoor Cycle LCB
void JHTLCBComm_RIS_HW_Initial(void)
{
  USART_InitTypeDef 	USART_InitStructure;
  NVIC_InitTypeDef 	NVIC_InitStructure;

  //
  NVIC_InitStructure.NVIC_IRQChannel = _JHT_COMM_IRQ;//Channel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);	
  USART_DeInit(_JHT_COMM_PORT) ;

  // Initial RxData ;
  JHTLCBComm_Initial() ;
  CommControlFlag.Bits.RIS_LCBMode = 1 ;
  
  //----------------------------------------------------------------------------
  USART_InitStructure.USART_BaudRate = 115200 ;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  // Configure USART2 
  USART_Init(_JHT_COMM_PORT, &USART_InitStructure);
  // Enable JHTUARTPORT Receive interrupt 
  USART_ITConfig(_JHT_COMM_PORT, USART_IT_RXNE, ENABLE);
  // Enable JHTUARTPORT Transmit interrupt 
  USART_ITConfig(_JHT_COMM_PORT,USART_IT_TC,ENABLE);
  //
  USART_Cmd(_JHT_COMM_PORT, ENABLE);
  //
  // Enable the USART2 Interrupt 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
  //
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_RIS_RxProcess(void)
{
  //
  unsigned char *pt1;
  unsigned short i ;
  unsigned short Length ; //modify 20220216 char to short for serial numer write issue.
  unsigned char CheckSum ;
  unsigned char RetData[_PackageLength_MaxDataLegth] ;

  // RIS LCB Master Mode
  if( CommControlFlag.Bits.RIS_MasterMode == 1 )
      {
      if( JHTLCBComm_CalculatorTimeTick(RISMasterModeTime) >= RISLCBParameter.StreamRate )
          {
          RISMasterModeTime = JHTLCBCommSystemTime ;
          Length = JHTLCBComm_RISSensorData(&RetData[0]) ;
          JHTLCBComm_RISReturnData( _CmdStreamSensorData, &RetData[0], Length ) ;
          CommControlFlag.Bits.RxEndWait5msReturn = 0 ;
          CommControlFlag.Bits.TxAction = 1 ; // 20130618
          oRS485Rx(_TXD) ;
          CommControlFlag.Bits.DirectorRXD = 0 ;
          TxDelayTimeCounter = 0 ;
          TxDataPoint = 1 ;
          USART_SendData(_JHT_COMM_PORT,RISTxData.Buffer[0]) ;
          }
      return ;
      }
  //
  if( CommControlFlag.Bits.RxEndWait5msReturn == 0 )
      {
      //---------------------------------------------------------------------------
      // Check Receive Buffer
      if(RxFunc_IsEmpty())
          {
          return ;
          }
      
      if( RxFunc_Length() > 6 )  
          {      
          RISRxAnalsys = (RISPacketFileType *)RxStartPoint ;
          // Check Start Byte
          if( RISRxAnalsys->member.StartWord != _RIS_START_WORD )
              {
              RxFunc_Getc();    // drop byte	
              return ;
              }      
          //
          if( RxFunc_Length() > (RISRxAnalsys->member.DateLength+6))
              {          
              //
              pt1 = RxStartPoint +6 ; 
              Length = RISRxAnalsys->member.DateLength ;
              // Move Data to Buffer  
              for( i = 0 ; i < Length ; i++ )
                  {
                  RISRxAnalsys->member.Data[i] = *pt1 ;
                  pt1 = RxFunc_NextPt(pt1) ;
                  }
              //------------------------------------------------------------
              Length = RISRxAnalsys->member.DateLength+6 ;
              CheckSum = *pt1 ;
              if( CheckSum == JHTLCBComm_CRC8( &RISRxAnalsys->Buffer[0],Length ) )
                  {
                  //--------------------------------------------------------
                  CommControlFlag.Bits.RxTimeoutCheckStart = 0 ;
                  CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 0 ;                            
                  UCBOfflineCounter = 0  ;        // Clear UCB Offline Counter
                  RxTimeoutCounter = 0 ;
                  RxDisconnectCounter = 0 ;
                  CommControlFlag.Bits.UCBOnline = 1 ;
                  CommControlFlag.Bits.UCBOffline = 0 ;
                  //20130816
                  if( ErrorCodeStatus.bit.EC04A0 == 1 )
                      {
                      ErrorCodeStatus.bit.EC04A0 = 0 ;
                      //JHTLCBComm_SkipErrorCode04A0() ;
                      }
                  //----------------------------------------------------------
                  if( CommControlFlag.Bits.DisableErPOnCmd == 0 )
                      CommControlFlag.Bits.DisconnectionTimeoutCheckStart = 1 ;
                  
                  //---------------------------------------------------------
                  JHTLCBComm_RISCommandDecoder( RISRxAnalsys->member.MsgID ) ;
                  // Clear buffer
                  for( i = 0 ; i < (Length+7) ; i++ )
                      {
                      RxFunc_Getc();    
                      }
                  //
                  CommControlFlag.Bits.RxEndWait5msReturn = 1 ;
                  //CommControlFlag.Bits.TxAction = 1 ; // 20130618
                  //--------------------------------------------------------
                  // Set Status LED Blink
                  CommControlFlag.Bits.StatusLED = ~CommControlFlag.Bits.StatusLED ;
                  //--------------------------------------------------------
                  }
              else
                  {
                  RxFunc_Getc();    // drop byte
                  RxFunc_Getc();    // drop byte
                  CommControlFlag.Bits.RxEndWait5msReturn = 0 ;
                  CommControlFlag.Bits.TxAction = 0 ;
                  }
              }
          }
      }
  else
      {
      // Delay 5ms to Response to UCB
      if( TxDelayTimeCounter > 5 ) 
          {
          CommControlFlag.Bits.RxEndWait5msReturn = 0 ;
          CommControlFlag.Bits.TxAction = 1 ; // 20130618
          oRS485Rx(_TXD) ;
          CommControlFlag.Bits.DirectorRXD = 0 ;
          TxDelayTimeCounter = 0 ;
          TxDataPoint = 1 ;
          USART_SendData(_JHT_COMM_PORT,RISTxData.Buffer[0]) ;
          }
      }
  //----------------------------------------------------------------------------
  return ;
}



#ifdef _SupportPhoenixConsole
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_RISCommandDecoder(unsigned char ECmd )
{
#if 1
  static unsigned short TestErpCount = 0 ; // for Test
#endif 
  unsigned char RetDataLength ;
  static unsigned char RetData[_PackageLength_MaxDataLegth] ;
  unsigned char Command ;
  unsigned char SubCommand ;
//  
  union {
    struct {
      unsigned short Low:8 ;
      unsigned short High:8 ;
    } By ;
    unsigned short All ;
  } TempData[2] ;
  // 20210413
  unsigned short WLength ;
  unsigned short Size ;
  //----------------------------------------------------------------------------  
  // Default 0
  RetDataLength = 0 ;
  RetData[0] = 0 ;
  TempData[0].All = 0 ;
  TempData[1].All = 0 ;
  //
  Command = ECmd ;
  SubCommand = RISRxAnalsys->member.Data[0] ;  
  //----------------------------------------------------------------------------
//Add 20210203  
#ifdef  _LogUCBCommandData_  
  JHTLCMComm_LogUCBCommand(Command,SubCommand) ;
#endif   
  //----------------------------------------------------------------------------
  if( JHTLCBComm_CheckRISCommandAndLength(Command,RISRxAnalsys->member.DateLength) == 1 )
      {
      //
      if( RISRxAnalsys->member.DateLength >= 2 )
          {
          TempData[0].By.High = RISRxAnalsys->member.Data[0] ; // High byte
          TempData[0].By.Low = RISRxAnalsys->member.Data[1] ;  // Low byte
          }
      // 
      if( RISRxAnalsys->member.DateLength >= 4 )
          {
          TempData[1].By.High = RISRxAnalsys->member.Data[2] ; // High byte
          TempData[1].By.Low = RISRxAnalsys->member.Data[3] ;  // Low byte    
          }
      //------------------------------------------------------------------------
      switch( Command )
          {
          // 0
          case  _CmdGetLCBInfo                      : //0x01  //  0     4       Get LCB type and version
                                                    RetDataLength = 4 ;
                                                    //Modify 20210413
                                                    //RetData[0] = 0 ;
                                                    /* Modify 20220216
                                                    if( LCBParameter.LCBSimulatorType == _Rower )
                                                        RetData[0] = _SDrive ; // Rower LCB & S-Drive LCB
                                                    else
                                                        RetData[0] = LCBParameter.LCBSimulatorType ;
                                                    //------------------------------------------------------------------------------------------
                                                    RetData[1] = RISLCBParameter.ManchineType ;
                                                    RetData[2] = 0 ;
                                                    RetData[3] = 1 ; 
                                                    */      
                                                    RetData[0] = LCBParameter.LCBSimulatorType ;
                                                    RetData[1] = RISLCBParameter.ManchineType ;
                                                    RetData[2] = 0 ;
                                                    RetData[3] = 1 ; 
                                                    switch( LCBParameter.LCBSimulatorType )
                                                        {
                                                        case  _SDrive     :
                                                                          RetData[2] = 1 ;
                                                                          RetData[3] = 2 ; 
                                                                          break ;
                                                        case  _Rower      :
                                                                          RetData[0] = _SDrive ; // Rower LCB & S-Drive LCB
                                                                          RetData[2] = 1 ;
                                                                          RetData[3] = 2 ; 
                                                                          break;
                                                        case  _CxpUpdate  :     
                                                                          RetData[0] = _UBCM ;   // 0x03: CXP updates  / UBCM(Upper body cycle for led console) 
                                                                          RetData[2] = 2 ;
                                                                          RetData[3] = 5 ; 
                                                                          break;
                                                        default           :
                                                                          break ;
                                                        }

                                                    break ;
          case  _CmdGetSensorData                   : //0x02  //  0     Varies  This is used to manually get the sensor data in Slave mode.          
                                                    RetDataLength = JHTLCBComm_RISSensorData(&RetData[0]) ;
                                                    break ;
          case  _CmdInitialize                      : //0x04  //  0     0       Initialize the LCB
                                                    ErrorCodeStatus.bit.EC01AC = 0 ;
                                                    ErrorCodeStatus.bit.EC01AF = 0 ;
                                                    ErrorCodeStatus.bit.EC02AB = 0 ;
                                                    ErrorCodeStatus.bit.EC02B4 = 0 ;
                                                    JHTLCBComm_ClearAllErrorMessage(1) ;            
                                                    break ;
          case  _CmdGetStatus                       : //0x05  //  0     0       This message will be used to return the status byte
                                                    break ;
          case  _CmdGetPositionSensorCalibration    : //0x08  //  0     4       This message will return the min and max calibration values for the position sensor
                                                    RetDataLength = 4 ;
                                                    TempData[0].All = RISLCBParameter.ResistancePositionMin ;
                                                    TempData[1].All = RISLCBParameter.ResistancePositionMax ;
                                                    RetData[0] = TempData[0].By.High ;
                                                    RetData[1] = TempData[0].By.Low ;
                                                    RetData[2] = TempData[1].By.High ;
                                                    RetData[3] = TempData[1].By.Low ;
                                                    break ;
          case  _CmdGetErrorCode                    : //0x20  //  0     2       Get the LCB most recent error code
                                                    RetDataLength = 2 ;
                                                    RetData[0] = JHTLCBComm_GetErrorCode()/0x100 ;
                                                    RetData[1] = JHTLCBComm_GetErrorCode()%0x100 ; ;        
                                                    break ;
          case  _CmdClearErrorCode                  : //0x21  //  0     0       Clear the LCB most recent error code     
                                                    JHTLCBComm_SkipErrorCode();
                                                    break ;
          case  _CmdEnterOrCheckLCBErpMode          : //0x07  //  1     1       This message will be used to enter ERP mode or check the LCB ERP status
                                                    // 0x00: LCB not support ERP mode function.
                                                    // 0x01: LCB can into ERP mode
                                                    // 0x02: LCB can¡¦t into ERP mode. Please try later.

                                                    TestErpCount += 1 ;
                                                    RetDataLength = 1 ;
                                                    /*
                                                    if( RISLCBParameter.RPM == 0 )
                                                        {
                                                        }
                                                    */
                                                    if( RISRxAnalsys->member.Data[0] == 0xFF )
                                                        {                                                    
                                                        RetData[0] = RISLCBParameter.EntryToErPMode ;
                                                        }
                                                    else if( RISRxAnalsys->member.Data[0] == 0x00)
                                                        {
                                                        RetData[0] = _WakeupOK ;
                                                        }
                                                    else
                                                        RetData[0] = _NoSuuprtErp ;   
                                                    break ;
          case  _CmdEnterLCBUpdateMode              : //0x80  //  1     1       UCB sent 0x01 (LCB app block) to LCB. 
                                                    // Then LCB shall return 0x01 (success) or 0x00 (fail) if the LCB supports the program update function            
                                                    RetDataLength = 1 ;
                                                    RetData[0] = 0 ;            
                                                    break ;		
          case  _CmdSetDataStreamRate               : //0x03  //  2     0       This is used to modify the stream rate of for the Stream Sensor Data message. The default is 50ms.
                                                    // Modify 20210413
                                                    //RISLCBParameter.StreamRate = TempData[0].All ;
                                                    if( TempData[0].All >= 50 )
                                                        RISLCBParameter.StreamRate = TempData[0].All ;
                                                    else
                                                        RISLCBParameter.StreamRate = 50 ; // Default 50ms
                                                    break ;
          case  _CmdTurnOffBattery                  : //0x06  //  2     0       This message will be used to turn off the battery
                                                    RISLCBParameter.BatteryOffDelayTime = TempData[0].All ;
                                                    RISBatteyOFFTime = (unsigned long)RISLCBParameter.BatteryOffDelayTime * 60000 ;
                                                    LCBSystemProcessStatus.B.sRISVbatOFFDelayTimeCheck = 1 ;
                                                    break ;	    
          case  _CmdSetPositionSensorCalibration    : //0x09  //  4     0       This message will set the min and max calibration values for the position sensor
                                                    RISLCBParameter.ResistancePositionMin = TempData[0].All ;
                                                    RISLCBParameter.ResistancePositionMax = TempData[1].All ;
                                                    EE93CXX_WriteDataToEeprom(_EER_PosMin,2,(unsigned char*)&RISLCBParameter.ResistancePositionMin) ;
                                                    EE93CXX_WriteDataToEeprom(_EER_PosMax,2,(unsigned char*)&RISLCBParameter.ResistancePositionMax) ;
                                                    break ;	
          case  _CmdSetSensorFunctionSetting        : //0x0A  //  2     0       Enable/disable sensor and LCB functionality.  
                                                    RIS_SensorFunctionStatus.Full = TempData[0].All ;
                                                    break ;
          case  _CmdGetSensorFunctionSetting        : //0x0B  //  0     2       Get the sensor/function setting for message 0x0A
                                                    RetDataLength = 1 ;                                                    
                                                    TempData[0].All = RIS_SensorFunctionStatus.Full ;
                                                    RetData[0] = TempData[0].By.High ;
                                                    RetData[1] = TempData[0].By.Low ;  
                                                    break ;
          //--------------------------------------------------------------------
          //Add 20180111  
          case  _CmdSetCalibrateSensors             : //0x0C  //  2     1       Set the calibration of sensors
                                                    RetDataLength = 1 ;
                                                    RetData[0] = 0 ;
                                                    break ;
          case  _CmdSetConsoleWatts                 : //0x0D  //  2     0       Let LCB know the power consumption of currently connected console  
                                                    RISLCBParameter.ConsoleWatts = TempData[0].All ;          
                                                    break ;
          case  _CmdSetPowerTableParameters         : //0x81  //  60    0       Set the parameters of power table, which the machine is calibrated by dyno machine.                                                
                                                    break ;
          case  _CmdGetPowerTableParameters         : //0x82  //  0     60      Get the parameters of power table, which the machine is calibrated by dyno machine,from LCB.  
                                                    for( RetDataLength = 0 ; RetDataLength < 60 ; RetDataLength++ )
                                                        {
                                                        if((RetDataLength % 4) == 0 )
                                                            RetData[RetDataLength] = PowerTableParameters.All[RetDataLength+3] ;
                                                        else if((RetDataLength % 4) == 1 )
                                                            RetData[RetDataLength] = PowerTableParameters.All[RetDataLength+1] ;
                                                        else if((RetDataLength % 4) == 2 )
                                                            RetData[RetDataLength] = PowerTableParameters.All[RetDataLength-1] ;
                                                        else
                                                            RetData[RetDataLength] = PowerTableParameters.All[RetDataLength-3] ;
                                                        }
                                                    break ;
          //--------------------------------------------------------------------
          // Add 20210413
          case  _CmdSerialNumber                    ://0x0E  
                                                    RetDataLength = 3 ;
                                                    RetData[0]=SubCommand ;
                                                    RetData[1] = 0 ;
                                                    RetData[2] = 0 ;
                                                    switch( SubCommand )
                                                        {
                                                        case CmdEraseSerialNumber   :// 0x00  //  3     3  
                                                                                    break ;
                                                        case CmdGetMemoryLength     :// 0x03  //  3     3
                                                                                    RetData[2] = _MaxLengthOfSerialNumber ;
                                                                                    break ;                                                         
                                                        case CmdReadSerialNumber    :// 0x02  //  3     3+N     N <= 252
                                                                                    RetData[2] = MachineSerialNumberLength ;
                                                                                    for(RetDataLength = 0; RetDataLength < MachineSerialNumberLength ; RetDataLength++)
                                                                                        RetData[3+RetDataLength]=MachineSerialNumber[RetDataLength] ; 
                                                                                    RetDataLength = 3 + MachineSerialNumberLength ;
                                                                                    break ;
                                                        case CmdWriteSerialNumber   :// 0x01  //  3+N   3       N <= 252
                                                                                    MachineSerialNumberLength = RISRxAnalsys->member.Data[2] ;
                                                                                    for(RetDataLength = 0; RetDataLength < MachineSerialNumberLength ; RetDataLength++)
                                                                                        MachineSerialNumber[RetDataLength] = RISRxAnalsys->member.Data[3+RetDataLength]; 
                                                                                    RetDataLength = 3 ;
                                                                                    LCBSimulator_SetSerialNumber(1) ;
                                                                                    break ;
                                                        }
                                                    break ;   
          case  _CmdSetStepMotorPosition            ://0x11  //  4     0
                                                    LCBParameter.MotorPosition.Byte[0]=RISRxAnalsys->member.Data[3] ;
                                                    LCBParameter.MotorPosition.Byte[1]=RISRxAnalsys->member.Data[2] ;
                                                    LCBParameter.MotorPosition.Byte[2]=RISRxAnalsys->member.Data[1] ;
                                                    LCBParameter.MotorPosition.Byte[3]=RISRxAnalsys->member.Data[0] ;
                                                    RISLCBParameter.Level = LCBSimulator_MotorPosToResistanceLevel(LCBParameter.MotorPosition.Full) ;
                                                    break ;            
          case  _CmdSetParameter                    ://0x83  //  1+4*N 0
                                                    LCBParameter.Calculation.NunberOfParameter = RISRxAnalsys->member.Data[0] ;            
                                                    Size = (unsigned short)LCBParameter.Calculation.NunberOfParameter*4 ;
                                                    if( Size > _AthenaMaxSize )
                                                        Size = _AthenaMaxSize ;
                                                    
                                                    for( WLength = 0 ; WLength < Size ; WLength+=4)
                                                        {
                                                        LCBParameter.Calculation.Parameter[WLength+0] = RISRxAnalsys->member.Data[1+WLength] ;
                                                        LCBParameter.Calculation.Parameter[WLength+1] = RISRxAnalsys->member.Data[2+WLength] ;
                                                        LCBParameter.Calculation.Parameter[WLength+2] = RISRxAnalsys->member.Data[3+WLength] ;
                                                        LCBParameter.Calculation.Parameter[WLength+3] = RISRxAnalsys->member.Data[4+WLength] ;
                                                        }
                                                    break ;            
          case  _CmdGetParameter                    ://0x84  //  0     1+4*N
                                                    // 20200707
                                                    if( LCBParameter.Calculation.NunberOfParameter == 0 )
                                                        LCBParameter.Calculation.NunberOfParameter = 21 ;
                                                    //
                                                    Size = (unsigned short)LCBParameter.Calculation.NunberOfParameter*4 ;
                                                    RetData[0] = LCBParameter.Calculation.NunberOfParameter ;
                                                    for( WLength = 0 ; WLength < Size ; WLength++ )
                                                        RetData[1+WLength] = LCBParameter.Calculation.Parameter[WLength] ;
                                                    
                                                    RetDataLength = 1+Size ;
                                                    break ;            
          case  _CmdSetActionSpinDown               ://0x85  //  1+N   1
                                                    RetData[0] = 0 ; // High byte
                                                    RetDataLength = 1 ;              
                                                    break ;            
          case  _CmdGetActionSpinDown               ://0x86  //  0     15
                                                    for( WLength = 0 ; WLength < 15 ; WLength++ )
                                                        RetData[WLength] = 0 ;
                                                    RetDataLength = WLength ;
                                                    break ;            
          case  _CmdSetErgModeSettings              ://0x87  //  1+4*N 0
                                                    LCBParameter.Erg.NunberOfParameter = RISRxAnalsys->member.Data[0] ;
                                                    Size = (unsigned short)LCBParameter.Erg.NunberOfParameter*4 ;
                                                    if( Size > _AthenaMaxSize )
                                                        Size = _AthenaMaxSize ;
                                                    
                                                    for( WLength = 0 ; WLength < Size ; WLength+=4)
                                                        {
                                                        LCBParameter.Erg.Parameter[WLength+0] = RISRxAnalsys->member.Data[1+WLength] ;
                                                        LCBParameter.Erg.Parameter[WLength+1] = RISRxAnalsys->member.Data[2+WLength] ;
                                                        LCBParameter.Erg.Parameter[WLength+2] = RISRxAnalsys->member.Data[3+WLength] ;
                                                        LCBParameter.Erg.Parameter[WLength+3] = RISRxAnalsys->member.Data[4+WLength] ;
                                                        }                                                    
                                                    break ;            
          case  _CmdGetErgModeSettings              ://0x88  //  0     1+4*N
                                                    // 20200707
                                                    if( LCBParameter.Erg.NunberOfParameter == 0 )
                                                        LCBParameter.Erg.NunberOfParameter = 7 ;
                                                    //
                                                    Size = (unsigned short)LCBParameter.Erg.NunberOfParameter*4 ;
                                                    RetData[0] = LCBParameter.Erg.NunberOfParameter ;
                                                    for( WLength = 0 ; WLength < Size ; WLength++ )
                                                        RetData[1+WLength] = LCBParameter.Erg.Parameter[WLength] ;
                                                    
                                                    RetDataLength = 1+Size ;            
                                                    break ;            
          case  _CmdSetResistanceLevel              ://0x10  //  1     0     
                                                    RISLCBParameter.Level = RISRxAnalsys->member.Data[0] ;
                                                    if( RISLCBParameter.Level >= 26 )
                                                        RISLCBParameter.Level = 25 ;
                                                    LCBParameter.MotorPosition.Member.Position = ResistanceTable[RISLCBParameter.Level] ;
                                                    LCBParameter.MotorPosition.Member.Mode = 4 ; // 1/16
                                                    break ; 
          // Add 20220216
          case  _CmdSetFeatureAndDevice             ://0x0F  // 10    10      To designate the requested features and devices to LCB. 
                                                    RetDataLength = 10 ;
                                                    RetData[0] = SubCommand ;
                                                    if( SubCommand == 0x01 )  // for write
                                                        {
                                                        for( WLength = 0 ; WLength < 8 ; WLength++ )
                                                            RISLCBParameter.FeatureAndDevice[WLength] = RISRxAnalsys->member.Data[WLength+1] ;
                                                        }
                                                    // Return data
                                                    for( WLength = 0 ; WLength < 8 ; WLength++ )
                                                        RetData[WLength+1] = RISLCBParameter.FeatureAndDevice[WLength] ;
                                                    //
                                                    break ;
          //------------------------------------------------------------------------------------------------------------------------------------                                                    
          default				    :
                                                    break ;
          }
      //----------------------------------------------------------------------------
      }
  else
      {
      // unKnown command process
      RetData[0] = Command ;
      Command = _CmdUnknownCommand ;
      RetDataLength = 1 ;
      }
  //----------------------------------------------------------------------------
  JHTLCBComm_RISReturnData( Command, &RetData[0], RetDataLength ) ;
  //----------------------------------------------------------------------------
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_CheckRISCommandAndLength(unsigned char CheckCmd,unsigned char CheckLen )
{
  unsigned char RetStatus = 2 ;
  //
  switch( CheckCmd )
      {
      // 0
      case  _CmdGetLCBInfo                      : //0x01  //  0     4       Get LCB type and version
      case  _CmdGetSensorData                   : //0x02  //  0     Varies  This is used to manually get the sensor data in Slave mode.          
      case  _CmdInitialize                      : //0x04  //  0     0       Initialize the LCB
      case  _CmdGetStatus                       : //0x05  //  0     0       This message will be used to return the status byte
      case  _CmdGetPositionSensorCalibration    : //0x08  //  0     4       This message will return the min and max calibration values for the position sensor
      case  _CmdGetErrorCode                    : //0x20  //  0     2       Get the LCB most recent error code
      case  _CmdClearErrorCode                  : //0x21  //  0     0       Clear the LCB most recent error code        
      case  _CmdGetSensorFunctionSetting        : //0x0B  //  0     2       Get the sensor/function setting for message 0x0A
      //Add 20180111
      case  _CmdGetPowerTableParameters         : //0x82  //  0     60      Get the parameters of power table, which the machine is calibrated by dyno machine,from LCB.  
      //Add 20210413
      case  _CmdGetParameter                    : //0x84  //  0     Varies
      case  _CmdGetActionSpinDown               : //0x86  //  0     19
      case  _CmdGetErgModeSettings              : //0x88  //  0     Varies         
                                                if( CheckLen == 0 )
                                                    RetStatus = 1 ;
                                                break ;
      // 1 byte                                                
      case  _CmdEnterOrCheckLCBErpMode          : //0x07  //  1     1       This message will be used to enter ERP mode or check the LCB ERP status
      case  _CmdEnterLCBUpdateMode              : //0x80  //  1     1       UCB sent 0x01 (LCB app block) to LCB. Then LCB shall return 0x01 (success) or 0x00 (fail) if the LCB supports the program update function
      //Add 20210413   
      case  _CmdSetResistanceLevel              : //0x10  //  1     0          
                                                if( CheckLen == 1 )
                                                    RetStatus = 1 ;                                                
                                                break ;		
      // 2 byte                                                
      case  _CmdSetDataStreamRate               : //0x03  //  2     0       This is used to modify the stream rate of for the Stream Sensor Data message. The default is 50ms.
      case  _CmdTurnOffBattery                  : //0x06  //  2     0       This message will be used to turn off the battery
      case  _CmdSetSensorFunctionSetting        : //0x0A  //  2     0       Enable/disable sensor and LCB functionality.  
      //Add 20180111
      case  _CmdSetCalibrateSensors             : //0x0C  //  2     1       Set the calibration of sensors
      case  _CmdSetConsoleWatts                 : //0x0D  //  2     0       Let LCB know the power consumption of currently connected console        
      //case  _CmdSetWattes                       : //0x13  //  2     0              
                                                if( CheckLen == 2 )
                                                    RetStatus = 1 ;	      
                                                break ;	    
      // 4 byte   
      case  _CmdSetPositionSensorCalibration    : //0x09  //  4     0       This message will set the min and max calibration values for the position sensor
      //Add 20210413
      case  _CmdSetStepMotorPosition            : //0x11  //  4     0        
                                                if( CheckLen == 4 )
                                                    RetStatus = 1 ;	      
                                                break ;	
      // Add 20210413                                                
      case  _CmdSerialNumber                    : //0x0E  //  3~N, N=0~255  To save serial number from the console                                              
      case  _CmdSetParameter                    : //0x83  //  Varies  0  
      case  _CmdSetErgModeSettings              : //0x87  //  Varies  0  
      case  _CmdSetActionSpinDown               : //0x85  //  1~5     1  
                                                RetStatus = 1 ;	
                                                break ;        
      // Add 20180111                                                
      // 60 byte
      case  _CmdSetPowerTableParameters         : //0x81  //  60    0       Set the parameters of power table, which the machine is calibrated by dyno machine.                                                
                                                if( CheckLen == 60 )
                                                    RetStatus = 1 ;	
                                                break ;
      // Add 20220216
      case  _CmdSetFeatureAndDevice             ://0x0F   //  10  10        To designate the requested features and devices to LCB.
                                                if( CheckLen == 10 )
                                                    RetStatus = 1 ;
                                                break ;
      default		                        :        
                                                RetStatus = 0 ;
                                                break ;
      }	
 
  return RetStatus ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void JHTLCBComm_RISReturnData( unsigned char TransCmd, unsigned char *DataPtr, unsigned char DataLen )
{
  unsigned short DataLength ;
  unsigned char DataIndex ;
  //
  DataLength = 6 + DataLen  ;
  DataIndex = 0 ;
  //----------------------------------------------------------------------------
  RISTxData.member.StartWord = _RIS_START_WORD ;
  // Check Battery Status < 0.8V(80)
  if( RISLCBParameter.BatteryVoltage < 80 )
      {
      RIS_LCBStatus.bit.BatteryLowStatus = 1 ;
      }
  else
      {
      RIS_LCBStatus.bit.BatteryLowStatus = 0 ;
      }  
  //----------------------------------------------------------------------------
  // Add 20210415 
  RIS_LCBStatus.bit.Source = 0x01 ; // From LCB
  
  //----------------------------------------------------------------------------
  RISTxData.member.Status = RIS_LCBStatus.Full ;
  RISTxData.member.SeqNumber = RISLCBParameter.RIS_SeqNumber++ ;
  RISTxData.member.MsgID = TransCmd ;
  RISTxData.member.DateLength = DataLen ;
  //----------------------------------------------------------------------------
  if( DataLen > 0 )
      {
      do
          {
          RISTxData.member.Data[DataIndex] = *(DataPtr+DataIndex) ;
          DataIndex += 1 ;
          } while( DataIndex < DataLen ) ;
      }
  //----------------------------------------------------------------------------
  RISTxData.member.Data[DataIndex++] = JHTLCBComm_CRC8((unsigned char*)&RISTxData.Buffer[0],	DataLength) ;
  //
  TxDataLength = DataIndex + 6 ;
  TxDataPoint = 0 ;
  TxDelayTimeCounter = 0 ;
  //
  return ;
}

          
          
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned char JHTLCBComm_RISSensorData( unsigned char *DataPtr )
{
  unsigned char RetLength = 0;
  unsigned short i ;
  static unsigned short CountSentToUCB = 0;
  unsigned char by_dat, by_dat_2;
  // 20210413 UBCP
  AthenaStepperMotor Temp ;  
  //
#if _V3
  *(DataPtr + RetLength++) = RISLCBParameter.ManchineType ;
  *(DataPtr + RetLength++) = (JHTLCBComm_GetErrorCode() / 0x100) ;
  *(DataPtr + RetLength++) = (JHTLCBComm_GetErrorCode() % 0x100) ;  
  switch(RISLCBParameter.ManchineType)
      {
      case  _RIS_Rower          :
                                //---------------------------------------------------------------
                                // Machine Type         1 unsigned char This is the LCB type. This value will be 0 [0 : rower, 1: s-drive, 2: indoor cycle]
                                // Heart Rate           1 unsigned char This is the heart rate read from the 5khz board. Return 0 if not available
                                // Battery Voltage      2 unsigned char This is the voltage of battery in voltage.
                                // Handle State         1 unsigned char This is the state of the rower handle [0: Stopped,1: Drive - nominally accelerating,2: Recovery - nominally decelerating]
                                // Watts                2 unsigned int Watts in .1 watts for the last stroke
                                // Drag Factor          2 unsigned int This is the drag factor computed for the last stroke. (TDB units)
                                // Handle Data Count    1 unsigned char This is the number of handle data points pair (X * 4 bytes)
                                //                        Note: the force and distance should be the actual x,y coordinate that will be used to plot the point on a
                                //                        force curve and not just the amount produced in some time. So the distance is the total rope pulled and not the difference.
                                // Handle Force n       2 unsigned int This is the acceleration computed in increments of .1 kgf (ie in the first 25ms period)
                                // Handle Distance n    2 unsigned int This is the accumulated distance of the rope traveled in .1 mm occurred (ie in the first 25ms period.)        
                                //RetLength = 12+(RISLCBParameter.HandleDataCount*4) ;
                                //if( RISLCBParameter.HeartRate > 40 )
                                *(DataPtr + RetLength++) = RISLCBParameter.HeartRate ;
                                //else
                                //    *(DataPtr + RetLength++) = 0 ;
                                *(DataPtr + RetLength++) = RISLCBParameter.RPM ;//(unsigned char)RowerWatts_Parameters(_Get, _RowerWatts_SPM, 0);
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage % 0x100) ;
                                *(DataPtr + RetLength++) = RowerAction.HandleState ;                                
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_WattsX10, 0)/ 0x100);
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_WattsX10, 0)% 0x100);
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_DragFactor, 0) / 0x100);
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_DragFactor, 0) % 0x100); 
                                if(CountSentToUCB > RowerAction.HandleData_Count)
                                    CountSentToUCB = 0;
                                by_dat = RowerAction.HandleData_Count - CountSentToUCB;
                                *(DataPtr + RetLength++) = by_dat ;   
                                by_dat_2 = CountSentToUCB ;
                                //j = 12 ;
                                for( i=by_dat_2; i<(by_dat+by_dat_2); i++)
                                    {//_PackageLength_MaxDataLegth
                                    if((RetLength + 4) > _PackageLength_MaxDataLegth)
                                        {
                                        *(DataPtr + 8) = i - by_dat_2; 
                                        break;
                                        }
                                    else
                                        {
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Force[i]/ 0x100);
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Force[i]% 0x100);
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Distance[i]/ 0x100);
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Distance[i]% 0x100);
                                        CountSentToUCB++;
                                        }
                                    }
                                // 20180111 Add Generator RPM
                                *(DataPtr + RetLength++) = (RISLCBParameter.GeneratorRPM/ 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.GeneratorRPM% 0x100);
                                //----------------------------------------------------------------
                                break ;
      case  _RIS_SDrive         :
                                //RetLength = 12 ; // 20170608                               
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM % 0x100) ;                              

                                if( RIS_SensorFunctionStatus.bit.LoadCell1 == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell1 / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell1 % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                //
                                if( RIS_SensorFunctionStatus.bit.LoadCell2 == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell2 / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell2 % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                //
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage % 0x100) ; 
                                *(DataPtr + RetLength++) = RISLCBParameter.HeartRate ; 
                                // 20180111 Add Generator RPM
                                *(DataPtr + RetLength++) = (RISLCBParameter.GeneratorRPM/ 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.GeneratorRPM% 0x100);
                                break ;
// Add 20210413 Upper Body Cycle                                
      case  _RIS_UpBodyCycle    :   
                                if( LCBParameter.LCBSimulatorType == _UBCP )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.RPM / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.RPM % 0x100) ;
                                    if( LCBParameter.CrankRPM != 0 )
                                      *(DataPtr + RetLength++) = (RISLCBParameter.Direction ) ; //Direction 1: Forward 2: Reverse
                                    else
                                        *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.FlywheelRPM/ 0x100);
                                    *(DataPtr + RetLength++) = (RISLCBParameter.FlywheelRPM% 0x100);
                                    // Add 20171115 Watts .01
                                    *(DataPtr + RetLength++) = (RISLCBParameter.Watts / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.Watts % 0x100) ;   
                                    // 2
                                    *(DataPtr + RetLength++) = (LCBParameter.MotorStatus.Full/ 0x100);
                                    *(DataPtr + RetLength++) = (LCBParameter.MotorStatus.Full% 0x100);                                    
                                    // 1
                                    *(DataPtr + RetLength++) = LCBParameter.MotorPosition.Member.Mode ;
                                    // 3
                                    *(DataPtr + RetLength++) = LCBParameter.MotorPosition.Byte[2] ;
                                    *(DataPtr + RetLength++) = LCBParameter.MotorPosition.Byte[1] ;
                                    *(DataPtr + RetLength++) = LCBParameter.MotorPosition.Byte[0] ;
                                    // 3
                                    Temp.Full = LCBSimulator_GetMotorPosMax(LCBParameter.MotorPosition.Member.Mode) ;
                                    *(DataPtr + RetLength++) = Temp.Byte[2] ;
                                    *(DataPtr + RetLength++) = Temp.Byte[1] ;
                                    *(DataPtr + RetLength++) = Temp.Byte[0] ;                                    
                                    // 1
                                    *(DataPtr + RetLength++) = RISLCBParameter.Level ;                                      
                                    break ;
                                    }
//-------------------------------------------------------------------------                                
      case  _RIS_IndoorCycle    : //Indoor cycle & UBCM)
      default                   :
                                //RetLength = 11 ;
                                if( RIS_SensorFunctionStatus.bit.Torque == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.Torque / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.Torque % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM % 0x100) ;
                                if( RIS_SensorFunctionStatus.bit.Resistanceposition == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.ResistancePosition / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.ResistancePosition % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage % 0x100) ;   
                                // Add 20171115 Watts .01
                                *(DataPtr + RetLength++) = (RISLCBParameter.Watts / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.Watts % 0x100) ;  
                                // 20180111 Add Generator RPM
                                *(DataPtr + RetLength++) = (RISLCBParameter.GeneratorRPM/ 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.GeneratorRPM% 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.FlywheelRPM/ 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.FlywheelRPM% 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.ElectronicsWatts/ 0x100);
                                *(DataPtr + RetLength++) = (RISLCBParameter.ElectronicsWatts% 0x100);
                                *(DataPtr + RetLength++) = RISLCBParameter.Level ;
                                
                                break ;
      }  
#elif _V2  
  *(DataPtr + RetLength++) = RISLCBParameter.ManchineType ;
  *(DataPtr + RetLength++) = (JHTLCBComm_GetErrorCode() / 0x100) ;
  *(DataPtr + RetLength++) = (JHTLCBComm_GetErrorCode() % 0x100) ;  
  switch(RISLCBParameter.ManchineType)
      {
      case  _RIS_Rower          :
                                //---------------------------------------------------------------
                                // Machine Type         1 unsigned char This is the LCB type. This value will be 0 [0 : rower, 1: s-drive, 2: indoor cycle]
                                // Heart Rate           1 unsigned char This is the heart rate read from the 5khz board. Return 0 if not available
                                // Battery Voltage      2 unsigned char This is the voltage of battery in voltage.
                                // Handle State         1 unsigned char This is the state of the rower handle [0: Stopped,1: Drive - nominally accelerating,2: Recovery - nominally decelerating]
                                // Watts                2 unsigned int Watts in .1 watts for the last stroke
                                // Drag Factor          2 unsigned int This is the drag factor computed for the last stroke. (TDB units)
                                // Handle Data Count    1 unsigned char This is the number of handle data points pair (X * 4 bytes)
                                //                        Note: the force and distance should be the actual x,y coordinate that will be used to plot the point on a
                                //                        force curve and not just the amount produced in some time. So the distance is the total rope pulled and not the difference.
                                // Handle Force n       2 unsigned int This is the acceleration computed in increments of .1 kgf (ie in the first 25ms period)
                                // Handle Distance n    2 unsigned int This is the accumulated distance of the rope traveled in .1 mm occurred (ie in the first 25ms period.)        
                                //RetLength = 12+(RISLCBParameter.HandleDataCount*4) ;
                                //if( RISLCBParameter.HeartRate > 40 )
                                *(DataPtr + RetLength++) = RISLCBParameter.HeartRate ;
                                //else
                                //    *(DataPtr + RetLength++) = 0 ;
                                *(DataPtr + RetLength++) = RISLCBParameter.RPM ;//(unsigned char)RowerWatts_Parameters(_Get, _RowerWatts_SPM, 0);
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage % 0x100) ;
                                *(DataPtr + RetLength++) = RowerAction.HandleState ;                                
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_WattsX10, 0)/ 0x100);
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_WattsX10, 0)% 0x100);
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_DragFactor, 0) / 0x100);
                                *(DataPtr + RetLength++) = (RowerWatts_Parameters(_Get, _RowerWatts_DragFactor, 0) % 0x100); 
                                if(CountSentToUCB > RowerAction.HandleData_Count)
                                    CountSentToUCB = 0;
                                by_dat = RowerAction.HandleData_Count - CountSentToUCB;
                                *(DataPtr + RetLength++) = by_dat ;   
                                by_dat_2 = CountSentToUCB ;
                                //j = 12 ;
                                for( i=by_dat_2; i<(by_dat+by_dat_2); i++)
                                    {//_PackageLength_MaxDataLegth
                                    if((RetLength + 4) > _PackageLength_MaxDataLegth)
                                        {
                                        *(DataPtr + 8) = i - by_dat_2; 
                                        break;
                                        }
                                    else
                                        {
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Force[i]/ 0x100);
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Force[i]% 0x100);
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Distance[i]/ 0x100);
                                        *(DataPtr + RetLength++) = (RowerAction.HandleData_Distance[i]% 0x100);
                                        CountSentToUCB++;
                                        }
                                    }
                                //----------------------------------------------------------------
                                break ;
      case  _RIS_SDrive         :
                                //RetLength = 12 ; // 20170608                               
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM % 0x100) ;                              

                                if( RIS_SensorFunctionStatus.bit.LoadCell1 == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell1 / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell1 % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                //
                                if( RIS_SensorFunctionStatus.bit.LoadCell2 == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell2 / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.LoadCell2 % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                //
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage % 0x100) ; 
                                *(DataPtr + RetLength++) = RISLCBParameter.HeartRate ; 
                                break ;
      case  _RIS_IndoorCycle    :        
      default                   :
                                //RetLength = 11 ;
                                if( RIS_SensorFunctionStatus.bit.Torque == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.Torque / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.Torque % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.RPM % 0x100) ;
                                if( RIS_SensorFunctionStatus.bit.Resistanceposition == 1 )
                                    {
                                    *(DataPtr + RetLength++) = (RISLCBParameter.ResistancePosition / 0x100) ;
                                    *(DataPtr + RetLength++) = (RISLCBParameter.ResistancePosition % 0x100) ;
                                    }
                                else
                                    {
                                    *(DataPtr + RetLength++) = 0 ;
                                    *(DataPtr + RetLength++) = 0 ;
                                    }
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.BatteryVoltage % 0x100) ;   
                                // Add 20171115 Watts .01
                                *(DataPtr + RetLength++) = (RISLCBParameter.Watts / 0x100) ;
                                *(DataPtr + RetLength++) = (RISLCBParameter.Watts % 0x100) ;  
                                break ;
      }
#else
  switch(RISLCBParameter.ManchineType)
      {
      case  _RIS_Rower          :
                                //---------------------------------------------------------------
                                // Machine Type         1 unsigned char This is the LCB type. This value will be 0 [0 : rower, 1: s-drive, 2: indoor cycle]
                                // Heart Rate           1 unsigned char This is the heart rate read from the 5khz board. Return 0 if not available
                                // Battery Voltage      2 unsigned char This is the voltage of battery in voltage.
                                // Handle State         1 unsigned char This is the state of the rower handle [0: Stopped,1: Drive - nominally accelerating,2: Recovery - nominally decelerating]
                                // Watts                2 unsigned int Watts in .1 watts for the last stroke
                                // Drag Factor          2 unsigned int This is the drag factor computed for the last stroke. (TDB units)
                                // Handle Data Count    1 unsigned char This is the number of handle data points pair (X * 4 bytes)
                                //                        Note: the force and distance should be the actual x,y coordinate that will be used to plot the point on a
                                //                        force curve and not just the amount produced in some time. So the distance is the total rope pulled and not the difference.
                                // Handle Force n       2 unsigned int This is the acceleration computed in increments of .1 kgf (ie in the first 25ms period)
                                // Handle Distance n    2 unsigned int This is the accumulated distance of the rope traveled in .1 mm occurred (ie in the first 25ms period.)        
                                RetLength = 10+(RISLCBParameter.HandleDataCount*4) ;
                                *(DataPtr+0) = RISLCBParameter.ManchineType ;                              
                                *(DataPtr+1) = RISLCBParameter.HeartRate ;
                                *(DataPtr+2) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr+3) = (RISLCBParameter.BatteryVoltage % 0x100) ;
                                *(DataPtr+4) = RISLCBParameter.HandleState ;                                
                                *(DataPtr+5) = (RISLCBParameter.Watts / 0x100) ;
                                *(DataPtr+6) = (RISLCBParameter.Watts % 0x100) ;
                                *(DataPtr+7) = (RISLCBParameter.DragFactor / 0x100) ;
                                *(DataPtr+8) = (RISLCBParameter.DragFactor % 0x100) ; 
                                *(DataPtr+9) = RISLCBParameter.HandleDataCount ;    
                                j = 10 ;
                                for( i = 0 ; i < RISLCBParameter.HandleDataCount ; i++ )
                                    {
                                    *(DataPtr+j++) = (RISLCBParameter.HandleForce[i] / 0x100) ;
                                    *(DataPtr+j++) = (RISLCBParameter.HandleForce[i] % 0x100) ;  
                                    *(DataPtr+j++) = (RISLCBParameter.HandleDistance[i] / 0x100) ;
                                    *(DataPtr+j++) = (RISLCBParameter.HandleDistance[i] % 0x100) ; 
                                    }
                                //----------------------------------------------------------------
                                break ;
      case  _RIS_SDrive         :
                                RetLength = 10 ; // 20170608
                                *(DataPtr+0) = RISLCBParameter.ManchineType ;
                                *(DataPtr+1) = (RISLCBParameter.RPM / 0x100) ;
                                *(DataPtr+2) = (RISLCBParameter.RPM % 0x100) ;
                                *(DataPtr+3) = 0 ;
                                *(DataPtr+4) = 0 ;                                
                                *(DataPtr+5) = 0 ;
                                *(DataPtr+6) = 0 ; 
                                if( RIS_SensorFunctionStatus.bit.LoadCell1 == 1 )
                                    {
                                    *(DataPtr+3) = (RISLCBParameter.LoadCell1 / 0x100) ;
                                    *(DataPtr+4) = (RISLCBParameter.LoadCell1 % 0x100) ;
                                    }
                                //
                                if( RIS_SensorFunctionStatus.bit.LoadCell2 == 1 )
                                    {
                                    *(DataPtr+5) = (RISLCBParameter.LoadCell2 / 0x100) ;
                                    *(DataPtr+6) = (RISLCBParameter.LoadCell2 % 0x100) ;
                                    }
                                //
                                *(DataPtr+7) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr+8) = (RISLCBParameter.BatteryVoltage % 0x100) ; 
                                *(DataPtr+9) = RISLCBParameter.HeartRate ; 
                                break ;
      case  _RIS_IndoorCycle    :        
      default                   :
                                RetLength = 9 ;
                                *(DataPtr+0) = RISLCBParameter.ManchineType ;
                                *(DataPtr+1) = 0 ;
                                *(DataPtr+2) = 0 ;
                                *(DataPtr+5) = 0 ;
                                *(DataPtr+6) = 0 ;
                                if( RIS_SensorFunctionStatus.bit.Torque == 1 )
                                    {
                                    *(DataPtr+1) = (RISLCBParameter.Torque / 0x100) ;
                                    *(DataPtr+2) = (RISLCBParameter.Torque % 0x100) ;
                                    }
                                *(DataPtr+3) = (RISLCBParameter.RPM / 0x100) ;
                                *(DataPtr+4) = (RISLCBParameter.RPM % 0x100) ;
                                if( RIS_SensorFunctionStatus.bit.Resistanceposition == 1 )
                                    {
                                    *(DataPtr+5) = (RISLCBParameter.ResistancePosition / 0x100) ;
                                    *(DataPtr+6) = (RISLCBParameter.ResistancePosition % 0x100) ;
                                    }
                                *(DataPtr+7) = (RISLCBParameter.BatteryVoltage / 0x100) ;
                                *(DataPtr+8) = (RISLCBParameter.BatteryVoltage % 0x100) ;        
                                break ;
      }  
#endif  
  return RetLength ;  
}

#endif
          
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void JHTLCBComm_SystemTime(void) 
{
  JHTLCBCommSystemTime += 1 ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short JHTLCBComm_CalculatorTimeTick( unsigned short last_TimeTick )
{
  unsigned short TempTick ;
  if( JHTLCBCommSystemTime < last_TimeTick )
      TempTick = ( 0xFFFF - last_TimeTick ) + JHTLCBCommSystemTime ;
  else  
      TempTick = JHTLCBCommSystemTime - last_TimeTick ;
  //
  return TempTick ;
}
          
          
          
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void JHTLCBComm_log0441Command(unsigned char LogCMD, unsigned char LogCMD1)
{
  unsigned short TmpData ;
  unsigned char i ;
  TmpData = (unsigned short)LogCMD*0x100+LogCMD1 ;
  for( i = 0 ; i < Log0441Index ; i++ )
      {
      if(Log0441Cmd[i] == TmpData )
          {
          break ;
          }
      }
  //
  if( i >= Log0441Index )
      {
      Log0441Cmd[Log0441Index] = TmpData ;
      Log0441Index += 1 ;
      if(Log0441Index > (_Log0441CmdSize-1) )
          Log0441Index = (_Log0441CmdSize-1) ;
      }
  //
  return ;
}
          
          
          
//------------------------------------------------------------------------------          
#ifdef  DebugMonitor

unsigned char DebugOut( void )
{
  if( CommControlFlag.Bits.TxAction == 0 )
      {
      memset(&TxData.Buffer[0],0,260);// clear
      sprintf((char*)&TxData.Buffer[0],"GV:%05d,GA:%05d,EA:%05d,1DC12V:%05d,PWM:%05d,Freq:%05d\r\n",FeedBackAdcData.GeneratorVoltage\
              ,FeedBackAdcData.GeneratorCurrent,FeedBackAdcData.ADC_ElectroMagnetCurrent,FeedBackAdcData.DC12Voltage,FeedBackAdcData.ErrorCode,FeedBackAdcData.RPM_Freq);
      TxDataLength = strlen((char const*)&TxData.Buffer[0]);//==>­pºâªø«×
      oRS485Rx(TXD) ;
      CommControlFlag.Bits.DirectorRXD = 0 ;
      TxDelayTimeCounter = 0 ;
      TxDataPoint = 1 ;
      USART_SendData(JHTUARTPORT,TxData.Buffer[0]) ;
      CommControlFlag.Bits.TxAction = 1 ;
      return 1 ;
      }
  return 0 ;
}
#endif
//------------------------------------------------------------------------------

