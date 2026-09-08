/**
  ******************************************************************************
  * @file    EXTI/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.2.0
  * @date    03/01/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "Boot_it.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup EXTI_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void BootNMI_Handler(void) 
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void BootHardFault_Handler(void)
{
  SCB->AIRCR = 0x05fa0004 ; // Software Reset   
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void BootMemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BootBusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void BootUsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void BootSVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void BootDebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void BootPendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void BootSysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles Window Watchdog interrupt request.
  * @param  None
  * @retval None
  */
void BootWWDG_IRQHandler(void)
{
}

/**
  * @brief  This function handles PVD through EXTI Line detect interrupt request.
  * @param  None
  * @retval None
  */
void BootPVD_IRQHandler(void)
{
}

/**
  * @brief  This function handles Tamper interrupt request.
  * @param  None
  * @retval None
  */
void BootTAMPER_IRQHandler(void)
{
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void BootRTC_IRQHandler(void)
{
}

/**
  * @brief  This function handles Flash interrupt request.
  * @param  None
  * @retval None
  */
void BootFLASH_IRQHandler(void) 
{
}

/**
  * @brief  This function handles RCC interrupt request.
  * @param  None
  * @retval None
  */
void BootRCC_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 0 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI0_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 1 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI1_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 2 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI2_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 3 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI3_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 4 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI4_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 1 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel1_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 2 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel2_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 3 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel3_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 4 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel4_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 5 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel5_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 6 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel6_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 7 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA1_Channel7_IRQHandler(void)
{
}

/**
  * @brief  This function handles ADC1 and ADC2 interrupt request.
  * @param  None
  * @retval None
  */
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL)
void BootADC1_IRQHandler(void)
#else
void BootADC1_2_IRQHandler(void)
#endif
{
}

/**
  * @brief  This function handles CAN1 TX Handler.
  * @param  None
  * @retval None
  */
void BootCAN1_TX_IRQHandler(void)
{
}

/**
  * @brief  This function handles CAN1 RX0 Handler.
  * @param  None
  * @retval None
  */
void BootCAN1_RX0_IRQHandler(void)
{
}

/**
  * @brief  This function handles CAN1 RX1 Handler.
  * @param  None
  * @retval None
  */
void BootCAN1_RX1_IRQHandler(void)
{
}

/**
  * @brief  This function handles CAN1 SCE Handler.
  * @param  None
  * @retval None
  */
void BootCAN1_SCE_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 9..5 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI9_5_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Break interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM1_BRK_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Update interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM1_UP_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Trigger and Commutation interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM1_TRG_COM_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Capture Compare interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM1_CC_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM2_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM3 interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM3_IRQHandler(void)
{
}
          
/**
  * @brief  This function handles TIM4 interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM4_IRQHandler(void)
{
}
          
/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval None
  */
void BootI2C1_EV_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
void BootI2C1_ER_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C2 Event interrupt request.
  * @param  None
  * @retval None
  */
void BootI2C2_EV_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
void BootI2C2_ER_IRQHandler(void)
{
}
          
/**
  * @brief  This function handles SPI1 interrupt request.
  * @param  None
  * @retval None
  */
void BootSPI1_IRQHandler(void)
{
}

/**
  * @brief  This function handles SPI2 interrupt request.
  * @param  None
  * @retval None
  */
void BootSPI2_IRQHandler(void)
{
}
          
/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void BootUSART1_IRQHandler(void)
{
} 

/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */
void BootUSART2_IRQHandler(void)
{
} 

/**
  * @brief  This function handles USART3 interrupt request.
  * @param  None
  * @retval None
  */
void BootUSART3_IRQHandler(void)
{
} 

/**
  * @brief  This function handles EXTI Line 15..10 interrupt request.
  * @param  None
  * @retval None
  */
void BootEXTI15_10_IRQHandler(void)
{
}            
    
/**
  * @brief  This function handles RTC alarm through EXTI line interrupt request.
  * @param  None
  * @retval None
  */
void BootRTCAlarm_IRQHandler(void)
{
}            

/**
  * @brief  This function handles USB OTG FS Wakeup through EXTI line interrupt request.
  * @param  None
  * @retval None
  */
void BootOTG_FS_WKUP_IRQHandler(void)
{
}  
    
/**
  * @brief  This function handles TIM5 interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM5_IRQHandler(void)
{
}          

/**
  * @brief  This function handles SPI3 interrupt request.
  * @param  None
  * @retval None
  */
void BootSPI3_IRQHandler(void)
{
}          
      
/**
  * @brief  This function handles UART4 interrupt request.
  * @param  None
  * @retval None
  */
void BootUART4_IRQHandler(void)
{
}          
         
/**
  * @brief  This function handles UART5 interrupt request.
  * @param  None
  * @retval None
  */
extern void WiFiBoot_UartTxRx_Information(void) ;
void BootUART5_IRQHandler(void)
{
  //WiFiBoot_UartTxRx_Information() ;
  return ;    
}          

/**
  * @brief  This function handles TIM6 interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM6_IRQHandler(void)
{
}

          
/**
  * @brief  This function handles TIM7 interrupt request.
  * @param  None
  * @retval None
  */
void BootTIM7_IRQHandler(void)
{
  
}
          
/**
  * @brief  This function handles DMA2 Channel 1 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA2_Channel1_IRQHandler(void)
{
}


/**
  * @brief  This function handles DMA2 Channel 2 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA2_Channel2_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA2 Channel 3 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA2_Channel3_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA2 Channel 4 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA2_Channel4_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA2 Channel 5 interrupt request.
  * @param  None
  * @retval None
  */
void BootDMA2_Channel5_IRQHandler(void)
{
}          

/**
  * @brief  This function handles Ethernet interrupt request.
  * @param  None
  * @retval None
  */
void BootETH_IRQHandler(void)
{
}

/**
  * @brief  This function handles Ethernet Wakeup through EXTI line interrupt request.
  * @param  None
  * @retval None
  */
void BootETH_WKUP_IRQHandler(void)
{
}
          
/**
  * @brief  This function handles CAN2 TX Handler.
  * @param  None
  * @retval None
  */
void BootCAN2_TX_IRQHandler(void)
{
}

/**
  * @brief  This function handles CAN2 RX0 Handler.
  * @param  None
  * @retval None
  */
void BootCAN2_RX0_IRQHandler(void)
{
}

/**
  * @brief  This function handles CAN2 RX1 Handler.
  * @param  None
  * @retval None
  */
void BootCAN2_RX1_IRQHandler(void)
{
}

/**
  * @brief  This function handles CAN2 SCE Handler.
  * @param  None
  * @retval None
  */
void BootCAN2_SCE_IRQHandler(void)
{
}

/**
  * @brief  This function handles USB OTG FS Handler.
  * @param  None
  * @retval None
  */
/*
void OTG_FS_IRQHandler(void)
{
}
*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
