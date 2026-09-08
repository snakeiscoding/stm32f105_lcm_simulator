//
// 
#include  "PinDefine.h"
#include  "LCM.h"
#include  "LCMDisplay.h"

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
#define   _LCD_CLR_       0x01
#define   _LCD_RET_       0x02
#define   _LCD_MODE_      0x07
#define   _LCD_ONOFF_     0x0f

#define   _LCD_CGRAM_     0x40      //Set CGRAM Address Command
#define   _LCD_DDRAM_     0x80      //Set DDRAM Address Command
#define   _LOOP_          0x5a
#define   _LOOP_RET_      0xa5

unsigned char LCMDisplay_xy( unsigned char x , unsigned char y ) ;
const unsigned char CFont[5][8] = {
  { 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10 },  //0
  { 0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18 },  //1
  { 0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C },  //2
  { 0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E },  //3
  { 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F }   //4
  
} ;

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
unsigned char LCMDisplay_xy( unsigned char x , unsigned char y )
{
  unsigned char lcd_xy_addr ;
  lcd_xy_addr = _LCD_DDRAM_ + (y * 0x40) + x ;
  return lcd_xy_addr ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_show_one_char( unsigned char x , unsigned char y , unsigned char lcd_data) 
{
  unsigned char lcd_xy_addr ;
  lcd_xy_addr = LCMDisplay_xy( x , y ) ;
  LCM_WriteCommand( lcd_xy_addr, 100 ) ;
  LCM_WriteData(lcd_data,100) ;
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_show_byte( unsigned char x , unsigned char y , unsigned char lcd_data ) 
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

  LCMDisplay_show_one_char( x , y , d_hi ) ;
  LCMDisplay_show_one_char( x+1 , y , d_lo ) ;
  return ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_show_word( unsigned char x , unsigned char y , unsigned int lcd_data ) 
{
  unsigned char d_hi , d_lo ;
  d_hi = lcd_data / 0x100 ;
  d_lo = lcd_data % 0x100 ;
  LCMDisplay_show_byte( x , y , d_hi ) ;
  LCMDisplay_show_byte( x+2 , y , d_lo ) ;
  return ;
}




/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_show_long_bcd( unsigned char x , unsigned char y , unsigned long long_data ) 
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

  LCMDisplay_show_byte( x   , y , tn[0] ) ;
  LCMDisplay_show_byte( x+2 , y , tn[1] ) ;
  LCMDisplay_show_byte( x+4 , y , tn[2] ) ;
  LCMDisplay_show_byte( x+6 , y , tn[3] ) ;
  LCMDisplay_show_byte( x+8 , y , tn[4] ) ;
  return ;
}



/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
unsigned char LCMDisplay_show_word_bcd(unsigned char x ,unsigned char y ,unsigned int hex_data , unsigned char Num ) 
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
              LCMDisplay_show_string(Pos_X,y,"      ") ;
#endif        
              if( t0 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  }
              else if( t1 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  }
              else if( t2 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;             
              LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
              /*
              if( t3 != 0 || t4 != 0)
                  LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
               
              else
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              */
#ifdef  _AutoFillSpace              
              // Fill Space
              if( t1 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              
              if( t2 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;  
                  Offset_x++ ;
                  }
              Pos_X -= Offset_x ;
              //
#endif              
              break ;        
      case  11: //xx.x
#ifndef  _AutoFillSpace                     //0123
              LCMDisplay_show_string(Pos_X,y,"    ") ;
#endif         
              if( t2 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t3 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , '.' ) ; 
              LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;            
              break ;        
      case  10://xx.xx
#ifndef  _AutoFillSpace                     //01234
              LCMDisplay_show_string(Pos_X,y,"     ") ;
#endif        
              if( t1 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  }
              else if( t2 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              
              LCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;               
              LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              if( t3 != 0 || t4 != 0)
                  LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace               
              else
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              // Fill Space
              if( t1 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              
              if( t2 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;  
                  Offset_x++ ;
                  }
              Pos_X -= Offset_x ;
              //
#endif              
              break ;
      case  9 ://x.xxxx
#ifndef  _AutoFillSpace                     //012345
              LCMDisplay_show_string(Pos_X,y,"      ") ;
#endif        
              if( t0 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
              if( t2 != 0 )    
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  if( t3 != 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      if( t4 != 0 )
                          LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace                       
                      else
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
#endif                      
                      }
                  else if( t4 != 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
                      }
#ifdef  _AutoFillSpace                   
                  else
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      }
#endif                  
                  }
              else if( t3 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  if( t4 != 0 )
                      LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace                   
                  else
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ') ;
#endif                  
                  }
              else if( t4 != 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                      LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
                      }
#ifdef  _AutoFillSpace               
              else
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x += 3 ;
                  }
              // Fill Space
              if( t0 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  }
              Pos_X -= Offset_x ;
              //
#endif              
              break ;
      case  8 ://xx.xxx
#ifndef  _AutoFillSpace                     //012345
              LCMDisplay_show_string(Pos_X,y,"      ") ;
#endif          
              if( t0 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  }
              else if( t1 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
              
              LCMDisplay_show_one_char( Pos_X++ , y , '.' ) ;               
              LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              if( t3 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  if( t4 != 0 )
                      LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
#ifdef  _AutoFillSpace                  
                  else
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
#endif                  
                  }
              else if( t4 != 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                      LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
                      }
#ifdef  _AutoFillSpace               
              else
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  }
              // Fill Space
              if( t0 == 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
              
              if( t1 == 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
              //
#endif              
              break ;
      case  7 ://xxxx
#ifndef  _AutoFillSpace                     //0123
              LCMDisplay_show_string(Pos_X,y,"    ") ;
#endif          
              if( t1 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t2 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t3 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;  
#ifdef  _AutoFillSpace    
              // Fill Space
              if( t1 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  if( t2 == 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }
              else
                  {
                  if( t2 == 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  else
                      {
                      if( t3 == 0 )
                          {
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }              
              Pos_X -= Offset_x ;
#endif        
              break ;
      case  6 : //xxx.x
#ifndef  _AutoFillSpace                     //01234
              LCMDisplay_show_string(Pos_X,y,"     ") ;
#endif         
              if( t1 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t2 != 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
                  }
              else if( t3 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              LCMDisplay_show_one_char( Pos_X++ , y , '.' ) ; 
              LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;   
#ifdef  _AutoFillSpace    
              // Fill Space
              if( t1 == 0 )
                  {
                  LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Offset_x++ ;
                  if( t2 == 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }
              else
                  {
                  if( t2 == 0 )
                      {
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                      Offset_x++ ;
                      if( t3 == 0 )
                          {
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  else
                      {
                      if( t3 == 0 )
                          {
                          LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                          Offset_x++ ;
                          }
                      }
                  }              
              Pos_X -= Offset_x ;
#endif                
              break ;
      case  5 :
              if( t0 != 0 )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t0+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }
      case  4 :
              if( t1 != 0 || (Num != 4 && (Offset_x == 0 || t0 != 0)) )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t1+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }
      case  3 ://xxx
              if( t2 != 0 || (Num != 3 && (Offset_x == 0 || t1 != 0)) || ( Num == 5 && t0 != 0 )  )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t2+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }  
      case  2 :
              if( t3 != 0 || (Num != 2 && (Offset_x == 0 || t2 != 0)) || ( Num >= 4 && t1 != 0 ) || ( Num == 5 && t0 != 0 ) )
                  LCMDisplay_show_one_char( Pos_X++ , y , (t3+'0') ) ;
              else
                  {
                  Offset_x++ ;
                  }         
      case  1 :
      default :
              LCMDisplay_show_one_char( Pos_X++ , y , (t4+'0') ) ;
              if(Offset_x != 0)
                  {
                  for( i = 0 ; i < Offset_x; i++ )
                      LCMDisplay_show_one_char( Pos_X++ , y , ' ' ) ;
                  Pos_X -= Offset_x ;
                  }
              break ;
      }
  
  return Pos_X ;
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_show_string( unsigned char x , unsigned char y , unsigned char *str_ptr )
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
          LCMDisplay_show_one_char( pos_x , pos_y , s_data ) ;
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
unsigned char LCMDisplay_show_scrollingstring( unsigned char X, unsigned char Y , unsigned char BarSize, unsigned char *ptr, unsigned char chCount)
{
  // Parameter                            
  unsigned char TempBufer[41] ;
  unsigned char i ,j ;
  unsigned char d_cnt ;

  // Check string length
  // Srting < Screen Bar Size then direct to show string
  for( i = 0 ; i <= BarSize ; i++)
      {
      if(*(ptr+i) == '\0' )
          {
          LCMDisplay_show_string(X,Y,ptr) ;
          return 0xFF ;
          }
      }
  //----------------------------------------------------------------------------   
  // Clear Screen Bar Buffer
  for( i = 0 ; i < 40; i++ )
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
          LCMDisplay_show_string(X,Y,&TempBufer[0]) ;
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
  LCMDisplay_show_string(X,Y,&TempBufer[0]) ;
  return chCount ;
}


//Add 20210420
/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_CreatCharacter(char Number,const unsigned char *CharPtr)
{
  unsigned char lcd_xy_addr ;
  if( Number < 8 ) // 0~7
      {
      lcd_xy_addr = _LCD_CGRAM_ + (Number * 8) ;
      LCM_WriteCommand( lcd_xy_addr, 100 ) ;
      LCM_WriteData(*CharPtr,100) ;
      LCM_WriteData(*(CharPtr+1),100) ;
      LCM_WriteData(*(CharPtr+2),100) ;
      LCM_WriteData(*(CharPtr+3),100) ;
      LCM_WriteData(*(CharPtr+4),100) ;
      LCM_WriteData(*(CharPtr+5),100) ;
      LCM_WriteData(*(CharPtr+6),100) ;
      LCM_WriteData(*(CharPtr+7),100) ;
      }
  return ;
}

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */
void LCMDisplay_InitialCreatCharacter(void)
{
  LCMDisplay_CreatCharacter(0,&CFont[0][0] ) ; 
  LCMDisplay_CreatCharacter(1,&CFont[1][0] ) ;
  LCMDisplay_CreatCharacter(2,&CFont[2][0] ) ;
  LCMDisplay_CreatCharacter(3,&CFont[3][0] ) ;
  LCMDisplay_CreatCharacter(4,&CFont[4][0] ) ;
}