//##############################################################################


//##############################################################################
/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#ifdef  _PRODUCT_BOARD
#include  "ADS1100.h"
#include  "LCMDisplay.h"
#include  "ADC.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define   _ShowADS1100_Debug            1
#define   _ADS1100FilterDataSize        10



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADS1100Config ADS1100ConfigReg ;
ADS1100Value  ADS1100Data[_ADS1100FilterDataSize] ;
ADS1100Value  ADS1100DebugData ;
ADS1100Value  ADS1100FilterValue ;
unsigned char ADS1100FilterDataCounter ;
unsigned short ADS1100SystemTime ;
unsigned short ADS1100PorcessTime ;
unsigned short ADS1100_Voltage ;
unsigned short ADS1100_Current ;
float ADS1100_offset ;

/* Private function prototypes -----------------------------------------------*/
void ADS1100_GPIO_Iinitial(void) ;
void ADS1100_TWI_START(void) ;
void ADS1100_TWI_STOP(void) ;
void ADS1100_TWI_ACK(void) ;
void ADS1100_TWI_NACK(void) ;
unsigned char ADS1100_TWI_WAIT_ACK(void) ;
void ADS1100_WriteByte(unsigned char oData) ;
unsigned char ADS1100_TWI_ReadByte(void) ;
void ADS1100_Write(unsigned char oData ) ;
unsigned short ADS1100_Read(void) ;
void ADS1100_FilterBubbleSort( unsigned short* iSource, unsigned char Length ) ;
unsigned short ADS1100_FilterAverage( unsigned short* iSource, unsigned char Length ) ;
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_Initial(void)
{
  //
  ADS1100_GPIO_Iinitial() ;
  ADS1100ConfigReg.All = ADS1110_CONFIG_REG ;
  ADS1100_Write(ADS1100ConfigReg.All) ;
  ADS1100SystemTime = 0 ;
  ADS1100FilterDataCounter = 0 ;
  ADS1100_offset = 0.98542024 ;
  //
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_GPIO_Iinitial(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  //
  oADS1100_SCL(Bit_SET) ;
  oADS1100_SDA(Bit_SET) ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SCL ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_Init(_ADS1100_SCL_GPIO_GROUP, &GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure);  
  //
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_TWI_START(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;  
  oADS1100_SDA(Bit_SET) ;
  // Set GPIO Output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure); 
  // 
#if __ICCARM__
  oADS1100_SCL(Bit_SET) ;  
  asm("nop") ;
  asm("nop") ;
  oADS1100_SDA(Bit_RESET) ;
  asm("nop") ;
  asm("nop") ;
  oADS1100_SCL(Bit_RESET) ;
  asm("nop") ;
  asm("nop") ;
#elif __GNUC__
  oADS1100_SCL(Bit_SET) ;  
  __asm("nop") ;
  __asm("nop") ;
  oADS1100_SDA(Bit_RESET) ;
  __asm("nop") ;
  __asm("nop") ;
  oADS1100_SCL(Bit_RESET) ;
  __asm("nop") ;
  __asm("nop") ;
#endif
  //
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_TWI_STOP(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;  
  oADS1100_SDA(Bit_RESET) ;
  // Set GPIO Output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure); 
  //  
#if __ICCARM__
  oADS1100_SCL(Bit_SET) ;
  asm("nop") ;
  asm("nop") ;
  oADS1100_SDA(Bit_SET) ;
  asm("nop") ;
  asm("nop") ;
#elif __GNUC__
  oADS1100_SCL(Bit_SET) ;
  __asm("nop") ;
  __asm("nop") ;
  oADS1100_SDA(Bit_SET) ;
  __asm("nop") ;
  __asm("nop") ;
#endif
  //
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_TWI_ACK(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure; 
  //
  oADS1100_SDA(Bit_RESET) ;
  // Set GPIO Output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure); 
  // 
  oADS1100_SDA(Bit_RESET) ;
  oADS1100_SCL(Bit_SET) ;
#if __ICCARM__
  asm("nop") ;
  asm("nop") ;
  asm("nop") ;
  asm("nop") ;
#elif __GNUC__
  __asm("nop") ;
  __asm("nop") ;
  __asm("nop") ;
  __asm("nop") ;
#endif
  oADS1100_SCL(Bit_RESET) ;
  //
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_TWI_NACK(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;  
  //
  oADS1100_SDA(Bit_SET) ; 
  // Set GPIO Output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure); 
  //   
  oADS1100_SCL(Bit_RESET) ;
  oADS1100_SCL(Bit_SET) ;  
#if __ICCARM__
  asm("nop") ;
  asm("nop") ;
  asm("nop") ;
  asm("nop") ;
#elif __GNUC__
  __asm("nop") ;
  __asm("nop") ;
  __asm("nop") ;
  __asm("nop") ;
#endif 
  oADS1100_SCL(Bit_RESET) ;
  //
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char ADS1100_TWI_WAIT_ACK(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure; 
  unsigned char State ;
  //
  //Set GPIO input
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure);
  //
#if __ICCARM__
  oADS1100_SCL(Bit_RESET) ;
  oADS1100_SCL(Bit_SET) ;
  asm("nop");
  State = iADS1100_SDA  ; 
  oADS1100_SCL(Bit_RESET) ;
  asm("nop"); 
  asm("nop");
  asm("nop");
  asm("nop");
#elif __GNUC__
  oADS1100_SCL(Bit_RESET) ;
  oADS1100_SCL(Bit_SET) ;
  __asm("nop");
  State = iADS1100_SDA  ; 
  oADS1100_SCL(Bit_RESET) ;
  __asm("nop"); 
  __asm("nop");
  __asm("nop");
  __asm("nop");
#endif
  //
  return State ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_WriteByte(unsigned char oData)
{
  GPIO_InitTypeDef          GPIO_InitStructure; 
  unsigned char i ;
  union {
    struct {
      unsigned char D0_6:7 ;
      unsigned char D7:1 ;
    } Bit ;
    unsigned char Byte ;
  } Data ;
  // Set GPIO Output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure);  
  Data.Byte = oData ;
  for( i = 0 ; i < 8 ; i++ )
      {  
      //
      if( Data.Bit.D7 == 1 )
          oADS1100_SDA(Bit_SET) ;
      else
          oADS1100_SDA(Bit_RESET) ; 
#if __ICCARM__
      asm("nop") ;
      asm("nop") ;
      oADS1100_SCL(Bit_SET) ;
      asm("nop") ;
      asm("nop") ;
      oADS1100_SCL(Bit_RESET) ;
#elif __GNUC__
      __asm("nop") ;
      __asm("nop") ;
      oADS1100_SCL(Bit_SET) ;
      __asm("nop") ;
      __asm("nop") ;
      oADS1100_SCL(Bit_RESET) ;
#endif
      Data.Byte <<= 1 ;
      }  
  //
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char ADS1100_TWI_ReadByte(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure; 
  unsigned char i,rData = 0;
  unsigned char j ;
  //
  //Set GPIO input
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
  GPIO_InitStructure.GPIO_Pin = _ADS1100_SDA ;
  GPIO_Init(_ADS1100_SDA_GPIO_GROUP, &GPIO_InitStructure);
  for( i = 0 ; i < 8 ; i++ )
      {   
      rData <<= 1 ;
      oADS1100_SCL(Bit_SET) ;
      for( j = 0 ; j < 10 ;j++ )
#if __ICCARM__
          asm("nop") ;        
#elif __GNUC__
          __asm("nop") ;
#endif

      if( iADS1100_SDA == Bit_SET )
          rData |= 0x01 ;       
      else
          rData &= 0xfe ;
      oADS1100_SCL(Bit_RESET) ;
#if __ICCARM__
      asm("nop") ;
      asm("nop") ;
      asm("nop") ;
#elif __GNUC__
      __asm("nop") ;
      __asm("nop") ;
      __asm("nop") ;
#endif
      }
  //
  return rData ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_Write(unsigned char oData )
{
  ADS1100_TWI_START();
  ADS1100_WriteByte(ADS1110_WR_ADDRESS);
  ADS1100_TWI_WAIT_ACK();
  ADS1100_WriteByte(oData);
  ADS1100_TWI_WAIT_ACK();
  ADS1100_TWI_STOP();
  return ;
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADS1100_Read(void)
{
  union {
    struct {
      unsigned short Lo:8 ;
      unsigned short Hi:8 ;
    } Byte ;
    unsigned short Word ;
  } rData ;
  //
  ADS1100_TWI_START();
  ADS1100_WriteByte(ADS1110_RD_ADDRESS);
  ADS1100_TWI_WAIT_ACK();  
  rData.Byte.Hi = ADS1100_TWI_ReadByte() ;
  ADS1100_TWI_ACK();
  rData.Byte.Lo = ADS1100_TWI_ReadByte() ;
  ADS1100_TWI_ACK();  
  ADS1100ConfigReg.All = ADS1100_TWI_ReadByte() ;
  ADS1100_TWI_NACK();
  ADS1100_TWI_STOP();
  //
  return rData.Word ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_SystemTime(void) 
{
  ADS1100SystemTime += 1 ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADS1100_CalculatorTimeTick( unsigned short last_TimeTick )
{
  unsigned short TempTick ;
  if( ADS1100SystemTime < last_TimeTick )
      TempTick = ( 0xFFFF - last_TimeTick ) + ADS1100SystemTime ;
  else  
      TempTick = ADS1100SystemTime - last_TimeTick ;
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
void ADS1100_Process(void)
{

  float TempData ;
  //
  if( ADS1100_CalculatorTimeTick(ADS1100PorcessTime) > 10 )
      {     
      //ADS1100_Write(ADS1110_CONFIG_REG) ;
      //asm("nop") ;
      ADS1100DebugData.All = ADS1100_Read() ;
      if( ADS1100DebugData.Data.Sign == 0 )
          {
          TempData = ADS1100DebugData.Data.Vlaue ;
          ADS1100_Voltage =(unsigned short)( TempData / 32767 * 3.3 * 1250 * ADS1100_offset ) ; // 0.0001v 1250 = 10000/8
          ADS1100_Current = (unsigned short)(( float)ADS1100_Voltage / 0.05 ) ;//* 100 ) ; //0.0001A
          }
      else
          {
          ADS1100_Voltage = 0 ;
          ADS1100_Current = 0 ;
          }
#ifdef _ShowADS1100_Debug
      // for test
      //LCMDisplay_show_word_bcd(0,0,ADS1100DebugData.Data.Vlaue,5) ;  
      LCMDisplay_show_word_bcd(7,0,ADS1100_Voltage,9) ;
      LCMDisplay_show_word_bcd(0,1,ADS1100_Current,9) ;
      LCMDisplay_show_word_bcd(0,0,ADC_GetDC12Voltage(),8) ;      
#endif      
      ADS1100PorcessTime = ADS1100SystemTime ; 
      //
      }
  
  
  //
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADS1100_FilterBubbleSort( unsigned short* iSource, unsigned char Length ) 
{
  unsigned char x , y ;
  unsigned short Temp ;
  //
  if( Length <= 1 )
      return ;
  //
  for( x = 0 ; x < Length ; x++ )
      {
      for( y = (x+1) ; y < Length ; y++ )
          {
          if( iSource[x] > iSource[y] )
              {
              Temp = iSource[x] ;
              iSource[x] = iSource[y] ;
              iSource[y] = Temp ;
              }
          }
      }		
  //	
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADS1100_FilterAverage( unsigned short* iSource, unsigned char Length ) 
{
  unsigned long iAverage = 0;
  unsigned char i = 0;
  //---------------------------------------------------------------------------- 
  for( i = 0 ; i < Length ; i++ )
      {
      iAverage = iAverage + (unsigned long)iSource[i];
      }
  //----------------------------------------------------------------------------
  return ((unsigned short)(iAverage/Length));
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADS1100_GetADCValue(unsigned char Mode)
{
  unsigned short ReturnData ;
  //
  if( Mode == 1 )
      {
      ReturnData = ADS1100DebugData.All ;
      }
  //
  return ReturnData ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADS1100_GetCurrentValue(void)
{
  return ADS1100_Current ;
}



#endif
