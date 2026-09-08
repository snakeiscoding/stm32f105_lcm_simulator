#ifndef __PINDEFINE_H__
#define __PINDEFINE_H__
//------------------------------------------------------------------------------
#include  "stm32f10x_conf.h"
#define   _PRODUCT_BOARD
#define   _PRODUCT_V2
#define   _V3             1               //RIS Protocol Version
//#define   _V2             1               //RIS Protocol Version

#define   _V3_Update


#ifdef    _PRODUCT_BOARD
    #ifdef    _PRODUCT_V2
    #define   _SupportPhoenixConsole
    #define   _SupportSerialNumberCommand
    #define   _AT_SupportInclineAutoCalibrate
    #else
    //#define   _SupportPhoenixConsole
    #endif
#else
//#define   _KL_VERSION
//#define   _SupportPhoenixConsole
#endif


#ifdef  _PRODUCT_BOARD
  #ifdef  _PRODUCT_V2
    // Console Power Control
    #define   _ON                         Bit_SET
    #define   _OFF                        Bit_RESET
    #define   _12SW_PIN                   GPIO_Pin_5
    #define   _12SW_GPIO_GROUP            GPIOA
    #define   oConsolePower(s)            GPIO_WriteBit(_12SW_GPIO_GROUP,_12SW_PIN,s) // LED1 System status
    
    //------------------------------------------------------------------------------
    // RS485 for JHT Communication
    #define   _JHT_COMM_IRQ               USART2_IRQn
    #define   _JHT_COMM_IRQ_Func          USART2_IRQHandler
    #define   _JHT_COMM_PORT              USART2
    #define   _JHT_COMM_TXD_PIN           GPIO_Pin_2
    #define   _JHT_COMM_RXD_PIN           GPIO_Pin_3
    #define   _JHT_COMM_CTRL_PIN          GPIO_Pin_1
    #define   _JHT_COMM_GPIO_GROUP        GPIOA
    #define   _RXD                        Bit_RESET
    #define   _TXD                        Bit_SET
    #define   oRS485Rx(s)                 GPIO_WriteBit(_JHT_COMM_GPIO_GROUP,GPIO_Pin_1,s) // LED4 STB Communication Status
    
    //------------------------------------------------------------------------------
    // Encoder A/B Button
    #define   _EN_A_PIN                   GPIO_Pin_6
    #define   _EN_B_PIN                   GPIO_Pin_7
    #define   _EN_Button_PIN              GPIO_Pin_5
    #define   _EN_GPIO_GROUP              GPIOB
    #define   _iButton(s)                 (s = GPIO_ReadInputDataBit(_EN_GPIO_GROUP,_EN_Button_PIN))
    //
    
    //------------------------------------------------------------------------------
    // LCM
    #define   _LCM_BL_PIN             GPIO_Pin_8
    #define   _LCM_BL_GPIO_GROUP      GPIOA
    #define   _LCM_E_PIN              GPIO_Pin_7
    #define   _LCM_E_GPIO_GROUP       GPIOC
    #define   _LCM_RW_PIN             GPIO_Pin_8
    #define   _LCM_RW_GPIO_GROUP      GPIOC
    #define   _LCM_RS_PIN             GPIO_Pin_9
    #define   _LCM_RS_GPIO_GROUP      GPIOC
    
    #define   _LCM_D0_PIN             GPIO_Pin_6
    #define   _LCM_LDATA0_GPIO_GROUP  GPIOC
    #define   _LCM_D1_PIN             GPIO_Pin_15
    #define   _LCM_D2_PIN             GPIO_Pin_14
    #define   _LCM_D3_PIN             GPIO_Pin_13
    #define   _LCM_LDATA1_GPIO_GROUP  GPIOD
    #define   _LCM_D4_PIN             GPIO_Pin_12
    #define   _LCM_D5_PIN             GPIO_Pin_11
    #define   _LCM_D6_PIN             GPIO_Pin_10
    #define   _LCM_D7_PIN             GPIO_Pin_9
    #define   _LCM_HDATA_GPIO_GROUP   GPIOD
    #define   _LCM_POWER_PIN          GPIO_Pin_9
    #define   _LCM_POWER_GPIO_GROUP   GPIOA
    
    
    #define   _BL_ON                  Bit_SET   
    #define   _BL_OFF                 Bit_RESET 
    #define   _LCM_ON                 Bit_SET   
    #define   _LCM_OFF                Bit_RESET 
    
    
    #define   IO_LCM_POWER(s)         GPIO_WriteBit(_LCM_POWER_GPIO_GROUP,_LCM_POWER_PIN,s)
    #define   IO_LCM_BL(s)            GPIO_WriteBit(_LCM_BL_GPIO_GROUP,_LCM_BL_PIN,s)
    #define   IO_LCM_E(s)             GPIO_WriteBit(_LCM_E_GPIO_GROUP,_LCM_E_PIN,s)
    #define   IO_LCM_RW(s)            GPIO_WriteBit(_LCM_RW_GPIO_GROUP,_LCM_RW_PIN,s) 
    #define   IO_LCM_RS(s)            GPIO_WriteBit(_LCM_RS_GPIO_GROUP,_LCM_RS_PIN,s) 
    #define   IO_LCM_oD0(s)           ( s == 0 ? GPIO_WriteBit(_LCM_LDATA0_GPIO_GROUP,_LCM_D0_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_LDATA0_GPIO_GROUP,_LCM_D0_PIN,Bit_SET))
    #define   IO_LCM_oD1(s)           ( s == 0 ? GPIO_WriteBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D1_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D1_PIN,Bit_SET))
    #define   IO_LCM_oD2(s)           ( s == 0 ? GPIO_WriteBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D2_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D2_PIN,Bit_SET))
    #define   IO_LCM_oD3(s)           ( s == 0 ? GPIO_WriteBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D3_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D3_PIN,Bit_SET))
    #define   IO_LCM_oD4(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D4_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D4_PIN,Bit_SET))
    #define   IO_LCM_oD5(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D5_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D5_PIN,Bit_SET))
    #define   IO_LCM_oD6(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D6_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D6_PIN,Bit_SET))
    #define   IO_LCM_oD7(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D7_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP, _LCM_D7_PIN,Bit_SET))
    #define   IO_LCM_iD0()            GPIO_ReadInputDataBit(_LCM_LDATA0_GPIO_GROUP,_LCM_D0_PIN) 
    #define   IO_LCM_iD1()            GPIO_ReadInputDataBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D1_PIN) 
    #define   IO_LCM_iD2()            GPIO_ReadInputDataBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D2_PIN)
    #define   IO_LCM_iD3()            GPIO_ReadInputDataBit(_LCM_LDATA1_GPIO_GROUP,_LCM_D3_PIN)
    #define   IO_LCM_iD4()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN) 
    #define   IO_LCM_iD5()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN) 
    #define   IO_LCM_iD6()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN)
    #define   IO_LCM_iD7()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN)
    
    
    //------------------------------------------------------------------------------
    // EEPROM ATMEL 93C86
    #define   _High                   Bit_SET
    #define   _Low                    Bit_RESET
    #define   _EE_CS_PIN              GPIO_Pin_8
    #define   _EE_CS_GPIO_GROUP       GPIOD
    #define   _EE_SK_PIN              GPIO_Pin_15
    #define   _EE_SK_GPIO_GROUP       GPIOB
    #define   _EE_DI_PIN              GPIO_Pin_14
    #define   _EE_DI_GPIO_GROUP       GPIOB
    #define   _EE_DO_PIN              GPIO_Pin_13
    #define   _EE_DO_GPIO_GROUP       GPIOB
    
    #define   oEEPROM_CS(s)	      GPIO_WriteBit(_EE_CS_GPIO_GROUP,_EE_CS_PIN,s)
    #define   oEEPROM_SK(s)	      GPIO_WriteBit(_EE_SK_GPIO_GROUP,_EE_SK_PIN,s)
    #define   oEEPROM_DI(s)	      GPIO_WriteBit(_EE_DI_GPIO_GROUP,_EE_DI_PIN,s)
    #define   iEEPROM_DO	      GPIO_ReadInputDataBit(_EE_DO_GPIO_GROUP,_EE_DO_PIN)
    //------------------------------------------------------------------------------
    #define   _SafetyKey_PIN          GPIO_Pin_1
    #define   _SafetyKey_GPIO_GROUP   GPIOB
    #define   iSafetyKey	          GPIO_ReadInputDataBit(_SafetyKey_GPIO_GROUP,_SafetyKey_PIN)
    #define   _SafetyKeyPress         Bit_RESET
    #define   _SafetyKeyReleas        Bit_SET
    // Phoenix Console
    #define   _oBATON_PIN             GPIO_Pin_2
    #define   _oBATON_GPIO_GROUP      GPIOE
    #define   _iBATON_PIN             GPIO_Pin_3
    #define   _iBATON_GPIO_GROUP      GPIOC
    #define   oBATON(s)	              GPIO_WriteBit(_oBATON_GPIO_GROUP,_oBATON_PIN,s)
    #define   iBATON	              GPIO_ReadInputDataBit(_iBATON_GPIO_GROUP,_iBATON_PIN)
    #define   _BATONPress             Bit_SET
    #define   _BATONReleas            Bit_RESET
    #define   _BATON                  Bit_SET
    #define   _BATOFF                 Bit_RESET
        // Master/Salve
    #define   iLCM_DMS_PIN            GPIO_Pin_0
    #define   iLCM_DMS_GPIO_GROUP     GPIOA
    #define   iLCM_DMS	              GPIO_ReadInputDataBit(iLCM_DMS_GPIO_GROUP,iLCM_DMS_PIN)
    #define   _Master                 Bit_SET
    #define   _Salve                  Bit_RESET

    // ADS1100 I2C
    #define   _ADS1100_SCL            GPIO_Pin_10
    #define   _ADS1100_SCL_GPIO_GROUP GPIOB
    #define   _ADS1100_SDA            GPIO_Pin_11
    #define   _ADS1100_SDA_GPIO_GROUP GPIOB
    #define   iADS1100_SDA	      GPIO_ReadInputDataBit(_ADS1100_SDA_GPIO_GROUP,_ADS1100_SDA)
    #define   oADS1100_SDA(s)	      GPIO_WriteBit(_ADS1100_SDA_GPIO_GROUP,_ADS1100_SDA,s)
    #define   oADS1100_SCL(s)	      GPIO_WriteBit(_ADS1100_SCL_GPIO_GROUP,_ADS1100_SCL,s)
    //-------------------------------------------------------------------------------------------------------
    // Analog Input for ADC12
    #define   _V12_PIN                GPIO_Pin_4
    #define   _V12_GPIO_GROUP         GPIOA
    #define   _V12_ADC_Channel        ADC_Channel_4
    //-------------------------------------------------------------------------------------------------------
  #else
    // Console Power Control
    #define   _ON                         Bit_SET
    #define   _OFF                        Bit_RESET
    #define   _12SW_PIN                   GPIO_Pin_0
    #define   _12SW_GPIO_GROUP            GPIOC
    #define   oConsolePower(s)            GPIO_WriteBit(_12SW_GPIO_GROUP,_12SW_PIN,s) // LED1 System status
    
    //------------------------------------------------------------------------------
    // RS485 for JHT Communication
    #define   _JHT_COMM_IRQ               USART2_IRQn
    #define   _JHT_COMM_IRQ_Func          USART2_IRQHandler
    #define   _JHT_COMM_PORT              USART2
    #define   _JHT_COMM_TXD_PIN           GPIO_Pin_2
    #define   _JHT_COMM_RXD_PIN           GPIO_Pin_3
    #define   _JHT_COMM_CTRL_PIN          GPIO_Pin_1
    #define   _JHT_COMM_GPIO_GROUP        GPIOA
    #define   _RXD                        Bit_RESET
    #define   _TXD                        Bit_SET
    #define   oRS485Rx(s)                 GPIO_WriteBit(_JHT_COMM_GPIO_GROUP,GPIO_Pin_1,s) // LED4 STB Communication Status
    
    //------------------------------------------------------------------------------
    // Encoder A/B Button
    #define   _EN_A_PIN                   GPIO_Pin_6
    #define   _EN_B_PIN                   GPIO_Pin_7
    #define   _EN_Button_PIN              GPIO_Pin_4
    #define   _EN_GPIO_GROUP              GPIOA
    #define   _iButton(s)                 (s = GPIO_ReadInputDataBit(_EN_GPIO_GROUP,_EN_Button_PIN))
    //
    
    //------------------------------------------------------------------------------
    // LCM
    #define   _LCM_BL_PIN             GPIO_Pin_5
    #define   _LCM_BL_GPIO_GROUP      GPIOB
    #define   _LCM_E_PIN              GPIO_Pin_8
    #define   _LCM_E_GPIO_GROUP       GPIOA
    #define   _LCM_RW_PIN             GPIO_Pin_11
    #define   _LCM_RW_GPIO_GROUP      GPIOA
    #define   _LCM_RS_PIN             GPIO_Pin_12
    #define   _LCM_RS_GPIO_GROUP      GPIOA
    
    #define   _LCM_D0_PIN             GPIO_Pin_6
    #define   _LCM_D1_PIN             GPIO_Pin_7
    #define   _LCM_D2_PIN             GPIO_Pin_8
    #define   _LCM_D3_PIN             GPIO_Pin_9
    #define   _LCM_LDATA_GPIO_GROUP    GPIOC
    #define   _LCM_D4_PIN             GPIO_Pin_12
    #define   _LCM_D5_PIN             GPIO_Pin_13
    #define   _LCM_D6_PIN             GPIO_Pin_14
    #define   _LCM_D7_PIN             GPIO_Pin_15
    #define   _LCM_HDATA_GPIO_GROUP   GPIOB
    #define   _LCM_POWER_PIN          GPIO_Pin_9
    #define   _LCM_POWER_GPIO_GROUP   GPIOA
    
    
    #define   _BL_ON                  Bit_SET   
    #define   _BL_OFF                 Bit_RESET 
    #define   _LCM_ON                 Bit_SET   
    #define   _LCM_OFF                Bit_RESET 
    
    
    #define   IO_LCM_POWER(s)         GPIO_WriteBit(_LCM_POWER_GPIO_GROUP,_LCM_POWER_PIN,s)
    #define   IO_LCM_BL(s)            GPIO_WriteBit(_LCM_BL_GPIO_GROUP,_LCM_BL_PIN,s)
    #define   IO_LCM_E(s)             GPIO_WriteBit(_LCM_E_GPIO_GROUP,_LCM_E_PIN,s)
    #define   IO_LCM_RW(s)            GPIO_WriteBit(_LCM_RW_GPIO_GROUP,_LCM_RW_PIN,s) 
    #define   IO_LCM_RS(s)            GPIO_WriteBit(_LCM_RS_GPIO_GROUP,_LCM_RS_PIN,s) 
    #define   IO_LCM_oD4(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN,Bit_SET))
    #define   IO_LCM_oD5(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN,Bit_SET))
    #define   IO_LCM_oD6(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN,Bit_SET))
    #define   IO_LCM_oD7(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN,Bit_SET))
    #define   IO_LCM_iD4()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN) 
    #define   IO_LCM_iD5()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN) 
    #define   IO_LCM_iD6()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN)
    #define   IO_LCM_iD7()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN)
    
    
    //------------------------------------------------------------------------------
    // EEPROM ATMEL 93C86
    #define   _High                   Bit_SET
    #define   _Low                    Bit_RESET
    #define   _EE_CS_PIN              GPIO_Pin_2
    #define   _EE_CS_GPIO_GROUP       GPIOD
    #define   _EE_SK_PIN              GPIO_Pin_12
    #define   _EE_SK_GPIO_GROUP       GPIOC
    #define   _EE_DI_PIN              GPIO_Pin_11
    #define   _EE_DI_GPIO_GROUP       GPIOC
    #define   _EE_DO_PIN              GPIO_Pin_10
    #define   _EE_DO_GPIO_GROUP       GPIOC
    
    #define   oEEPROM_CS(s)	      GPIO_WriteBit(_EE_CS_GPIO_GROUP,_EE_CS_PIN,s)
    #define   oEEPROM_SK(s)	      GPIO_WriteBit(_EE_SK_GPIO_GROUP,_EE_SK_PIN,s)
    #define   oEEPROM_DI(s)	      GPIO_WriteBit(_EE_DI_GPIO_GROUP,_EE_DI_PIN,s)
    #define   iEEPROM_DO	      GPIO_ReadInputDataBit(_EE_DO_GPIO_GROUP,_EE_DO_PIN)
    //------------------------------------------------------------------------------
    #define   _SafetyKey_PIN          GPIO_Pin_1
    #define   _SafetyKey_GPIO_GROUP   GPIOB
    #define   iSafetyKey	      GPIO_ReadInputDataBit(_SafetyKey_GPIO_GROUP,_SafetyKey_PIN)
    #define   _SafetyKeyPress         Bit_RESET
    #define   _SafetyKeyReleas        Bit_SET
#ifdef _SupportPhoenixConsole
    #define   _oBATON_PIN             GPIO_Pin_9
    #define   _oBATON_GPIO_GROUP      GPIOB
    #define   _iBATON_PIN             GPIO_Pin_8
    #define   _iBATON_GPIO_GROUP      GPIOB
    #define   oBATON(s)	              GPIO_WriteBit(_oBATON_GPIO_GROUP,_oBATON_PIN,s)
    #define   iBATON	              GPIO_ReadInputDataBit(_iBATON_GPIO_GROUP,_iBATON_PIN)
    #define   _BATONPress             Bit_RESET
    #define   _BATONReleas            Bit_SET
#endif
    #define   iLCM_DMS	              GPIO_ReadInputDataBit(_SafetyKey_GPIO_GROUP,_SafetyKey_PIN)
    #define   _Master                 Bit_RESET
    #define   _Salve                  Bit_SET

    // ADS1100 I2C
    #define   _ADS1100_SCL            GPIO_Pin_10
    #define   _ADS1100_SCL_GPIO_GROUP GPIOB
    #define   _ADS1100_SDA            GPIO_Pin_11
    #define   _ADS1100_SDA_GPIO_GROUP GPIOB
    #define   iADS1100_SDA	      GPIO_ReadInputDataBit(_ADS1100_SDA_GPIO_GROUP,_ADS1100_SDA)
    #define   oADS1100_SDA(s)	      GPIO_WriteBit(_ADS1100_SDA_GPIO_GROUP,_ADS1100_SDA,s)
    #define   oADS1100_SCL(s)	      GPIO_WriteBit(_ADS1100_SCL_GPIO_GROUP,_ADS1100_SCL,s)
    //-------------------------------------------------------------------------------------------------------
    // Analog Input for ADC12
    #define   _V12_PIN                GPIO_Pin_0
    #define   _V12_GPIO_GROUP         GPIOB
    #define   _V12_ADC_Channel        ADC_Channel_8
    //-------------------------------------------------------------------------------------------------------
  #endif    
#else

#ifdef  _KL_VERSION
#define _DIV4_                        1
#endif

// Console Power Control
#define   _ON                         Bit_SET
#define   _OFF                        Bit_RESET
#define   _12SW_PIN                   GPIO_Pin_10
#define   _12SW_GPIO_GROUP            GPIOB
#define   oConsolePower(s)            GPIO_WriteBit(GPIOB,GPIO_Pin_10,s) // LED1 System status

//------------------------------------------------------------------------------
// LED Control
#define   _LED_ON                     Bit_RESET
#define   _LED_OFF                    Bit_SET

// STATUS LED
// Output PC6: LED1
//        PC7: LED2
//        PC8: LED3
//        PC9: LED4
//#define   STATUSLED1(s)               GPIO_WriteBit(GPIOC,GPIO_Pin_6,s) // LED1 System status
//#define   STATUSLED2(s)               GPIO_WriteBit(GPIOC,GPIO_Pin_7,s) // LED2 Communication status
//#define   STATUSLED3(s)               GPIO_WriteBit(GPIOC,GPIO_Pin_8,s) // LED3 IR Output Control Mode code / Pattern Mode
//#define   STATUSLED4(s)               GPIO_WriteBit(GPIOC,GPIO_Pin_9,s) // LED4 STB Communication Status
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RS485 for JHT Communication
#define   _JHT_COMM_IRQ               USART2_IRQn
#define   _JHT_COMM_IRQ_Func          USART2_IRQHandler
#define   _JHT_COMM_PORT              USART2
#define   _JHT_COMM_TXD_PIN           GPIO_Pin_2
#define   _JHT_COMM_RXD_PIN           GPIO_Pin_3
#define   _JHT_COMM_CTRL_PIN          GPIO_Pin_1
#define   _JHT_COMM_GPIO_GROUP        GPIOA
#define   _RXD                        Bit_RESET
#define   _TXD                        Bit_SET
#define   oRS485Rx(s)                 GPIO_WriteBit(_JHT_COMM_GPIO_GROUP,GPIO_Pin_1,s) // LED4 STB Communication Status

//------------------------------------------------------------------------------
// Encoder A/B Button
#ifdef  _KL_VERSION
#define   _EN_A_PIN                   GPIO_Pin_6
#define   _EN_B_PIN                   GPIO_Pin_7
#define   _EN_Button_PIN              GPIO_Pin_5
#define   _EN_GPIO_GROUP              GPIOA
#else
#define   _EN_A_PIN                   GPIO_Pin_6
#define   _EN_B_PIN                   GPIO_Pin_7
#define   _EN_Button_PIN              GPIO_Pin_4
#define   _EN_GPIO_GROUP              GPIOA
#endif
#define   _iButton(s)                 (s = GPIO_ReadInputDataBit(_EN_GPIO_GROUP,_EN_Button_PIN))
//

//------------------------------------------------------------------------------
// LCM
#define   _LCM_BL_PIN             GPIO_Pin_0
#define   _LCM_BL_GPIO_GROUP      GPIOA
#define   _LCM_E_PIN              GPIO_Pin_8
#define   _LCM_E_GPIO_GROUP       GPIOA
#define   _LCM_RW_PIN             GPIO_Pin_11
#define   _LCM_RW_GPIO_GROUP      GPIOA
#define   _LCM_RS_PIN             GPIO_Pin_12
#define   _LCM_RS_GPIO_GROUP      GPIOA
#define   _LCM_D4_PIN             GPIO_Pin_12
#define   _LCM_D5_PIN             GPIO_Pin_13
#define   _LCM_D6_PIN             GPIO_Pin_14
#define   _LCM_D7_PIN             GPIO_Pin_15
#define   _LCM_HDATA_GPIO_GROUP    GPIOB
#define   _LCM_POWER_PIN          GPIO_Pin_3
#define   _LCM_POWER_GPIO_GROUP   GPIOC


#ifdef  _KL_VERSION
#define   _BL_ON                  Bit_RESET
#define   _BL_OFF                 Bit_SET
#define   _LCM_ON                 Bit_RESET
#define   _LCM_OFF                Bit_SET
#else
#define   _BL_ON                  Bit_SET   
#define   _BL_OFF                 Bit_RESET 
#define   _LCM_ON                 Bit_SET   
#define   _LCM_OFF                Bit_RESET 
#endif

#define   IO_LCM_POWER(s)         GPIO_WriteBit(_LCM_POWER_GPIO_GROUP,_LCM_POWER_PIN,s)
#define   IO_LCM_BL(s)            GPIO_WriteBit(_LCM_BL_GPIO_GROUP,_LCM_BL_PIN,s)
#define   IO_LCM_E(s)             GPIO_WriteBit(_LCM_E_GPIO_GROUP,_LCM_E_PIN,s)
#define   IO_LCM_RW(s)            GPIO_WriteBit(_LCM_RW_GPIO_GROUP,_LCM_RW_PIN,s) 
#define   IO_LCM_RS(s)            GPIO_WriteBit(_LCM_RS_GPIO_GROUP,_LCM_RS_PIN,s) 
#define   IO_LCM_oD4(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN,Bit_SET))
#define   IO_LCM_oD5(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN,Bit_SET))
#define   IO_LCM_oD6(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN,Bit_SET))
#define   IO_LCM_oD7(s)           ( s == 0 ? GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN,Bit_RESET) : GPIO_WriteBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN,Bit_SET))
#define   IO_LCM_iD4()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D4_PIN) 
#define   IO_LCM_iD5()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D5_PIN) 
#define   IO_LCM_iD6()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D6_PIN)
#define   IO_LCM_iD7()            GPIO_ReadInputDataBit(_LCM_HDATA_GPIO_GROUP,_LCM_D7_PIN)


//------------------------------------------------------------------------------
// EEPROM ATMEL 93C86
#define   _High                   Bit_SET
#define   _Low                    Bit_RESET
#define   _EE_CS_PIN              GPIO_Pin_10
#define   _EE_CS_GPIO_GROUP       GPIOC
#define   _EE_SK_PIN              GPIO_Pin_11
#define   _EE_SK_GPIO_GROUP       GPIOC
#define   _EE_DI_PIN              GPIO_Pin_12
#define   _EE_DI_GPIO_GROUP       GPIOC
#define   _EE_DO_PIN              GPIO_Pin_2
#define   _EE_DO_GPIO_GROUP       GPIOD

#define	  oEEPROM_CS(s)		  GPIO_WriteBit(_EE_CS_GPIO_GROUP,_EE_CS_PIN,s)
#define	  oEEPROM_SK(s)		  GPIO_WriteBit(_EE_SK_GPIO_GROUP,_EE_SK_PIN,s)
#define	  oEEPROM_DI(s)		  GPIO_WriteBit(_EE_DI_GPIO_GROUP,_EE_DI_PIN,s)
#define	  iEEPROM_DO		  GPIO_ReadInputDataBit(_EE_DO_GPIO_GROUP,_EE_DO_PIN)
//------------------------------------------------------------------------------
#define   _SafetyKey_PIN          GPIO_Pin_1
#define   _SafetyKey_GPIO_GROUP   GPIOB
#define	  iSafetyKey		  GPIO_ReadInputDataBit(_SafetyKey_GPIO_GROUP,_SafetyKey_PIN)
#define   _SafetyKeyPress         Bit_RESET
#define   _SafetyKeyReleas        Bit_SET

#define   _oBATON_PIN             GPIO_Pin_0
#define   _oBATON_GPIO_GROUP      GPIOB
#define   _iBATON_PIN             GPIO_Pin_4
#define   _iBATON_GPIO_GROUP      GPIOC
#define   oBATON(s)	          GPIO_WriteBit(_oBATON_GPIO_GROUP,_oBATON_PIN,s)
#define   iBATON	          GPIO_ReadInputDataBit(_iBATON_GPIO_GROUP,_iBATON_PIN)
#define   _BATONPress             Bit_RESET
#define   _BATONReleas            Bit_SET




#define   iLCM_DMS	          GPIO_ReadInputDataBit(_SafetyKey_GPIO_GROUP,_SafetyKey_PIN)
#define   _Master                 Bit_RESET
#define   _Salve                  Bit_SET

#endif



#endif /* __PINDEFINE_H__ */


