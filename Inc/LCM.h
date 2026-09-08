#ifndef __LCM_H
#define __LCM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//
#define	_LCMDataSize4  

typedef union {
  struct {
    unsigned char AC:7 ;
    unsigned char BF:1 ;
  } B ;
  unsigned char Status ;
} LCMStatusFormat ;

void LCM_HW_Initial(void) ;
void LCM_Initial(void);
void LCM_DelayTime(void) ;
void LCM_WriteCommand( unsigned char cmd ,unsigned int dt );
void LCM_WriteData( unsigned char lcd_data,unsigned int dt );


#endif /* __LCM_H */

