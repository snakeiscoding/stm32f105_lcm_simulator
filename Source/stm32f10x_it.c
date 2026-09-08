/**
  ******************************************************************************
  * @file    ADC/TIMTrigger_AutoInjection/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include  "stm32f10x_it.h"
#include  "PinDefine.h"


/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_TIMTrigger_AutoInjection
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
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*
void SysTick_Handler(void)
{
}
*/

/**
  * @brief  This function handles Window Watchdog interrupt request.
  * @param  None
  * @retval None
  */
void WWDG_IRQHandler(void)
{
}

/**
  * @brief  This function handles PVD through EXTI Line detect interrupt request.
  * @param  None
  * @retval None
  */
void PVD_IRQHandler(void)
{
}

/**
  * @brief  This function handles Tamper interrupt request.
  * @param  None
  * @retval None
  */
void TAMPER_IRQHandler(void)
{
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
}

/**
  * @brief  This function handles Flash interrupt request.
  * @param  None
  * @retval None
  */
void FLASH_IRQHandler(void) 
{
}

/**
  * @brief  This function handles RCC interrupt request.
  * @param  None
  * @retval None
  */
void RCC_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 2 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line 4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 1 interrupt request.
  * @param  None
  * @retval None
  */
//void DMA1_Channel1_IRQHandler(void)
//{

//}

/**
  * @brief  This function handles DMA1 Channel 2 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel2_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 3 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel3_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 4 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel4_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 5 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 6 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel6_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA1 Channel 7 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel7_IRQHandler(void)
{
}

/**
  * @brief  This function handles ADC1 and ADC2 interrupt request.
  * @param  None
  * @retval None
  */
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL)
void ADC1_IRQHandler(void)
#else
void ADC1_2_IRQHandler(void)
#endif
{
}


/**
  * @brief  This function handles EXTI Line 9..5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Break interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_BRK_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Update interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_UP_IRQHandler(void)
{
}

/**
  * @brief  This function handles TIM1 Trigger and Commutation interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_TRG_COM_IRQHandler(void)
{
}


/**
  * @brief  This function handles TIM1 Capture Compare interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_CC_IRQHandler(void)
{

}

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
/*
void TIM2_IRQHandler(void)
{
  return ;
}
*/

/**
  * @brief  This function handles TIM3 interrupt request.
  * @param  None
  * @retval None
  */
///*
void TIM3_IRQHandler(void)
{
  //TIM_ClearFlag(TIM3,(TIM_FLAG_Update|TIM_FLAG_CC1OF));   
  //TIM_Cmd(TIM3, ENABLE); 
}
//*/

/**
  * @brief  This function handles TIM4 interrupt request.
  * @param  None
  * @retval None
  */
/*
void TIM4_IRQHandler(void)
{
}
*/          
/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_EV_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_ER_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C2 Event interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_EV_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_ER_IRQHandler(void)
{
}
          
/**
  * @brief  This function handles SPI1 interrupt request.
  * @param  None
  * @retval None
  */
void SPI1_IRQHandler(void)
{
}

/**
  * @brief  This function handles SPI2 interrupt request.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void)
{
}

/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
/*
void USART1_IRQHandler(void)
{
} 
*/

/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */
/*
void USART2_IRQHandler(void)
{
} 
*/

/**
  * @brief  This function handles USART3 interrupt request.
  * @param  None
  * @retval None
  */
/*
void USART3_IRQHandler(void)
{
} 
*/

/**
  * @brief  This function handles EXTI Line 15..10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
}            
    
/**
  * @brief  This function handles RTC alarm through EXTI line interrupt request.
  * @param  None
  * @retval None
  */
void RTCAlarm_IRQHandler(void)
{
}            


#if defined (STM32F10X_MD) 
/**
  * @brief  This function handles USB Wakeup from suspend line interrupt request.
  * @param  None
  * @retval None
  */
void USBWakeUp_IRQHandler(void)
{
}    
#else
/**
  * @brief  This function handles HDMI CEC line interrupt request.
  * @param  None
  * @retval None
  */
void CEC_IRQHandler(void)
{
}


/**
  * @brief  This function handles TIM6 interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void)
{
}

          
/**
  * @brief  This function handles TIM7 interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(void)
{
  
}
          
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
