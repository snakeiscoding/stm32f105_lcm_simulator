
#include  "stm32_reg.h"
#include  "Stm32GPIO.h"



/**
  * @brief  Sets or clears the selected data port bit.
  * @param  GPx: where x can be (A..G) to select the GPIO peripheral.
  * @param  Pinx: specifies the port bit to be written.
  *   This parameter can be one of Pinx where x can be (0..15).
  * @param  BitValue: specifies the value to be written to the selected bit.
  *   This parameter can be one of the BitAction enum values:
  *     @arg 0: to clear the port pin
  *     @arg 1: to set the port pin
  * @retval None
  */
void Stm32GPIO_WriteBit(unsigned char GPx , unsigned short Pinx , unsigned short BitValue )
{
  if( BitValue == 0 )
      {
      switch(GPx)
          {
          case  'A' :
                    HCC_GPIO_BRR(A) = Pinx ;
                    break ;
          case  'B' :
                    HCC_GPIO_BRR(B) = Pinx ;
                    break ;            
          case  'C' :
                    HCC_GPIO_BRR(C) = Pinx ;
                    break ;            
          case  'D' :
                    HCC_GPIO_BRR(D) = Pinx ;
                    break ;            
          case  'E' :
                    HCC_GPIO_BRR(E) = Pinx ;          
                    break ;
          case  'G' :
                    HCC_GPIO_BRR(G) = Pinx ;          
                    break ;                    
          }
      }
  else
      {
      switch(GPx)
          {
          case  'A' :
                    HCC_GPIO_BSRR(A) = Pinx ;
                    break ;
          case  'B' :
                    HCC_GPIO_BSRR(B) = Pinx ;
                    break ;            
          case  'C' :
                    HCC_GPIO_BSRR(C) = Pinx ;
                    break ;            
          case  'D' :
                    HCC_GPIO_BSRR(D) = Pinx ;
                    break ;            
          case  'E' :
                    HCC_GPIO_BSRR(E) = Pinx ;          
                    break ;
          case  'G' :
                    HCC_GPIO_BSRR(G) = Pinx ;          
                    break ;                    
          }        
      }
}



/**
  * @brief  Reads the specified input port pin.
  * @param  GPx: where x can be (A..G) to select the GPIO peripheral.
  * @param  Pinx:  specifies the port bit to read.
  *   This parameter can be Pinx where x can be (0..15).
  * @retval The input port pin value.
  */
unsigned char Stm32GPIO_ReadInputDataBit(unsigned char GPx , unsigned short Pinx)
{
  unsigned short ReadData ;
  switch(GPx)
      {
      case  'A' :
                ReadData = HCC_GPIO_IDR(A) & Pinx ;
                break ;
      case  'B' :
                ReadData = HCC_GPIO_IDR(B) & Pinx ;
                break ;            
      case  'C' :
                ReadData = HCC_GPIO_IDR(C) & Pinx ;
                break ;            
      case  'D' :
                ReadData = HCC_GPIO_IDR(D) & Pinx ;
                break ;            
      case  'E' :
                ReadData = HCC_GPIO_IDR(E) & Pinx ;       
                break ;
      case  'G' :
                ReadData = HCC_GPIO_IDR(G) & Pinx ;         
                break ;                    
      }
  //
  if( ReadData != 0 )
    return 1 ;
  //
  return 0 ;
}