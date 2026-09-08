

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARDINFO_H
#define __BOARDINFO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
// Infomation 
//#pragma location = ".bootinfo"
#ifdef __ICCARM__
__root const unsigned char Boot_Manufacture[52] = "Johnson Health Tech. Co., Ltd." ;
__root const unsigned char Boot_McuType[20] = "STM32F105VC" ;
__root const unsigned char Boot_ModuleName[20] = "LCB Simulator_IAR9" ;
__root const unsigned char Boot_ModuleNo[20] = "Simulator-LCB201811" ;
__root const unsigned char Boot_Product[20] = "Simulator" ;
__root const unsigned char Boot_Version[20] = "V1.0.20181107" ;


// array size �����O�|������
__root const unsigned char User_Manufacture[52]  @ ".userinfo" = "Johnson Health Tech. Co., Ltd." ;
__root const unsigned char User_McuType[20]      @ ".userinfo" = "STM32F105VC" ;
__root const unsigned char User_ModuleName[20]   @ ".userinfo" = "LCB Simulator_IAR9" ;
__root const unsigned char User_ModuleNo[20]     @ ".userinfo" = "Simulator-LCB201811" ;
__root const unsigned char User_Product[20]      @ ".userinfo" = "Simulator" ;
__root const unsigned char User_Version[20]      @ ".userinfo" = "1.3.20260126" ;
#elif __GNUC__
__attribute__((used)) const unsigned char Boot_Manufacture[52] = "Johnson Health Tech. Co., Ltd." ;
__attribute__((used)) const unsigned char Boot_McuType[20] = "STM32F105VC" ;
__attribute__((used)) const unsigned char Boot_ModuleName[20] = "LCB Simulator_IAR9" ;
__attribute__((used)) const unsigned char Boot_ModuleNo[20] = "Simulator-LCB201811" ;
__attribute__((used)) const unsigned char Boot_Product[20] = "Simulator" ;
__attribute__((used)) const unsigned char Boot_Version[20] = "V1.0.20181107" ;


// array size �����O�|������
typedef struct
{
  const unsigned char Manufacture[52] ;
  const unsigned char McuType[20] ;
  const unsigned char ModuleName[20] ;
  const unsigned char ModuleNo[20] ;
  const unsigned char Product[20] ;
  const unsigned char Version[20] ;
} UserInfo_T;

__attribute__((section(".userinfo"), used)) const UserInfo_T UserInfo = {
  "Johnson Health Tech. Co., Ltd." ,
  "STM32F105VC" ,
  "LCB Simulator_IAR9" ,
  "Simulator-LCB201811" ,
  "Simulator" ,
  "1.3.20260126"
};

// __attribute__((section(".userinfo"), used)) const unsigned char User_Manufacture[52]  = "Johnson Health Tech. Co., Ltd." ;
// __attribute__((section(".userinfo"), used)) const unsigned char User_McuType[20]      = "STM32F105VC" ;
// __attribute__((section(".userinfo"), used)) const unsigned char User_ModuleName[20]   = "LCB Simulator_IAR9" ;
// __attribute__((section(".userinfo"), used)) const unsigned char User_ModuleNo[20]     = "Simulator-LCB201811" ;
// __attribute__((section(".userinfo"), used)) const unsigned char User_Product[20]      = "Simulator" ;
// __attribute__((section(".userinfo"), used)) const unsigned char User_Version[20]      = "1.3.20260126" ;
#endif



#endif /* __BOARDINFO_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
