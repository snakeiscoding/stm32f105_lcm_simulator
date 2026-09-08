//------------------------------------------------------------------------------
#include  "Stm32GPIO.h"
#include  "stm32_reg.h"
#include  "BootLCM.h"



#define   IO_BootLCM_POWER(s)         Stm32GPIO_WriteBit('A',PIN9,s)
#define   IO_BootLCM_BL(s)            Stm32GPIO_WriteBit('A',PIN8,s)
#define   IO_BootLCM_E(s)             Stm32GPIO_WriteBit('C',PIN7,s)
#define   IO_BootLCM_RW(s)            Stm32GPIO_WriteBit('C',PIN8,s) 
#define   IO_BootLCM_RS(s)            Stm32GPIO_WriteBit('C',PIN9,s) 
#define   IO_BootLCM_oD0(s)           ( s == 0 ? Stm32GPIO_WriteBit('C',PIN6,0 ) : Stm32GPIO_WriteBit('C',PIN6,1))
#define   IO_BootLCM_oD1(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN15,0) : Stm32GPIO_WriteBit('D',PIN15,1))
#define   IO_BootLCM_oD2(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN14,0) : Stm32GPIO_WriteBit('D',PIN14,1))
#define   IO_BootLCM_oD3(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN13,0) : Stm32GPIO_WriteBit('D',PIN13,1))
#define   IO_BootLCM_oD4(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN12,0) : Stm32GPIO_WriteBit('D',PIN12,1))
#define   IO_BootLCM_oD5(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN11,0) : Stm32GPIO_WriteBit('D',PIN11,1))
#define   IO_BootLCM_oD6(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN10,0) : Stm32GPIO_WriteBit('D',PIN10,1))
#define   IO_BootLCM_oD7(s)           ( s == 0 ? Stm32GPIO_WriteBit('D',PIN9,0 ) : Stm32GPIO_WriteBit('D',PIN9,1))
#define   IO_BootLCM_iD0()            Stm32GPIO_ReadInputDataBit('C',PIN6) 
#define   IO_BootLCM_iD1()            Stm32GPIO_ReadInputDataBit('D',PIN15) 
#define   IO_BootLCM_iD2()            Stm32GPIO_ReadInputDataBit('D',PIN14)
#define   IO_BootLCM_iD3()            Stm32GPIO_ReadInputDataBit('D',PIN13)
#define   IO_BootLCM_iD4()            Stm32GPIO_ReadInputDataBit('D',PIN12) 
#define   IO_BootLCM_iD5()            Stm32GPIO_ReadInputDataBit('D',PIN11) 
#define   IO_BootLCM_iD6()            Stm32GPIO_ReadInputDataBit('D',PIN10)
#define   IO_BootLCM_iD7()            Stm32GPIO_ReadInputDataBit('D',PIN9)


union {
  struct {
    unsigned char AC:7 ;
    unsigned char BF:1 ;
  } B ;
  unsigned char Status ;
} BootLCMStatus ;


void BootLCM_Clear(void) ;
void BootLCM_CheckBusy( unsigned int dt ) ;
#ifdef	_BootLCMDataSize4  
void BootLCM_OutData( unsigned char lcd_data,unsigned char Mode ) ;
unsigned char BootLCM_InData(unsigned char Mode) ;
#else
void BootLCM_OutData( unsigned char lcd_data ) ;
unsigned char BootLCM_InData( void ) ;
#endif
void BootLCM_DelayTime(void) ;

unsigned short BootLCM_DelayTimeCount ;

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_HW_Initial(void) 
{

  //
  IO_BootLCM_POWER(_OFF) ;
  IO_BootLCM_BL(_OFF) ;
  IO_BootLCM_E(0); 
  IO_BootLCM_RW(1) ;
  IO_BootLCM_RS(0) ;
  //
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_Initial(void)
{
  IO_BootLCM_POWER(_ON) ; 
  //
  BootLCM_DelayTimeCount = 50 ;         // Dealy 50ms
  while(BootLCM_DelayTimeCount > 0) {BootLCM_DelayTime();}
  //
  BootLCMStatus.Status = 0 ;            // Clear BootLCM Status Buffer
  //
  BootLCM_DelayTimeCount = 100 ;        // Delay 100ms
  while(BootLCM_DelayTimeCount > 0) {BootLCM_DelayTime();}
  //
  IO_BootLCM_RS(0) ;            // Set RS = Low level
#ifdef	_BootLCMDataSize4   
  BootLCM_OutData(0x20,1) ;             // Set 4 bit Control
#else
  BootLCM_OutData(0x30) ;             // Set 8 bit Control  
#endif  
  //
  BootLCM_DelayTimeCount = 10 ;          // Dealy 2ms
  while(BootLCM_DelayTimeCount > 0) {BootLCM_DelayTime();}
  BootLCM_Clear( ) ;  
  //
  BootLCMStatus.B.BF = 0 ;              // Clear BootLCM busy flag
#ifdef	_BootLCMDataSize4    
  BootLCM_WriteCommand(0x28,10) ;       // Set 4 bit Control
  BootLCM_WriteCommand(0x0c,10) ;
  BootLCM_WriteCommand(0x06,10) ;
  BootLCM_WriteCommand(0x01,10) ;
  /*
  BootLCM_WriteCommand(0x14,10) ;
  BootLCM_WriteCommand(0x80,10) ;
  BootLCM_WriteCommand(0x40,10) ;
  BootLCM_WriteCommand(0x06,10) ;
  */
#else
  BootLCM_WriteCommand(0x38,10) ;       // Set 8 bit Control
  BootLCM_WriteCommand(0x0c,10) ;
  BootLCM_WriteCommand(0x06,10) ;
  BootLCM_WriteCommand(0x01,10) ;  
#endif
  BootLCM_Clear( ) ;
  BootLCM_DelayTimeCount = 100 ;         // Dealy 100ms
  while(BootLCM_DelayTimeCount > 0) {BootLCM_DelayTime();}
  //
  BootLCMStatus.B.BF = 0 ;              // Clear BootLCM busy flag
  //
  IO_BootLCM_BL(_ON) ;
  //
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_DelayTime(void)
{
  unsigned short i ;
  for(i=0; i < 0x1000 ;i++) ;
  if( BootLCM_DelayTimeCount != 0 )
      BootLCM_DelayTimeCount -= 1 ;
                   
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_Clear(void)
{
  BootLCMStatus.B.BF = 1 ;
  BootLCM_WriteCommand(0x01,100) ;
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_CheckBusy( unsigned int dt )
{
  unsigned char lcd_data = 0 ;
  //
  BootLCM_DelayTimeCount = dt ;
  //----------------------------------------------------------------------------
  if( BootLCMStatus.B.BF == 1 )
      {
      while(BootLCM_DelayTimeCount > 0) {BootLCM_DelayTime();}      
      BootLCMStatus.B.BF = 0 ;
      }
  else
      {
      //------------------------------------------------------------------------
#ifdef	_BootLCMDataSize4  
      IO_BootLCM_oD4(1) ;
      IO_BootLCM_oD5(1) ;
      IO_BootLCM_oD6(1) ;
      IO_BootLCM_oD7(1) ;
#else
      IO_BootLCM_oD0(1) ;
      IO_BootLCM_oD1(1) ;
      IO_BootLCM_oD2(1) ;
      IO_BootLCM_oD3(1) ;
      IO_BootLCM_oD4(1) ;
      IO_BootLCM_oD5(1) ;
      IO_BootLCM_oD6(1) ;
      IO_BootLCM_oD7(1) ;	  
#endif	   
      //------------------------------------------------------------------------
      BootLCMStatus.B.BF = 1 ;
      while( BootLCMStatus.B.BF == 1 && BootLCM_DelayTimeCount > 0  )
          {
          //
          BootLCM_DelayTime();
          //
          IO_BootLCM_RS(0) ;
          #ifdef	_BootLCMDataSize4
          lcd_data = BootLCM_InData(1) ;
          lcd_data |= BootLCM_InData(0) ;
          #else
          lcd_data = BootLCM_InData() ;
          #endif
          BootLCMStatus.Status = lcd_data ;
          }
      BootLCMStatus.B.BF = 0 ;
      }
  //----------------------------------------------------------------------------
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_WriteCommand( unsigned char cmd ,unsigned int dt )
{
  BootLCM_CheckBusy(dt) ;
  IO_BootLCM_RS(0) ;
#ifdef	_BootLCMDataSize4	
  BootLCM_OutData( cmd ,1) ;
  BootLCM_OutData( cmd ,0) ;
#else
  BootLCM_OutData( cmd ) ;
#endif
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCM_WriteData( unsigned char lcd_data,unsigned int dt )
{
  BootLCM_CheckBusy(dt) ;
  IO_BootLCM_RS(1) ;
#ifdef	_BootLCMDataSize4		
  BootLCM_OutData( lcd_data,1 ) ;
  BootLCM_OutData( lcd_data,0 ) ;
#else
  BootLCM_OutData( lcd_data ) ;
#endif  
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
#ifdef	_BootLCMDataSize4 
void BootLCM_OutData( unsigned char lcd_data , unsigned char Mode)
#else
void BootLCM_OutData( unsigned char lcd_data )
#endif
{
  union {
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
  } LCMData ;
  
  //----------------------------------------------------------------------------
  LCMData.All = lcd_data ;
  //
  IO_BootLCM_RW(0) ;   // Low
#ifdef	_BootLCMDataSize4  
  if( Mode == 1 )       // Upper 4 bit
      {
      IO_BootLCM_oD4(LCMData.bit.B4) ;
      IO_BootLCM_oD5(LCMData.bit.B5) ;
      IO_BootLCM_oD6(LCMData.bit.B6) ;
      IO_BootLCM_oD7(LCMData.bit.B7) ;
      }
  else                  // Low 4 bit
      {             
      IO_BootLCM_oD4(LCMData.bit.B0) ;
      IO_BootLCM_oD5(LCMData.bit.B1) ;
      IO_BootLCM_oD6(LCMData.bit.B2) ;
      IO_BootLCM_oD7(LCMData.bit.B3) ;
      }
#else
      IO_BootLCM_oD0(LCMData.bit.B0) ;
      IO_BootLCM_oD1(LCMData.bit.B1) ;
      IO_BootLCM_oD2(LCMData.bit.B2) ;
      IO_BootLCM_oD3(LCMData.bit.B3) ;
      IO_BootLCM_oD4(LCMData.bit.B4) ;
      IO_BootLCM_oD5(LCMData.bit.B5) ;
      IO_BootLCM_oD6(LCMData.bit.B6) ;
      IO_BootLCM_oD7(LCMData.bit.B7) ;	  
#endif	  
  IO_BootLCM_E(1) ;      // High
  IO_BootLCM_E(0) ;    // Low
  //----------------------------------------------------------------------------
	return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
#ifdef	_BootLCMDataSize4 
unsigned char BootLCM_InData( unsigned char Mode )
#else
unsigned char BootLCM_InData( void )
#endif
{
  union {
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
  } LCMData ;  

  LCMData.All = 0 ;
  //----------------------------------------------------------------------------
  //
#ifdef	_BootLCMDataSize4     
  /*
  IO_BootLCM_oD4(1) ;
  IO_BootLCM_oD5(1) ;
  IO_BootLCM_oD6(1) ;
  IO_BootLCM_oD7(1) ;
  */
  //
  IO_BootLCM_RW(1) ;            // High
  IO_BootLCM_E(1) ;             // High
  //
  /*
  GPIO_InitStructure.GPIO_Pin = _BootLCM_D4_PIN | _BootLCM_D5_PIN  | _BootLCM_D6_PIN | _BootLCM_D7_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
  GPIO_Init(_BootLCM_HDATA_GPIO_GROUP, &GPIO_InitStructure);	
  */  
  // Change to Input pull-up
  HCC_GPIO_CRH(D) |=0x000ffff0 ; // PD9 ~ PD12
  HCC_GPIO_CRH(D) &=0xfff8888f ; // Input  
  //
  if( Mode == 1 )                 // Upper 4 bit
      {  
      LCMData.bit.B4 = IO_BootLCM_iD4() ;
      LCMData.bit.B5 = IO_BootLCM_iD5() ;
      LCMData.bit.B6 = IO_BootLCM_iD6() ;
      LCMData.bit.B7 = IO_BootLCM_iD7() ;
      }
  else
      {      
      LCMData.bit.B0 = IO_BootLCM_iD4() ;
      LCMData.bit.B1 = IO_BootLCM_iD5() ;
      LCMData.bit.B2 = IO_BootLCM_iD6() ;
      LCMData.bit.B3 = IO_BootLCM_iD7() ;
      }
  //
  /*
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_Init(_BootLCM_HDATA_GPIO_GROUP, &GPIO_InitStructure);	  
  */
  // Change to Output push-pull
  HCC_GPIO_CRH(D) |=0x000ffff0 ; // PD9 ~ PD12
  HCC_GPIO_CRH(D) &=0xfff3333f ;  
  //
#else
  IO_BootLCM_oD0(1) ;
  IO_BootLCM_oD1(1) ;
  IO_BootLCM_oD2(1) ;
  IO_BootLCM_oD3(1) ;
  IO_BootLCM_oD4(1) ;
  IO_BootLCM_oD5(1) ;
  IO_BootLCM_oD6(1) ;
  IO_BootLCM_oD7(1) ;
  //
  IO_BootLCM_RW(1) ;            // High
  IO_BootLCM_E(1) ;             // High
  
  LCMData.bit.B0 = IO_BootLCM_iD0() ;
  LCMData.bit.B1 = IO_BootLCM_iD1() ;
  LCMData.bit.B2 = IO_BootLCM_iD2() ;
  LCMData.bit.B3 = IO_BootLCM_iD3() ;
  LCMData.bit.B4 = IO_BootLCM_iD4() ;
  LCMData.bit.B5 = IO_BootLCM_iD5() ;
  LCMData.bit.B6 = IO_BootLCM_iD6() ;
  LCMData.bit.B7 = IO_BootLCM_iD7() ;	  
#endif	  
  IO_BootLCM_E(0) ;    // Low
  //----------------------------------------------------------------------------
  return LCMData.All ;
}





//
/* ********************************************************************** */
// ##### LCD MODULE CONTROL COMMAND ##### KS 0066 
// (LCDCW = 00H) RS=0,R/W=0
//   D7  D6  D5  D4  D3  D2  D1  D0
//   0   0   0   0   0   0   0   1    CLEAR DISPLAY
//   0   0   0   0   0   0   1   X    RETURN DISPLAY BEING SHIFTED OF DISPLAY
//   0   0   0   0   0   1  I/D  S    SET CURSOR MOVE DIRECTION & SPECIFES
//                                    SHIFT OF DISPLAY
//   0   0   0   0   1   D   C   B    D = 1 DISPLAY ON, 0 DIAPLAY OFF
//                                    C = 1 CURSOR ON, 0 CURSOR OFF
//                                    B = 1 CURSOR BLINK, 0 CURSOR NOT BLINK
//   0   0   0   1  S/C R/L  X   X    S/C = 1 DISPLAY SHIFT, 0 CURSOR MOVE
//                                    R/L = 1 SHIFTS RIGHT, 0 SHIFTS LEFT
//   0   0   1  DL   N   F   X   X    DL = 1 DATA 8 BIT, 0 DATA 4 BIT
//                                    N = 1 TWO LINE, 0 ONE LINE
//                                    F = 1 5x10 DOTS, 0 5x7 DOTS
//   0   1  <   CG RAM ADDRESS    >   SET CG RAM ADDRESS
//   1  <     DD RAM ADDRESS      >   SET DD RAM ADDRESS
//   BF <   READ DD/CG ADDRESS    >   BF = 0 NO BUSY, 1 BUSY
//                                    READ DD/CG RAM ADDRESS
// (01H)  RS=1,R/W=0
//  <  WRITE DATA INTO DD/CG RAM  >   WRITES DATA INTO DD/CG RAM
//
// ##### LCD DD RAM ADDRESS MAPPING #####
//         1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20
// 1-LINE 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
// 2-LINE 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53
// 3-LINE 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27
// 4-LINE 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67
//
/* ********************************************************************** */


/* ---------------------------------------------------------------------- */
#define   _LINE0ADDR_     0x80 
#define   _LINE1ADDR_     0xc0 
#define   _LINE2ADDR_     0x94 
#define   _LINE3ADDR_     0xd4 

#define   _LCD_CLR_       0x01
#define   _LCD_RET_       0x02
#define   _LCD_MODE_      0x07
#define   _LCD_ONOFF_     0x0f

#define   _LOOP_          0x5a
#define   _LOOP_RET_      0xa5

unsigned char BootLCMDisplay_xy( unsigned char x , unsigned char y ) ;



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
unsigned char BootLCMDisplay_xy( unsigned char x , unsigned char y )
{
  unsigned char lcd_xy_addr ;
  lcd_xy_addr = 0x80 + (y * 0x40) + x ;
  return lcd_xy_addr ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCMDisplay_show_one_char( unsigned char x , unsigned char y , unsigned char lcd_data) 
{
  unsigned char lcd_xy_addr ;
  lcd_xy_addr = BootLCMDisplay_xy( x , y ) ;
  BootLCM_WriteCommand( lcd_xy_addr, 100 ) ;
  BootLCM_WriteData(lcd_data,100) ;
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCMDisplay_show_byte( unsigned char x , unsigned char y , unsigned char lcd_data ) 
{
  unsigned char d_hi , d_lo ;
  d_hi = ( lcd_data / 0x10 ) + 0x30 ;
  if( d_hi > 0x39 )
      {
      d_hi = d_hi + 0x07 ;
      }

  d_lo = ( lcd_data % 0x10 ) + 0x30 ;
  if( d_lo > 0x39 )
      {
      d_lo = d_lo + 0x07 ;
      }

  BootLCMDisplay_show_one_char( x , y , d_hi ) ;
  BootLCMDisplay_show_one_char( x+1 , y , d_lo ) ;
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCMDisplay_show_word( unsigned char x , unsigned char y , unsigned int lcd_data ) 
{
  unsigned char d_hi , d_lo ;
  d_hi = lcd_data / 0x100 ;
  d_lo = lcd_data % 0x100 ;
  BootLCMDisplay_show_byte( x , y , d_hi ) ;
  BootLCMDisplay_show_byte( x+2 , y , d_lo ) ;
  return ;
}




/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCMDisplay_show_long_bcd( unsigned char x , unsigned char y , unsigned long long_data ) 
{
  unsigned long mod1,mod2 ;
  unsigned char t,t1,t2,t3,t4,t5,t6,t7,t8,t9;
  unsigned char tn[5] ;


  t    = (unsigned char)(long_data / 1000000000) ;
  mod1 = long_data % 1000000000 ;
  t1   = (unsigned char)(mod1 / 100000000) ;
  mod2 = mod1 % 100000000 ;
  t2   = (unsigned char)(mod2 / 10000000) ;
  mod1 = mod2 % 10000000 ;
  t3   = (unsigned char)(mod1 / 1000000) ;
  mod2 = mod1 % 1000000 ;
  t4   = (unsigned char)(mod2 / 100000)   ;
  mod1 = mod2 % 100000 ;
  t5 = (unsigned char)(mod1 / 10000) ;
  mod2 = mod1 % 10000;
  t6 = (unsigned char)(mod2 / 1000) ;
  mod1 = mod2 % 1000 ;
  t7 = (unsigned char)(mod1 / 100) ;
  mod2 = mod1 % 100 ;
  t8 = (unsigned char)(mod2 / 10) ;
  t9 = mod2 % 10 ;

  tn[0] = (t  * 0x10) + t1 ;
  tn[1] = (t2 * 0x10) + t3 ;
  tn[2] = (t4 * 0x10) + t5 ;
  tn[3] = (t6 * 0x10) + t7 ;
  tn[4] = (t8 * 0x10) + t9 ;

  BootLCMDisplay_show_byte( x   , y , tn[0] ) ;
  BootLCMDisplay_show_byte( x+2 , y , tn[1] ) ;
  BootLCMDisplay_show_byte( x+4 , y , tn[2] ) ;
  BootLCMDisplay_show_byte( x+6 , y , tn[3] ) ;
  BootLCMDisplay_show_byte( x+8 , y , tn[4] ) ;
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
unsigned char BootLCMDisplay_show_word_bcd(unsigned char x ,unsigned char y ,unsigned int hex_data , unsigned char Num ) 
{
  unsigned short mod0 , mod1 , mod2 ;
  unsigned char t0,t1,t2,t3,t4 ;
  unsigned char Pos_X ;
  unsigned char Offset_x ;
  unsigned char i ;
  
  Pos_X = x ;
  Offset_x =  0 ;
  
  t0 = (unsigned char)(hex_data / 10000 ) ;
  mod0 = hex_data % 10000  ;
  t1 = (unsigned char)(mod0 / 1000 ) ;
  mod1 = hex_data % 1000 ;
  t2 = (unsigned char)( mod1 / 100 ) ;
  mod2 = mod1 % 100  ;
  t3 = (unsigned char)( mod2 / 10 ) ;
  t4 = (unsigned char)( mod2 % 10 ) ;

  switch( Num )
      {
      case  12://xxx.xx
#ifndef  _AutoFillSpace                     //012345
              BootLCMDisplay_show_string(Pos_X,y,"      ") ;
#endif        
              if( t0 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  }
              else if( t1 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  }
              else if( t2 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;             
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
              /*
              if( t3 != 0 || t4 != 0)
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
               
              else
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              */
#ifdef  _AutoFillSpace              
              // Fill Space
              if( t1 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              
              if( t2 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;  
                  Offset_x++ ;
                  }
              Pos_X -= Offset_x ;
              //
#endif              
              break ;        
      case  11: //xx.x
#ifndef  _AutoFillSpace                     //0123
              BootLCMDisplay_show_string(Pos_X,y,"    ") ;
#endif         
              if( t2 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t3 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , '.' ) ; 
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;            
              break ;        
      case  10://xx.xx
#ifndef  _AutoFillSpace                     //01234
              BootLCMDisplay_show_string(Pos_X,y,"     ") ;
#endif        
              if( t1 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  }
              else if( t2 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              
              BootLCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;               
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              if( t3 != 0 || t4 != 0)
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace               
              else
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              // Fill Space
              if( t1 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              
              if( t2 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;  
                  Offset_x++ ;
                  }
              Pos_X -= Offset_x ;
              //
#endif              
              break ;
      case  9 ://x.xxxx
#ifndef  _AutoFillSpace                     //012345
              BootLCMDisplay_show_string(Pos_X,y,"      ") ;
#endif        
              if( t0 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
              if( t2 != 0 )    
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  if( t3 != 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      if( t4 != 0 )
                          BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace                       
                      else
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
#endif                      
                      }
                  else if( t4 != 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
                      }
#ifdef  _AutoFillSpace                   
                  else
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      }
#endif                  
                  }
              else if( t3 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  if( t4 != 0 )
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace                   
                  else
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ') ;
#endif                  
                  }
              else if( t4 != 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
                      }
#ifdef  _AutoFillSpace               
              else
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x += 3 ;
                  }
              // Fill Space
              if( t0 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              Pos_X -= Offset_x ;
              //
#endif              
              break ;
      case  8 ://xx.xxx
#ifndef  _AutoFillSpace                     //012345
              BootLCMDisplay_show_string(Pos_X,y,"      ") ;
#endif          
              if( t0 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  }
              else if( t1 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
              
              BootLCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;               
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              if( t3 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  if( t4 != 0 )
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace                  
                  else
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
#endif                  
                  }
              else if( t4 != 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
                      }
#ifdef  _AutoFillSpace               
              else
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  }
              // Fill Space
              if( t0 == 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
              
              if( t1 == 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
              //
#endif              
              break ;
      case  7 ://xxxx
#ifndef  _AutoFillSpace                     //0123
              BootLCMDisplay_show_string(Pos_X,y,"    ") ;
#endif          
              if( t1 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t2 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t3 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;  
#ifdef  _AutoFillSpace    
              // Fill Space
              if( t1 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  if( t2 == 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }
              else
                  {
                  if( t2 == 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  else
                      {
                      if( t3 == 0 )
                          {
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }              
              Pos_X -= Offset_x ;
#endif        
              break ;
      case  6 : //xxx.x
#ifndef  _AutoFillSpace                     //01234
              BootLCMDisplay_show_string(Pos_X,y,"     ") ;
#endif         
              if( t1 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t2 != 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t3 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              BootLCMDisplay_show_one_char( Pos_X++ , y , '.' ) ; 
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;   
#ifdef  _AutoFillSpace    
              // Fill Space
              if( t1 == 0 )
                  {
                  BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  if( t2 == 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }
              else
                  {
                  if( t2 == 0 )
                      {
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  else
                      {
                      if( t3 == 0 )
                          {
                          BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }              
              Pos_X -= Offset_x ;
#endif                
              break ;
      case  5 :
              if( t0 != 0 )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }
      case  4 :
              if( t1 != 0 || (Num != 4 && (Offset_x == 0 || t0 != 0)) )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }
      case  3 :
              if( t2 != 0 || (Num != 3 && (Offset_x == 0 || t1 != 0)) || ( Num == 5 && t0 != 0 )  )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }              
      case  2 :
              if( t3 != 0 || (Num != 2 && (Offset_x == 0 || t2 != 0)) || ( Num >= 4 && t1 != 0 ) || ( Num == 5 && t0 != 0 ) )
                  BootLCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }         
      case  1 :
      default :
              BootLCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
              if(Offset_x != 0)
                  {
                  for( i = 0 ; i < Offset_x; i++ )
                      BootLCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Pos_X -= Offset_x ;
                  }
              break ;
      }
  
  return Pos_X ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void BootLCMDisplay_show_string( unsigned char x , unsigned char y , unsigned char *str_ptr )
{
  unsigned char s_data = 0 ;
  unsigned char d_cnt = 0 ;
  unsigned char loop_ctrl = _LOOP_ ;
  unsigned char pos_x,pos_y ;

  pos_x = x ;
  pos_y = y ;

  do
      {
      s_data = *(str_ptr+d_cnt) ;
      d_cnt += 1 ;
      if( s_data == '\0')
          loop_ctrl = _LOOP_RET_ ;
      else
          {
          BootLCMDisplay_show_one_char( pos_x , pos_y , s_data ) ;
          pos_x += 1 ;
          if( pos_x >= 16 )
              {
              pos_y += 1 ;
              pos_x =0 ;
              if( pos_y > 1 )
                  pos_y = 0 ;
              }
          }      
      } while( loop_ctrl == _LOOP_ ) ;

  return ;
}




/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
unsigned char BootLCMDisplay_show_scrollingstring( unsigned char X, unsigned char Y , unsigned char BarSize, unsigned char *ptr, unsigned char chCount)
{
  // Parameter                            
  unsigned char TempBufer[21] ;
  unsigned char i ,j ;
  unsigned char d_cnt ;

  // Check string length
  // Srting < Screen Bar Size then direct to show string
  for( i = 0 ; i <= BarSize ; i++)
      {
      if(*(ptr+i) == '\0' )
          {
          BootLCMDisplay_show_string(X,Y,ptr) ;
          return 0xFF ;
          }
      }
  //----------------------------------------------------------------------------   
  // Clear Screen Bar Buffer
  for( i = 0 ; i < 20; i++ )
      {
      TempBufer[i] = ' ' ;
      if( i >= BarSize )
          TempBufer[i] = '\0' ;
      }
  TempBufer[i] = '\0' ;
  //----------------------------------------------------------------------------
  // Fill String to Display Buffer
  if( chCount <= BarSize )
      j = BarSize-chCount ;
  else
      j = 0 ;
  // 
  if( chCount <= BarSize )
      d_cnt = 0 ;
  else
      {
      d_cnt = chCount-BarSize ;
      // Checek End of String and chear screen bar
      if(*(ptr+d_cnt) == '\0' )
          {
          d_cnt = 0 ;
          BootLCMDisplay_show_string(X,Y,&TempBufer[0]) ;
          return 0 ;
          }
      }
  //
  for( i = j ; i < BarSize ; i++ )
      {
      TempBufer[i]=  *(ptr+d_cnt) ;
      if( TempBufer[i] == '\0' )
          TempBufer[i] = ' ' ;
      else  
          d_cnt += 1 ;      
      }
  TempBufer[i] = '\0';
  //
  chCount += 1 ;
  //
  BootLCMDisplay_show_string(X,Y,&TempBufer[0]) ;
  return chCount ;
}






