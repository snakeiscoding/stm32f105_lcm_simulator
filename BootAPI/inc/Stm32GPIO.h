/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32GPIO_H
#define __STM32GPIO_H

#define PIN0                 ((unsigned short)0x0001)  /*!< Pin 0 selected */
#define PIN1                 ((unsigned short)0x0002)  /*!< Pin 1 selected */
#define PIN2                 ((unsigned short)0x0004)  /*!< Pin 2 selected */
#define PIN3                 ((unsigned short)0x0008)  /*!< Pin 3 selected */
#define PIN4                 ((unsigned short)0x0010)  /*!< Pin 4 selected */
#define PIN5                 ((unsigned short)0x0020)  /*!< Pin 5 selected */
#define PIN6                 ((unsigned short)0x0040)  /*!< Pin 6 selected */
#define PIN7                 ((unsigned short)0x0080)  /*!< Pin 7 selected */
#define PIN8                 ((unsigned short)0x0100)  /*!< Pin 8 selected */
#define PIN9                 ((unsigned short)0x0200)  /*!< Pin 9 selected */
#define PIN10                ((unsigned short)0x0400)  /*!< Pin 10 selected */
#define PIN11                ((unsigned short)0x0800)  /*!< Pin 11 selected */
#define PIN12                ((unsigned short)0x1000)  /*!< Pin 12 selected */
#define PIN13                ((unsigned short)0x2000)  /*!< Pin 13 selected */
#define PIN14                ((unsigned short)0x4000)  /*!< Pin 14 selected */
#define PIN15                ((unsigned short)0x8000)  /*!< Pin 15 selected */


void Stm32GPIO_WriteBit(unsigned char GPx , unsigned short Pinx , unsigned short BitValue ) ;
unsigned char Stm32GPIO_ReadInputDataBit(unsigned char GPx , unsigned short Pinx)  ;







#endif /* __STM32GPIO_H */