#ifndef __ENCODER_H
#define __ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define   _NORMAL       0
#define   _RING         1




void Encoder_Initial(void) ;
void Encoder_IRQFunction(void) ;
void Encoder_GetValue( unsigned short Min, unsigned short Max , unsigned short *PtrOfData , unsigned char Mode, unsigned char Scale) ;

#endif /* __ENCODER_H */

