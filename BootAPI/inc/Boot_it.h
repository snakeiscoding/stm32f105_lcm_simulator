

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOOT_IT_H
#define __BOOT_IT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void BootNMI_Handler(void);
void BootHardFault_Handler(void);
void BootMemManage_Handler(void);
void BootBusFault_Handler(void);
void BootUsageFault_Handler(void);
void BootSVC_Handler(void);
void BootDebugMon_Handler(void);
void BootPendSV_Handler(void);
void BootSysTick_Handler(void);

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
void BootWWDG_IRQHandler(void);
void BootPVD_IRQHandler(void);
void BootTAMPER_IRQHandler(void);
void BootRTC_IRQHandler(void);
void BootFLASH_IRQHandler(void);
void BootRCC_IRQHandler(void);
void BootEXTI0_IRQHandler(void);
void BootEXTI1_IRQHandler(void);
void BootEXTI2_IRQHandler(void);
void BootEXTI3_IRQHandler(void);
void BootEXTI4_IRQHandler(void);
void BootDMA1_Channel1_IRQHandler(void);
void BootDMA1_Channel2_IRQHandler(void);
void BootDMA1_Channel3_IRQHandler(void);
void BootDMA1_Channel4_IRQHandler(void);
void BootDMA1_Channel5_IRQHandler(void);
void BootDMA1_Channel6_IRQHandler(void);
void BootDMA1_Channel7_IRQHandler(void);
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL)
void BootADC1_IRQHandler(void);
#else
void BootADC1_2_IRQHandler(void);
#endif
void BootCAN1_TX_IRQHandler(void);
void BootCAN1_RX0_IRQHandler(void);
void BootCAN1_RX1_IRQHandler(void);
void BootCAN1_SCE_IRQHandler(void);
void BootEXTI9_5_IRQHandler(void);
void BootTIM1_BRK_IRQHandler(void);
void BootTIM1_UP_IRQHandler(void);
void BootTIM1_TRG_COM_IRQHandler(void);
void BootTIM1_CC_IRQHandler(void);
void BootTIM2_IRQHandler(void);
void BootTIM3_IRQHandler(void);
void BootTIM4_IRQHandler(void);
void BootI2C1_EV_IRQHandler(void);
void BootI2C1_ER_IRQHandler(void);
void BootI2C2_EV_IRQHandler(void);
void BootI2C2_ER_IRQHandler(void);
void BootSPI1_IRQHandler(void);
void BootSPI2_IRQHandler(void);
void BootUSART1_IRQHandler(void);
void BootUSART2_IRQHandler(void);
void BootUSART3_IRQHandler(void);
void BootEXTI15_10_IRQHandler(void);
void BootRTCAlarm_IRQHandler(void);
void BootOTG_FS_WKUP_IRQHandler(void);
void BootTIM5_IRQHandler(void);
void BootSPI3_IRQHandler(void);
void BootUART4_IRQHandler(void);
void BootUART5_IRQHandler(void);
void BootTIM6_IRQHandler(void);
void BootTIM7_IRQHandler(void);
void BootDMA2_Channel1_IRQHandler(void);
void BootDMA2_Channel2_IRQHandler(void);
void BootDMA2_Channel3_IRQHandler(void);
void BootDMA2_Channel4_IRQHandler(void);
void BootDMA2_Channel5_IRQHandler(void);
void BootETH_IRQHandler(void);
void BootETH_WKUP_IRQHandler(void);
void BootCAN2_TX_IRQHandler(void);
void BootCAN2_RX0_IRQHandler(void);
void BootCAN2_RX1_IRQHandler(void);
void BootCAN2_SCE_IRQHandler(void);
void OTG_FS_IRQHandler(void);


#endif /* __BOOT_IT_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
