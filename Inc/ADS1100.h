#ifndef __ADS1100_H__
#define __ADS1100_H__




typedef union {
  struct {
    unsigned char PGA:2 ;
    unsigned char DR:2 ;
    unsigned char SC:1 ;
    unsigned char Rev:2 ;
    unsigned char ST_BSY:1 ;
  } Value ;
  unsigned char All ;
} ADS1100Config ;

typedef union {
  struct {
    unsigned short Vlaue:15 ;
    unsigned short Sign:1 ;
  } Data ;
  unsigned short All ;
} ADS1100Value ;

//----------------ADS1110(AD0)-----------------//
#define ADS1110_WR_ADDRESS    0x90 // 1001 000 0 
#define ADS1110_RD_ADDRESS    0x91 // 1001 000 1 
#define ADS1110_CONFIG_REG    0x8F // 1000 1100 defaut
//
#define   _128SPS           0   // 0xF800 FFFF(-1) 0000 0001(+1) 0x07FF (12Bit)  -2048 ~ 2047
#define   _32SPS            1   // 0xE000 FFFF(-1) 0000 0001(+1) 0x1FFF (14Bit)  -8192 ~ 8191
#define   _16SPS            2   // 0xC000 FFFF(-1) 0000 0001(+1) 0x3FFF (15Bit)  -16384 ~ 16383
#define   _8SPS             3   // 0x8000 FFFF(-1) 0000 0001(+1) 0x7FFF (16Bit)  -32768 ~ 32767
//
#define   _PGA1             0   // GAIN 1
#define   _PGA2             1   // GAIN 2
#define   _PGA4             2   // GAIN 4
#define   _PGA8             3   // GAIN 8





/* -------- function prototypes -----------------------------------------------*/
void ADS1100_Initial(void) ;
void ADS1100_SystemTime(void) ;
unsigned short ADS1100_CalculatorTimeTick( unsigned short last_TimeTick );
void ADS1100_Process(void) ;
unsigned short ADS1100_GetADCValue(unsigned char Mode) ;
unsigned short ADS1100_GetCurrentValue(void) ;



#endif /* __ADS1100_H__ */


