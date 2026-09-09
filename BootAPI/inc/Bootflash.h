/**
  ******************************************************************************
  * @file    stm32f10x_flash.h
  * @author  MCD Application Team
  * @version V3.2.0
  * @date    03/01/2010
  * @brief   This file contains all the functions prototypes for the FLASH 
  *          firmware library.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOOTFLASH_H
#define __BOOTFLASH_H

#ifdef __cplusplus
 extern "C" {
#endif


   
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#ifdef STM32F10X_LD_VL
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#elif defined STM32F10X_LD
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#elif defined STM32F10X_MD_VL
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#elif defined STM32F10X_MD
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#elif defined STM32F10X_HD
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#elif defined STM32F10X_CL
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)  
#endif /* STM32F10X_LD_VL */   



/** @defgroup FLASH_Exported_Functions
  * @{
  */
void BootFLASH_Unlock(void);
FLASH_Status BootFLASH_ErasePage(uint32_t Page_Address) ;
FLASH_Status BootFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data) ;
void BootFLASH_ClearFlag(uint16_t FLASH_FLAG);
FLASH_Status BootFLASH_GetStatus(void);
FLASH_Status BootFLASH_WaitForLastOperation(uint32_t Timeout) ;
FLASH_Status BootFLASH_EarseUserProgram(uint32_t SAddr,uint32_t EAddr) ;
#ifdef __cplusplus
}
#endif

#endif /* __BOOTFLASH_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
