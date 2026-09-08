
#include "PinDefine.h"  
#include "LCM.h"


LCMStatusFormat LCMStatus ;

void LCM_Clear(void) ;
void LCM_CheckBusy( unsigned int dt ) ;
#ifdef	_LCMDataSize4  
void LCM_OutData( unsigned char lcd_data,unsigned char Mode ) ;
unsigned char LCM_InData(unsigned char Mode) ;
#else
void LCM_OutData( unsigned char lcd_data ) ;
unsigned char LCM_InData( void ) ;
#endif

typedef union {
  struct{
    unsigned char B0:1 ;
    unsigned char B1:1 ;
    unsigned char B2:1 ;
    unsigned char B3:1 ;
    unsigned char B4:1 ;
    unsigned char B5:1 ;
    unsigned char B6:1 ;
    unsigned char B7:1 ;
  } bit ;
  unsigned char All ;
} TransByte2Bit ;


volatile unsigned short LCM_DelayTimeCount ;

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_HW_Initial(void) 
{

  //
  IO_LCM_POWER(_LCM_OFF) ;
  IO_LCM_BL(_BL_OFF) ;
  IO_LCM_E(Bit_RESET); 
  IO_LCM_RW(Bit_SET) ;
  IO_LCM_RS(Bit_RESET) ;
  //
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_Initial(void)
{
  IO_LCM_POWER(_LCM_ON) ; 
  //
  LCM_DelayTimeCount = 50 ;         // Dealy 50ms
  while(LCM_DelayTimeCount > 0) ;
  //
  LCMStatus.Status = 0 ;            // Clear LCM Status Buffer
  //
  LCM_DelayTimeCount = 100 ;        // Delay 100ms
  while(LCM_DelayTimeCount > 0) ;
  //
  IO_LCM_RS(Bit_RESET) ;            // Set RS = Low level
#ifdef	_LCMDataSize4   
  LCM_OutData(0x20,1) ;             // Set 4 bit Control
#else
  LCM_OutData(0x30) ;             // Set 8 bit Control  
#endif  
  //
  LCM_DelayTimeCount = 10 ;          // Dealy 2ms
  while(LCM_DelayTimeCount > 0) ;
  LCM_Clear( ) ;  
  //
  LCMStatus.B.BF = 0 ;              // Clear LCM busy flag
#ifdef	_LCMDataSize4    
  LCM_WriteCommand(0x28,10) ;       // Set 4 bit Control
  LCM_WriteCommand(0x0c,10) ;
  LCM_WriteCommand(0x06,10) ;
  LCM_WriteCommand(0x01,10) ;
#else
  LCM_WriteCommand(0x38,10) ;       // Set 8 bit Control
  LCM_WriteCommand(0x0c,10) ;       // Display on, Cursor off , Blinking Cursor off(00001DCB)
  LCM_WriteCommand(0x06,10) ;       // Set Cursor move Increment.
  LCM_WriteCommand(0x01,10) ;       // Clear Display
#endif
  LCM_Clear( ) ;
  LCM_DelayTimeCount = 100 ;         // Dealy 100ms
  while(LCM_DelayTimeCount > 0) ;
  //
  LCMStatus.B.BF = 0 ;              // Clear LCM busy flag
  //
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_DelayTime(void)
{
  if( LCM_DelayTimeCount != 0 )
      LCM_DelayTimeCount -= 1 ;
                   
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_Clear(void)
{
  LCMStatus.B.BF = 1 ;
  LCM_WriteCommand(0x01,100) ;
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_CheckBusy( unsigned int dt )
{
  unsigned char lcd_data = 0 ;
  //
  LCM_DelayTimeCount = dt ;
  //----------------------------------------------------------------------------
  if( LCMStatus.B.BF == 1 )
      {
      while(LCM_DelayTimeCount > 0) ;      
      LCMStatus.B.BF = 0 ;
      }
  else
      {
      //------------------------------------------------------------------------
#ifdef	_LCMDataSize4  
      IO_LCM_oD4(1) ;
      IO_LCM_oD5(1) ;
      IO_LCM_oD6(1) ;
      IO_LCM_oD7(1) ;
#else
      IO_LCM_oD0(1) ;
      IO_LCM_oD1(1) ;
      IO_LCM_oD2(1) ;
      IO_LCM_oD3(1) ;
      IO_LCM_oD4(1) ;
      IO_LCM_oD5(1) ;
      IO_LCM_oD6(1) ;
      IO_LCM_oD7(1) ;	  
#endif	   
      //------------------------------------------------------------------------
      LCMStatus.B.BF = 1 ;
      while( LCMStatus.B.BF == 1 && LCM_DelayTimeCount > 0  )
          {
          IO_LCM_RS(Bit_RESET) ;
          #ifdef	_LCMDataSize4
          lcd_data = LCM_InData(1) ;
          lcd_data |= LCM_InData(0) ;
          #else
          lcd_data = LCM_InData() ;
          #endif
          LCMStatus.Status = lcd_data ;
          }
      LCMStatus.B.BF = 0 ;
      }
  //----------------------------------------------------------------------------
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_WriteCommand( unsigned char cmd ,unsigned int dt )
{
  LCM_CheckBusy(dt) ;
  IO_LCM_RS(Bit_RESET) ;
#ifdef	_LCMDataSize4	
  LCM_OutData( cmd ,1) ;
  LCM_OutData( cmd ,0) ;
#else
  LCM_OutData( cmd ) ;
#endif
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCM_WriteData( unsigned char lcd_data,unsigned int dt )
{
  LCM_CheckBusy(dt) ;
  IO_LCM_RS(Bit_SET) ;
#ifdef	_LCMDataSize4		
  LCM_OutData( lcd_data,1 ) ;
  LCM_OutData( lcd_data,0 ) ;
#else
  LCM_OutData( lcd_data ) ;
#endif  
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
#ifdef	_LCMDataSize4 
void LCM_OutData( unsigned char lcd_data , unsigned char Mode)
#else
void LCM_OutData( unsigned char lcd_data )
#endif
{
  TransByte2Bit LCMData ;
  //----------------------------------------------------------------------------
  LCMData.All = lcd_data ;
  //
  IO_LCM_RW(Bit_RESET) ;   // Low
#ifdef	_LCMDataSize4  
  if( Mode == 1 )       // Upper 4 bit
      {
      IO_LCM_oD4(LCMData.bit.B4) ;
      IO_LCM_oD5(LCMData.bit.B5) ;
      IO_LCM_oD6(LCMData.bit.B6) ;
      IO_LCM_oD7(LCMData.bit.B7) ;
      }
  else                  // Low 4 bit
      {             
      IO_LCM_oD4(LCMData.bit.B0) ;
      IO_LCM_oD5(LCMData.bit.B1) ;
      IO_LCM_oD6(LCMData.bit.B2) ;
      IO_LCM_oD7(LCMData.bit.B3) ;
      }
#else
      IO_LCM_oD0(LCMData.bit.B0) ;
      IO_LCM_oD1(LCMData.bit.B1) ;
      IO_LCM_oD2(LCMData.bit.B2) ;
      IO_LCM_oD3(LCMData.bit.B3) ;
      IO_LCM_oD4(LCMData.bit.B4) ;
      IO_LCM_oD5(LCMData.bit.B5) ;
      IO_LCM_oD6(LCMData.bit.B6) ;
      IO_LCM_oD7(LCMData.bit.B7) ;	  
#endif	  
  IO_LCM_E(Bit_SET) ;      // High
  IO_LCM_E(Bit_RESET) ;    // Low
  //----------------------------------------------------------------------------
	return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
#ifdef	_LCMDataSize4 
unsigned char LCM_InData( unsigned char Mode )
#else
unsigned char LCM_InData( void )
#endif
{
#ifdef	_LCMDataSize4
  GPIO_InitTypeDef          GPIO_InitStructure;
#endif
  TransByte2Bit LCMData ;
  LCMData.All = 0 ;
  //----------------------------------------------------------------------------
  //
#ifdef	_LCMDataSize4     
  /*
  IO_LCM_oD4(1) ;
  IO_LCM_oD5(1) ;
  IO_LCM_oD6(1) ;
  IO_LCM_oD7(1) ;
  */
  //
  IO_LCM_RW(Bit_SET) ;            // High
  IO_LCM_E(Bit_SET) ;             // High
  //
  GPIO_InitStructure.GPIO_Pin = _LCM_D4_PIN | _LCM_D5_PIN  | _LCM_D6_PIN | _LCM_D7_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
  GPIO_Init(_LCM_HDATA_GPIO_GROUP, &GPIO_InitStructure);	  
  //
  if( Mode == 1 )                 // Upper 4 bit
      {  
      LCMData.bit.B4 = IO_LCM_iD4() ;
      LCMData.bit.B5 = IO_LCM_iD5() ;
      LCMData.bit.B6 = IO_LCM_iD6() ;
      LCMData.bit.B7 = IO_LCM_iD7() ;
      }
  else
      {      
      LCMData.bit.B0 = IO_LCM_iD4() ;
      LCMData.bit.B1 = IO_LCM_iD5() ;
      LCMData.bit.B2 = IO_LCM_iD6() ;
      LCMData.bit.B3 = IO_LCM_iD7() ;
      }
  //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_Init(_LCM_HDATA_GPIO_GROUP, &GPIO_InitStructure);	  
#else
  IO_LCM_oD0(1) ;
  IO_LCM_oD1(1) ;
  IO_LCM_oD2(1) ;
  IO_LCM_oD3(1) ;
  IO_LCM_oD4(1) ;
  IO_LCM_oD5(1) ;
  IO_LCM_oD6(1) ;
  IO_LCM_oD7(1) ;
  //
  IO_LCM_RW(Bit_SET) ;            // High
  IO_LCM_E(Bit_SET) ;             // High
  
  LCMData.bit.B0 = IO_LCM_iD0() ;
  LCMData.bit.B1 = IO_LCM_iD1() ;
  LCMData.bit.B2 = IO_LCM_iD2() ;
  LCMData.bit.B3 = IO_LCM_iD3() ;
  LCMData.bit.B4 = IO_LCM_iD4() ;
  LCMData.bit.B5 = IO_LCM_iD5() ;
  LCMData.bit.B6 = IO_LCM_iD6() ;
  LCMData.bit.B7 = IO_LCM_iD7() ;	  
#endif	  
  IO_LCM_E(Bit_RESET) ;    // Low
  //----------------------------------------------------------------------------
  return LCMData.All ;
}








