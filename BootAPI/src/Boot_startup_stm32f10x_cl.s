;******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
;* File Name          : startup_stm32f10x_cl.s
;* Author             : MCD Application Team
;* Version            : V3.5.0
;* Date               : 11-March-2011
;* Description        : STM32F10x Connectivity line devices vector table for 
;*                      EWARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Configure the clock system
;*                      - Set the initial PC == __iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR 
;*                        address.
;*                      After Reset the Cortex-M3 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;********************************************************************************
;* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
;* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
;* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
;* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
;* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
;* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************
;
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit        
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     BootReset_Handler             ; Reset Handler
        DCD     BootNMI_Handler               ; NMI Handler
        DCD     BootHardFault_Handler         ; Hard Fault Handler
        DCD     BootMemManage_Handler         ; MPU Fault Handler
        DCD     BootBusFault_Handler          ; Bus Fault Handler
        DCD     BootUsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     BootSVC_Handler               ; SVCall Handler
        DCD     BootDebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     BootPendSV_Handler            ; PendSV Handler
        DCD     BootSysTick_Handler           ; SysTick Handler

         ; External Interrupts
        DCD     BootWWDG_IRQHandler            ; Window Watchdog
        DCD     BootPVD_IRQHandler             ; PVD through EXTI Line detect
        DCD     BootTAMPER_IRQHandler          ; Tamper
        DCD     BootRTC_IRQHandler             ; RTC
        DCD     BootFLASH_IRQHandler           ; Flash
        DCD     BootRCC_IRQHandler             ; RCC
        DCD     BootEXTI0_IRQHandler           ; EXTI Line 0
        DCD     BootEXTI1_IRQHandler           ; EXTI Line 1
        DCD     BootEXTI2_IRQHandler           ; EXTI Line 2
        DCD     BootEXTI3_IRQHandler           ; EXTI Line 3
        DCD     BootEXTI4_IRQHandler           ; EXTI Line 4
        DCD     BootDMA1_Channel1_IRQHandler   ; DMA1 Channel 1
        DCD     BootDMA1_Channel2_IRQHandler   ; DMA1 Channel 2
        DCD     BootDMA1_Channel3_IRQHandler   ; DMA1 Channel 3
        DCD     BootDMA1_Channel4_IRQHandler   ; DMA1 Channel 4
        DCD     BootDMA1_Channel5_IRQHandler   ; DMA1 Channel 5
        DCD     BootDMA1_Channel6_IRQHandler   ; DMA1 Channel 6
        DCD     BootDMA1_Channel7_IRQHandler   ; DMA1 Channel 7
        DCD     BootADC1_2_IRQHandler          ; ADC1 and ADC2
        DCD     BootCAN1_TX_IRQHandler         ; CAN1 TX
        DCD     BootCAN1_RX0_IRQHandler        ; CAN1 RX0
        DCD     BootCAN1_RX1_IRQHandler        ; CAN1 RX1
        DCD     BootCAN1_SCE_IRQHandler        ; CAN1 SCE
        DCD     BootEXTI9_5_IRQHandler         ; EXTI Line 9..5
        DCD     BootTIM1_BRK_IRQHandler        ; TIM1 Break
        DCD     BootTIM1_UP_IRQHandler         ; TIM1 Update
        DCD     BootTIM1_TRG_COM_IRQHandler    ; TIM1 Trigger and Commutation
        DCD     BootTIM1_CC_IRQHandler         ; TIM1 Capture Compare
        DCD     BootTIM2_IRQHandler            ; TIM2
        DCD     BootTIM3_IRQHandler            ; TIM3
        DCD     BootTIM4_IRQHandler            ; TIM4
        DCD     BootI2C1_EV_IRQHandler         ; I2C1 Event
        DCD     BootI2C1_ER_IRQHandler         ; I2C1 Error
        DCD     BootI2C2_EV_IRQHandler         ; I2C2 Event
        DCD     BootI2C2_ER_IRQHandler         ; I2C1 Error
        DCD     BootSPI1_IRQHandler            ; SPI1
        DCD     BootSPI2_IRQHandler            ; SPI2
        DCD     BootUSART1_IRQHandler          ; USART1
        DCD     BootUSART2_IRQHandler          ; USART2
        DCD     BootUSART3_IRQHandler          ; USART3
        DCD     BootEXTI15_10_IRQHandler       ; EXTI Line 15..10
        DCD     BootRTCAlarm_IRQHandler        ; RTC alarm through EXTI line
        DCD     BootOTG_FS_WKUP_IRQHandler     ; USB OTG FS Wakeup through EXTI line
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     BootTIM5_IRQHandler            ; TIM5
        DCD     BootSPI3_IRQHandler            ; SPI3
        DCD     BootUART4_IRQHandler           ; UART4
        DCD     BootUART5_IRQHandler           ; UART5
        DCD     BootTIM6_IRQHandler            ; TIM6
        DCD     BootTIM7_IRQHandler            ; TIM7
        DCD     BootDMA2_Channel1_IRQHandler   ; DMA2 Channel1
        DCD     BootDMA2_Channel2_IRQHandler   ; DMA2 Channel2
        DCD     BootDMA2_Channel3_IRQHandler   ; DMA2 Channel3
        DCD     BootDMA2_Channel4_IRQHandler   ; DMA2 Channel4
        DCD     BootDMA2_Channel5_IRQHandler   ; DMA2 Channel5
        DCD     BootETH_IRQHandler             ; Ethernet
        DCD     BootETH_WKUP_IRQHandler        ; Ethernet Wakeup through EXTI line
        DCD     BootCAN2_TX_IRQHandler         ; CAN2 TX
        DCD     BootCAN2_RX0_IRQHandler        ; CAN2 RX0
        DCD     BootCAN2_RX1_IRQHandler        ; CAN2 RX1
        DCD     BootCAN2_SCE_IRQHandler        ; CAN2 SCE
        DCD     OTG_FS_IRQHandler          ; USB OTG FS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK BootReset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
BootReset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        
        PUBWEAK BootNMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootNMI_Handler
        B BootNMI_Handler

        PUBWEAK BootHardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootHardFault_Handler
        B BootHardFault_Handler

        PUBWEAK BootMemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootMemManage_Handler
        B BootMemManage_Handler

        PUBWEAK BootBusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootBusFault_Handler
        B BootBusFault_Handler

        PUBWEAK BootUsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootUsageFault_Handler
        B BootUsageFault_Handler

        PUBWEAK BootSVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootSVC_Handler
        B BootSVC_Handler

        PUBWEAK BootDebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDebugMon_Handler
        B BootDebugMon_Handler

        PUBWEAK BootPendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootPendSV_Handler
        B BootPendSV_Handler

        PUBWEAK BootSysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootSysTick_Handler
        B BootSysTick_Handler

        PUBWEAK BootWWDG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootWWDG_IRQHandler
        B BootWWDG_IRQHandler

        PUBWEAK BootPVD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootPVD_IRQHandler
        B BootPVD_IRQHandler

        PUBWEAK BootTAMPER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTAMPER_IRQHandler
        B BootTAMPER_IRQHandler

        PUBWEAK BootRTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootRTC_IRQHandler
        B BootRTC_IRQHandler

        PUBWEAK BootFLASH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootFLASH_IRQHandler
        B BootFLASH_IRQHandler

        PUBWEAK BootRCC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootRCC_IRQHandler
        B BootRCC_IRQHandler

        PUBWEAK BootEXTI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI0_IRQHandler
        B BootEXTI0_IRQHandler

        PUBWEAK BootEXTI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI1_IRQHandler
        B BootEXTI1_IRQHandler

        PUBWEAK BootEXTI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI2_IRQHandler
        B BootEXTI2_IRQHandler

        PUBWEAK BootEXTI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI3_IRQHandler
        B BootEXTI3_IRQHandler


        PUBWEAK BootEXTI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI4_IRQHandler
        B BootEXTI4_IRQHandler

        PUBWEAK BootDMA1_Channel1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel1_IRQHandler
        B BootDMA1_Channel1_IRQHandler

        PUBWEAK BootDMA1_Channel2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel2_IRQHandler
        B BootDMA1_Channel2_IRQHandler

        PUBWEAK BootDMA1_Channel3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel3_IRQHandler
        B BootDMA1_Channel3_IRQHandler

        PUBWEAK BootDMA1_Channel4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel4_IRQHandler
        B BootDMA1_Channel4_IRQHandler

        PUBWEAK BootDMA1_Channel5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel5_IRQHandler
        B BootDMA1_Channel5_IRQHandler

        PUBWEAK BootDMA1_Channel6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel6_IRQHandler
        B BootDMA1_Channel6_IRQHandler

        PUBWEAK BootDMA1_Channel7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA1_Channel7_IRQHandler
        B BootDMA1_Channel7_IRQHandler

        PUBWEAK BootADC1_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootADC1_2_IRQHandler
        B BootADC1_2_IRQHandler

        PUBWEAK BootCAN1_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN1_TX_IRQHandler
        B BootCAN1_TX_IRQHandler

        PUBWEAK BootCAN1_RX0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN1_RX0_IRQHandler
        B BootCAN1_RX0_IRQHandler

        PUBWEAK BootCAN1_RX1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN1_RX1_IRQHandler
        B BootCAN1_RX1_IRQHandler

        PUBWEAK BootCAN1_SCE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN1_SCE_IRQHandler
        B BootCAN1_SCE_IRQHandler

        PUBWEAK BootEXTI9_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI9_5_IRQHandler
        B BootEXTI9_5_IRQHandler

        PUBWEAK BootTIM1_BRK_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM1_BRK_IRQHandler
        B BootTIM1_BRK_IRQHandler

        PUBWEAK BootTIM1_UP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM1_UP_IRQHandler
        B BootTIM1_UP_IRQHandler

        PUBWEAK BootTIM1_TRG_COM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM1_TRG_COM_IRQHandler
        B BootTIM1_TRG_COM_IRQHandler

        PUBWEAK BootTIM1_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM1_CC_IRQHandler
        B BootTIM1_CC_IRQHandler

        PUBWEAK BootTIM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM2_IRQHandler
        B BootTIM2_IRQHandler

        PUBWEAK BootTIM3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM3_IRQHandler
        B BootTIM3_IRQHandler

        PUBWEAK BootTIM4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM4_IRQHandler
        B BootTIM4_IRQHandler

        PUBWEAK BootI2C1_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootI2C1_EV_IRQHandler
        B BootI2C1_EV_IRQHandler

        PUBWEAK BootI2C1_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootI2C1_ER_IRQHandler
        B BootI2C1_ER_IRQHandler

        PUBWEAK BootI2C2_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootI2C2_EV_IRQHandler
        B BootI2C2_EV_IRQHandler

        PUBWEAK BootI2C2_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootI2C2_ER_IRQHandler
        B BootI2C2_ER_IRQHandler

        PUBWEAK BootSPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootSPI1_IRQHandler
        B BootSPI1_IRQHandler    

        PUBWEAK BootSPI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootSPI2_IRQHandler
        B BootSPI2_IRQHandler

        PUBWEAK BootUSART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootUSART1_IRQHandler
        B BootUSART1_IRQHandler

        PUBWEAK BootUSART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootUSART2_IRQHandler
        B BootUSART2_IRQHandler

        PUBWEAK BootUSART3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootUSART3_IRQHandler
        B BootUSART3_IRQHandler

        PUBWEAK BootEXTI15_10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootEXTI15_10_IRQHandler
        B BootEXTI15_10_IRQHandler

        PUBWEAK BootRTCAlarm_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootRTCAlarm_IRQHandler
        B BootRTCAlarm_IRQHandler

        PUBWEAK BootOTG_FS_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootOTG_FS_WKUP_IRQHandler
        B BootOTG_FS_WKUP_IRQHandler

        PUBWEAK BootTIM5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM5_IRQHandler
        B BootTIM5_IRQHandler

        PUBWEAK BootSPI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootSPI3_IRQHandler
        B BootSPI3_IRQHandler

        PUBWEAK BootUART4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootUART4_IRQHandler
        B BootUART4_IRQHandler

        PUBWEAK BootUART5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootUART5_IRQHandler
        B BootUART5_IRQHandler

        PUBWEAK BootTIM6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM6_IRQHandler
        B BootTIM6_IRQHandler

        PUBWEAK BootTIM7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootTIM7_IRQHandler
        B BootTIM7_IRQHandler

        PUBWEAK BootDMA2_Channel1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA2_Channel1_IRQHandler
        B BootDMA2_Channel1_IRQHandler

        PUBWEAK BootDMA2_Channel2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA2_Channel2_IRQHandler
        B BootDMA2_Channel2_IRQHandler

        PUBWEAK BootDMA2_Channel3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA2_Channel3_IRQHandler
        B BootDMA2_Channel3_IRQHandler

        PUBWEAK BootDMA2_Channel4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA2_Channel4_IRQHandler
        B BootDMA2_Channel4_IRQHandler

        PUBWEAK BootDMA2_Channel5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootDMA2_Channel5_IRQHandler
        B BootDMA2_Channel5_IRQHandler

        PUBWEAK BootETH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootETH_IRQHandler
        B BootETH_IRQHandler

        PUBWEAK BootETH_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootETH_WKUP_IRQHandler
        B BootETH_WKUP_IRQHandler

        PUBWEAK BootCAN2_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN2_TX_IRQHandler
        B BootCAN2_TX_IRQHandler

        PUBWEAK BootCAN2_RX0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN2_RX0_IRQHandler
        B BootCAN2_RX0_IRQHandler

        PUBWEAK BootCAN2_RX1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN2_RX1_IRQHandler
        B BootCAN2_RX1_IRQHandler

        PUBWEAK BootCAN2_SCE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BootCAN2_SCE_IRQHandler
        B BootCAN2_SCE_IRQHandler

        PUBWEAK OTG_FS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_FS_IRQHandler
        B OTG_FS_IRQHandler

        END
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
