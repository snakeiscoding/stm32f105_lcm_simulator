/**************************************************************************//**
 * @file     main.c
 * @brief    
 * @version  
 * @date     
 *
 *
 ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#include  "LCBSimulator.h"
#include  "JHTLCBComm.h"
#include  "JHTCOMMAND.h"
#include  "LCM.h"
#include  "LCMDisplay.h"
#include  "Screen.h"
#include  "Encoder.h"
#include  "EE93CXX.h"

#ifdef _PRODUCT_BOARD
#include  "ADC.h"
#include  "ADS1100.h"
#endif

#ifdef  _PRODUCT_V2
#if __ICCARM__
extern __root const unsigned char User_Version[20] ;
#elif __GNUC__
#include "BoardInfo.h"
extern __attribute__((section(".userinfo"), used)) const UserInfo_T UserInfo;
#endif
#endif


#ifdef _SupportPhoenixConsole
#include  "RowerWatts.h"
#include  "SinTable.h"
#endif

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define   SimulatorMain_SystemTick        SysTick_Handler
#define   _MaxMachineType                 51  // 20220216 39->40 add support Cxp Update LCB
                                              // 20220314 40->41 add support ICR70 LCB
                                              // 20230522 41->43 add support Global MCB
                                              // 20250526 43->44 add support impulse Climbmill LCB
                                              // 20251215 44->46 add support Universal LCB and GTM LCB
                                              // 20260126 46->51 
#ifdef    _SupportPhoenixConsole           // Indoor Cycle VBAT_ON
#define   _MachineTypeSelectLimit         _MaxMachineType    
#else
#define   _MachineTypeSelectLimit         _MaxMachineType-8
#endif
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
#if __ICCARM__
__no_init unsigned short SystemStatusLED ;
#elif __GNUC__
__attribute__((section(".noinit"))) unsigned short SystemStatusLED ;
#endif


#ifndef _V3_Update
typedef struct {
  unsigned char Type ;
  unsigned char Screen[30] ;
} MTypeDataStruct ;

const  MTypeDataStruct MType[_MaxMachineType] = { 
    {_LCB1_       ,"ST/BK/EP LCB1 07"},
    {_LCB2_       ,"Ascent Trainer LCB2 0A"},
    {_LCB3_       ,"Ascent Trainer LCB3 08"},
    {_Climbmill_  ,"Climbmill 0B"},
    {_ClimbmillEN_,"EN Climbmill 17"},
    {_ClimbmillEN_,"Liftstyle Climbmill 17"},
    {_ClimbmillR_ ,"Retail Climbmill 19"},
    {_LCBA_       ,"Chopper HUR LCBA 13"},
    {_LCBA_M_     ,"R3xm LCB 18"},
    {_Athena      ,"Athena LCB 1B"},      
    {_LCB1x_      ,"JIS 1x LCB1x 14"},
    {_DCI_        ,"DCI      C0"},
    {_TopTek_     ,"TopTek   C1"},  
    {_Delta_I     ,"Delta    C2"}, 
    {_Delta_H     ,"Delta    C3"}, 
    {_Delta_I1    ,"Delta    C4"}, 
    {_Delta_I2    ,"Delta    C5"}, 
    {_Delta_I3    ,"Delta    C6"}, 
    {_Liteon      ,"Liteon   C7"}, 
    {_Delta_I4    ,"Delta    C8"}, 
    {_LiteonJHT   ,"Johnson  F0"},
    {_VAVE_A1     ,"VAVE MCB A1"},
    {_VAVE_A2     ,"VAVE MCB A2"},
    {_VAVE_A3     ,"VAVE MCB A3"},
    {_VAVE_F1     ,"VAVE MCB F1"},    
    {_JISECB_I    ,"JIS A30  0B"},
    {_JISEM       ,"JIS U/R/SE50I 0C"},
    {_JISDCTM     ,"JIS T30/50 TF30/50 0E"},
    {_JISECB      ,"JIS U/R/SE30 12"},
    {_JISEM_I     ,"JIS A50I 15"},
    {_JISACTM110  ,"JIS T90-110V E0"},
    {_JISACTM220  ,"JIS T90-220V E1"},  
    {_ITC         ,"ITC Load Cell 16"}, 
    {_JISDCLCB    ,"JIS DC MCB 04"},
    {_JISICR70LCB ,"JIS ICR70 LCB 1D"},
    {_JISGMCB110  ,"JIS GloablMCB D0"},
    {_JISGMCB220  ,"JIS GloablMCB D1"},
    {_IMPULSE_LCB ,"IMPULSE LCB 1E" } ,
    {_ATLCB2_2026 ,"Ascent Trainer LCB 1F"},
    {_BKLCBA_2026 ,"HUR LCBA 21"} ,
    {_CMLCB_2026  ,"EN Climbmill 20"} ,
    {_PS_PMSM     ,"PS PMSM LCB C9"} ,
    {_PP_PMSM     ,"PP PMSM LCB F2"} ,
    //
    {_IndoorCycle ,"Indoor Cycle LCB 01"},
    {_CxpUpdate   ,"CxP Update   LCB 03"},
    {_Rower       ,"Rower LCB 02"},
    {_SDrive      ,"S-Drive LCB 02"},
    {_UBCM        ,"Upper Body Cycle(M) 03"},
    {_UBCP        ,"Upper Body Cycle(P) 04"},
    {_Universal   ,"Universal LCB 05"},
    {_GTM         ,"GTM LCB 06"}    

} ;
#else
typedef struct {
  unsigned char Type ;
  unsigned char ModelName[15] ;
  unsigned char LcbType[17] ;
} MTypeDataStruct ;
const  MTypeDataStruct MType[_MaxMachineType] = { 
  //                12345678901234   1234567890123456
    {_LCB1_       ,"ST/BK/EP      ","LCB1          07"},
    {_LCB2_       ,"A3x Ascent    ","Ascent LCB2   0A"},
    {_LCB3_       ,"PS Ascent     ","Ascent LCB3   08"},
    {_Climbmill_  ,"Climbmill     ","LCBCM1        0B"},
    {_ClimbmillEN_,"EN CM         ","LCBCM2        17"},
    {_ClimbmillEN_,"LS CM         ","LCBCM2        17"},
    {_ClimbmillR_ ,"MxR CM        ","LCBCM3        19"},
    {_LCBA_       ,"Chopper HUR   ","LCBA          13"},
    {_LCBA_M_     ,"R3xM          ","MedCE LCB     18"},
    {_Athena      ,"Athena        ","Athena LCB    1B"},      
    {_LCB1x_      ,"HURE 1x       ","LCB1x         14"},
    {_DCI_        ,"Treadmill     ","DCI-Old       C0"},
    {_TopTek_     ,"Treadmill     ","TopTek        C1"},  
    {_Delta_I     ,"Treadmill     ","Delta         C2"}, 
    {_Delta_H     ,"Treadmill     ","Delta         C3"}, 
    {_Delta_I1    ,"Treadmill     ","Delta         C4"}, 
    {_Delta_I2    ,"Treadmill     ","Delta         C5"}, 
    {_Delta_I3    ,"Treadmill     ","Delta         C6"}, 
    {_Liteon      ,"Treadmill     ","Liteon        C7"}, 
    {_Delta_I4    ,"Treadmill     ","Delta TMP+    C8"}, 
    {_LiteonJHT   ,"Treadmill     ","Johnson       F0"},
    {_VAVE_A1     ,"Treadmill     ","Johnson       A1"},
    {_VAVE_A2     ,"Treadmill     ","Johnson       A2"},
    {_VAVE_A3     ,"Treadmill     ","Johnson       A3"},
    {_VAVE_F1     ,"Treadmill     ","Johnson       F1"},    
    {_JISECB_I    ,"MxR A30       ","DC-EP/AT      0B"},
    {_JISEM       ,"URE 50        ","JIS-LCB50     0C"},
    {_JISDCTM     ,"T30/T50       ","DC-TM-LCB     0E"},
    {_JISECB      ,"URE 30        ","JIS-ECB30     12"},
    {_JISEM_I     ,"MxR A50       ","Indc_Brake    15"},
    {_JISACTM110  ,"ST90-110V     ","MLHI701H      E0"},
    {_JISACTM220  ,"ST90-220V     ","MLHI701H      E1"}, 
    {_ITC         ,"ITC           ","Load Cell     16"},
    {_JISDCLCB    ,"Treadmill     ","JIS DC MCB    04"},
    {_JISICR70LCB ,"IndoorCycle   ","JIS IRC70 MCB 1D"},
    {_JISGMCB110  ,"Treadmill     ","Global DCMCB  D0"},
    {_JISGMCB220  ,"Treadmill     ","Global DCMCB  D1"},
    {_IMPULSE_LCB ,"Climbmill     ","IMPULSE LCB   1E"},
    {_ATLCB2_2026 ,"Ascent        ","Ascent LCB    1F"},
    {_BKLCBA_2026 ,"HUR BIKE      ","HUR LCBA      21"},
    {_CMLCB_2026  ,"Climbmill     ","LCBCM         20"},
    {_PS_PMSM     ,"Treadmill     ","PS PMSM LCB   C9"},
    {_PP_PMSM     ,"Treadmill     ","PP PMSM LCB   F2"},
    // J4(10 pin) for Phoenix Console
    {_IndoorCycle ,"IndoorCycle   ","CxP LCB       01"},
    {_CxpUpdate   ,"IndoorCycle   ","CxP Update    03"},
    {_Rower       ,"Rower         ","RxP LCB       02"},
    {_SDrive      ,"S-Drive       ","SxP LCB       02"},
    {_UBCM        ,"UBCM          ","UBCM LCB      03"},
    {_UBCP        ,"UBCP          ","UBCP LCB      04"},    
    {_Universal   ,"Universal     ","Universal LCB 05"},
    {_GTM         ,"GTM           ","GTM LCB       06"}
} ;

#endif

                                   //12345678901234
const unsigned char MTypeLCB1_0[] = "EP/BIKE     07" ;
const unsigned char MTypeLCB1_1[] = "STEPPER     07" ;

const unsigned char RIS_MType[5] = { 0, 2, 1, 3, 3 } ;

/* Private function prototypes -----------------------------------------------*/
void SimulatorMain_Initial(void) ;
unsigned short SimulatorMain_CalculatorTimeTick( unsigned short last_TimeTick ) ;

volatile unsigned short MainSystemTime ;
unsigned short LCBSystemTime ;



/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles simulator initial Handler.
  * @param  None
  * @retval None
  */
//------------------------------------------------------------------------------
void SimulatorMain_Initial(void) 
{
  GPIO_InitTypeDef          GPIO_InitStructure; 
 
  //
  SystemInit() ;
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
#ifdef _PRODUCT_BOARD    
  #ifdef _PRODUCT_V2
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_USART2 ,ENABLE );
  #else  
  /* Enable Device clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_USART2 ,ENABLE );
  #endif
  // Enable DMA1 clock 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

#else
  /* Enable Device clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_USART2 ,ENABLE );
#endif  
  //----------------------------------------------------------------------------
  // Initial GPIO
  //----------------------------------------------------------------------------
  // E-STOP Switch signal Input
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Pin = _SafetyKey_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
  GPIO_Init(_SafetyKey_GPIO_GROUP, &GPIO_InitStructure);  
#ifdef _SupportPhoenixConsole // Indoor Cycle VBAT_ON
  oBATON(_BATONReleas);
  GPIO_InitStructure.GPIO_Pin = _oBATON_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; //GPIO_Mode_IN_FLOATING ;
  GPIO_Init(_oBATON_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = _iBATON_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
  GPIO_Init(_iBATON_GPIO_GROUP, &GPIO_InitStructure); 
  // Add by Kunlung 20181019
  #ifdef   _PRODUCT_V2
  GPIO_InitStructure.GPIO_Pin = iLCM_DMS_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING ;
  GPIO_Init(iLCM_DMS_GPIO_GROUP, &GPIO_InitStructure);  
  #endif  
  // for Rower
  RowerWatts_ClearData() ;
  RowerWatts_Initial();
#endif  

  //----------------------------------------------------------------------------
  // Console power control
  oConsolePower(_OFF) ;
  GPIO_InitStructure.GPIO_Pin = _12SW_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_Init(_12SW_GPIO_GROUP, &GPIO_InitStructure);
  //----------------------------------------------------------------------------
  // LCM Control GPIO Configuare
  LCM_HW_Initial() ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; 
  // Enable
  GPIO_InitStructure.GPIO_Pin = _LCM_E_PIN ;
  GPIO_Init(_LCM_E_GPIO_GROUP, &GPIO_InitStructure);
  // Read/Write
  GPIO_InitStructure.GPIO_Pin = _LCM_RW_PIN ;
  GPIO_Init(_LCM_RW_GPIO_GROUP, &GPIO_InitStructure);
  // Data/Instruction
  GPIO_InitStructure.GPIO_Pin = _LCM_RS_PIN ;
  GPIO_Init(_LCM_RS_GPIO_GROUP, &GPIO_InitStructure);  
  // Back light
  GPIO_InitStructure.GPIO_Pin = _LCM_BL_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(_LCM_BL_GPIO_GROUP, &GPIO_InitStructure);    
  // LCM OFF
  GPIO_InitStructure.GPIO_Pin = _LCM_POWER_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(_LCM_POWER_GPIO_GROUP, &GPIO_InitStructure); 
#ifdef	_LCMDataSize4   
  // Data D4~D7
  GPIO_InitStructure.GPIO_Pin = _LCM_D4_PIN | _LCM_D5_PIN  | _LCM_D6_PIN | _LCM_D7_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_Init(_LCM_HDATA_GPIO_GROUP, &GPIO_InitStructure);	
#else
  GPIO_InitStructure.GPIO_Pin = _LCM_D0_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;
  GPIO_Init(_LCM_LDATA0_GPIO_GROUP, &GPIO_InitStructure);	
  // Data D3~D1  
  GPIO_InitStructure.GPIO_Pin = _LCM_D3_PIN | _LCM_D2_PIN  | _LCM_D1_PIN ;
  GPIO_Init(_LCM_LDATA1_GPIO_GROUP, &GPIO_InitStructure);	  
  // Data D4~D7
  GPIO_InitStructure.GPIO_Pin = _LCM_D4_PIN | _LCM_D5_PIN  | _LCM_D6_PIN | _LCM_D7_PIN ;
  GPIO_Init(_LCM_HDATA_GPIO_GROUP, &GPIO_InitStructure);	  
#endif  
  
  //----------------------------------------------------------------------------
  // EEPROM 93C86 Configuare
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; 
  // Chip Select
  GPIO_InitStructure.GPIO_Pin = _EE_CS_PIN ;
  GPIO_Init(_EE_CS_GPIO_GROUP, &GPIO_InitStructure);
  // Clcok
  GPIO_InitStructure.GPIO_Pin = _EE_SK_PIN ;
  GPIO_Init(_EE_SK_GPIO_GROUP, &GPIO_InitStructure);
  // Data Out to EEPROM
  GPIO_InitStructure.GPIO_Pin = _EE_DI_PIN ;
  GPIO_Init(_EE_DI_GPIO_GROUP, &GPIO_InitStructure);  
  // Dadat In from EEPROM
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
  GPIO_InitStructure.GPIO_Pin = _EE_DO_PIN ;
  GPIO_Init(_EE_DO_GPIO_GROUP, &GPIO_InitStructure); 
  
  //----------------------------------------------------------------------------
  // Rs485 Conmunication Configuare
  // Configuare Rx/Tx direct as output push-pull
  GPIO_InitStructure.GPIO_Pin = _JHT_COMM_CTRL_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(_JHT_COMM_GPIO_GROUP, &GPIO_InitStructure);	
  
  // Configure Rx as input push-up 
  GPIO_InitStructure.GPIO_Pin = _JHT_COMM_RXD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(_JHT_COMM_GPIO_GROUP, &GPIO_InitStructure);
  
  // Configure Tx as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = _JHT_COMM_TXD_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(_JHT_COMM_GPIO_GROUP, &GPIO_InitStructure);    
  
  // Modify by Kunlung 20170512
  EE93CXX_ReadDataFromEeprom(_EEMachineType,1,&LCBParameter.LCBSimulatorType) ;
  //Modify 20210413
  //if( LCBParameter.LCBSimulatorType == _IndoorCycle || LCBParameter.LCBSimulatorType == _SDrive || LCBParameter.LCBSimulatorType == _Rower )
  //Modify 20220216
  //if( LCBParameter.LCBSimulatorType == _IndoorCycle || LCBParameter.LCBSimulatorType == _SDrive || LCBParameter.LCBSimulatorType == _Rower || LCBParameter.LCBSimulatorType == _UBCM || LCBParameter.LCBSimulatorType == _UBCP )
  if( LCBParameter.LCBSimulatorType == _IndoorCycle || LCBParameter.LCBSimulatorType == _SDrive || LCBParameter.LCBSimulatorType == _Rower || LCBParameter.LCBSimulatorType == _UBCM || LCBParameter.LCBSimulatorType == _UBCP || LCBParameter.LCBSimulatorType == _CxpUpdate || LCBParameter.LCBSimulatorType == _Universal || LCBParameter.LCBSimulatorType == _GTM )
      {
      JHTLCBComm_RIS_HW_Initial() ;
      }
  else
      {
      JHTLCBComm_HW_Initial() ;
      }
  /*-----Timer 3 Encoder, PA6, PA7 , Button PA5*/
  // Encoder A/B Button
  Encoder_Initial() ;
  //
#ifdef _PRODUCT_BOARD  
  ADC_Initial() ;
  ADS1100_Initial() ;
#endif
  //----------------------------------------------------------------------------
  /* Setup SysTick Timer for 1 msec interrupts  */
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);
  //----------------------------------------------------------------------------
  LCM_Initial() ;
  //20210602
  LCMDisplay_InitialCreatCharacter() ;
  //----------------------------------------------------------------------------
  //
//#ifdef _PRODUCT_BOARD  
  IO_LCM_BL(_BL_ON) ;
//#endif  
  
  return ;
}

const unsigned char RowerSpeedTable[8][2] = {
  { 2 , 7 },  { 2 , 6 },  { 2 , 5 }, { 2 , 4 }, { 2 , 3 } , { 2 , 2 } , { 4 , 3 } , { 5 , 3 }   
} ;
  
/*******************************************************************************
* Function Name  : LCBMain_SystemTick
* Description    : 1ms Interrupt
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SimulatorMain_SystemTick(void)
{
#ifdef _SupportPhoenixConsole
  unsigned short TestCaptureData = 0 ;
  static unsigned short SinTableIndex = 1;
  static unsigned char A = 2;
  static unsigned char B = 4;  
  unsigned char x ;
  static unsigned char CaptureTime = 0 ;  
  static unsigned short OldRPM = 0 ;
#endif
  //----------------------------------------------------------------------------
  LCM_DelayTime() ;
  //----------------------------------------------------------------------------
  JHTLCBComm_SystemTime() ;
  if( LCBSimulator_ConsolePowerControl() == 1 )
      {
      JHTLCBComm_Timeout() ;
      }
  LCBSimulator_SystemTime() ;   
  //----------------------------------------------------------------------------
#ifdef _PRODUCT_BOARD  
  ADS1100_SystemTime() ;
#endif
  //----------------------------------------------------------------------------
  if( MainSystemTime != 0 )
      MainSystemTime -= 1 ;
  LCBSystemTime += 1 ;

//
#ifdef _SupportPhoenixConsole
  if( LCBParameter.LCBSimulatorType == _Rower )
      {
      if( RISLCBParameter.RPM != 0 )
          {
          if( OldRPM != RISLCBParameter.RPM && SinTableIndex == 1 )
              {
              OldRPM = RISLCBParameter.RPM ;
              x = (unsigned char)(RISLCBParameter.RPM / 10) ;
              if( x > 7 )
                  x = 7 ;
              A = RowerSpeedTable[x][0] ;
              B = RowerSpeedTable[x][1] ;
              }
          CaptureTime += 1 ;
          if( CaptureTime >= B )
              {
              CaptureTime = 0 ;
              TestCaptureData = 6000 - SinTable[SinTableIndex] * 4 ;
              RowerWatts_everypole(TestCaptureData) ;          
              SinTableIndex += A ;
              if( SinTableIndex > 999 )
                  SinTableIndex = 1 ;
              }
          }
      else
          {
          if( OldRPM != RISLCBParameter.RPM )
              {
              // for Rower
              RowerWatts_ClearData() ;
              RowerWatts_Initial();
              OldRPM = RISLCBParameter.RPM ;
              }
          }
      RowerWatts_1ms_Int() ;
      }
  //----------------------------------------------------------------------------
  // 20170809 Check HeartRate Status for Rower and S-Drive
  if( LCBParameter.LCBSimulatorType != _IndoorCycle )
      {
      if( RISLCBParameter.HeartRate != 0 )//&& RISLCBParameter.HeartRate <= 40 )
          {
          RIS_LCBStatus.bit.HeartRateStatus = 1 ;
          }
      else
          {
          RIS_LCBStatus.bit.HeartRateStatus = 0 ;
          }  
      }
  else
      RIS_LCBStatus.bit.HeartRateStatus = 0 ;
  //----------------------------------------------------------------------------
#endif
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short SimulatorMain_CalculatorTimeTick( unsigned short last_TimeTick )
{
  unsigned short TempTick ;
  if( LCBSystemTime < last_TimeTick )
      TempTick = ( 0xFFFF - last_TimeTick ) + LCBSystemTime ;
  else  
      TempTick = LCBSystemTime - last_TimeTick ;
  //
  return TempTick ;
}


//------------------------------------------------------------------------------
/**
  * @brief  This function handles simulator main Handler.
  * @param  None
  * @retval None
  */
#if __ICCARM__
void Simulator_Main(void)
#elif __GNUC__
int main(void)
#endif
{
  static volatile unsigned char ShowScreen ;
  static unsigned char Mode ;
  static unsigned short Type ;
  static unsigned short TypeOld ;
  static unsigned char SetLCBType ;  
  unsigned char Btn ;
  unsigned char BtnActive ;
  unsigned char StringNo ;
  static unsigned short ScrollTime ;
  static unsigned short BtnPressTime ;
  unsigned char *ScrPtr ;
#ifdef  _PRODUCT_BOARD  
  unsigned char PX ;
  unsigned short ShowCurrent ;
#endif  
//20181114  
#ifdef  _SupportSerialNumberCommand
  unsigned char count ;
#endif
  
#ifdef _V3_Update // 20210526
  unsigned char i ;
                             //12345678901234
  unsigned char RunScreen[15]="              " ;
#endif
  //
  SimulatorMain_Initial() ;
  LCBSimulator_Initial() ;
  // Show Version
  // 20190416 Add SN,OC 0144&0146
  //          Add C7 Type
  // 20190507 Diasble 0x442 error coce 
  // 20200115 Add log not support command.
  // 20200302 Add Performance TM ,Endurance TM 0xC7
  //          Add Performance Plus TM 0xC6
  //          Add Lifestyle Series Frames �V CM, EP  
  // 20200319 Add JIS LCB Type 0x0B,0x0C,0x0E,0x12,0x15,0xE0,0xE1
  // 20200513 Modify Sikp Error Function Bug.
  // 20200528 Add Scale of adjustment,
  // 20200707 Modify Athena default NunberOfParameter value
  // 20200708 Add Delta Type C8
  // 20200709 Modify TM Incline min value from 3276 chang to 276
  // 20200731 Modify LCB reponse version for serial number support.
  // 20210223 Add F0 MCB Type F0
  // 20210315 Modify JIS Type OC issue
  // 20210317 Add Command 0x50+0x80,0x50+0x81
  // 20210322 Add VAVE MCB A1/A2/A3/F1
  // 20210413 Add Upper Body Cycle LCB(UBCM&UBCP)
  // 20210415 Fix UBCP Bug
  // 20210526 Updata screen
  // 20210602 Add ITC
  // 20210623 Modify Rep Data process
  // 20210628 Add Auto Rep Mode
  // 20210705 Modify LCB Type
  // 20210819 Modify Show string data length 20->40
  // 20211201 Add JIS DC MCB type 04
  // 20220119 Modyfy Rower show LCB type 00->02
  // 20220216 Add Support LCB type 03 for Indoor Cycle FC28/29 
  //          Modify Rower and S-Drive LCB version from 0.1 -> 1.2, support serial number
  // 20230314 Add Support LCB Type 1D for Indooe Cycle IRC70
  //----------------------------------------------------------------------------------------
  // 20230412 Change IAR version from V5.41 to V9.20.4 , USB update incompatible IAR 5.41
  // 20230522 Add Support Global DC MCB Type D0 and D1
  // 20230526 Fix Bootloader USB unplugin bug.
  // 20250601 Add support impulse climbmill LCB type:0x1E
  // 20250701 Modify 04A0 Timeout from 3 sec change to 7 sec.
  // 20250731 Add JHT Teradmill Non Used parameter can setting
  // 20260113 Add Universal and GTM LCB
  // 20260126 Add 0x1F,0x20,0x21,0xC9,0xF2 New LCB
#ifdef  _PRODUCT_V2
  LCMDisplay_show_string(0,0,"LCB Simulator V3") ; 
  LCMDisplay_show_string(0,1,"Ver:            ") ;
#if __ICCARM__
  LCMDisplay_show_string(4,1,(unsigned char *)&User_Version[0]) ;
#elif __GNUC__
  LCMDisplay_show_string(4,1,(unsigned char *)&UserInfo.Version[0]) ;
#endif
#else  
  #ifdef  _SupportPhoenixConsole  
    //----------------------------------------------------------------------------
    // 2017
    // 0731 Add Indoor Cycle Erp Mode Testing function
    // 0802 Add Rower
    // 0809 Add Rower Check BPM value, If BPM != 0 then to setting status.b3 = 1
    //                                 else status.b3 = 0 
    // 0912 Add S-Drive
    //
    // 1115 Modify Indoor Cycle sensor data 
    // 2018
    // 0111 Modify Sensor data
    // 20180402 Modify TM ESTOP Process flowchart
    // 20180430 Modify LCB1 02AB issue
    // 20180503 Add Check LCB1 and Machine tyep is stepper
    // 20180507 Add TM 0x140 error
    // 20180509 Add TM Provided Error Code  
    // 20190808 Add CMD 0x50 for Version
    //              Modify LCB Version 
    // 20210223 Add F0 MCB Type
    // 20210315 Modify JIS Type OC issue
    // 20210317 Add Command 0x50+0x80,0x50+0x81
    // 20210322 Add VAVE MCB A1/A2/A3/F1
    // 20210413 Add Upper Body Cycle LCB(UBCM&UBCP) 
    // 20210415 Fix UBCP Bug
    // 20210526 Updata screen
    // 20210602 Add ITC  
    // 20210623 Modify Rep Data process
    // 20210628 Add Auto Rep Mode
    // 20210705 Modify LCB Type
    // 20210819 Modify Show string data length 20->40
    // 20211201 Add JIS DC MCB type 04
    // 20220119 Modyfy Rower show LCB type 00->02
    // 20220216 Add Support LCB type 03 for Indoor Cycle FC28/29 
    //          Modify Rower and S-Drive LCB version from 0.1 -> 1.2, support serial number
    //
    // 20230314 Add Support LCB Type 1D for Indooe Cycle IRC70
    //----------------------------------------------------------------------------------------
    // 20230412 Change IAR version from V5.41 to V9.20.4 , USB update incompatible IAR 5.41
    // 20230522 Add Support Global DC MCB Type D0 and D1
    // 20230526 Fix Bootloader USB unplugin bug.
    // 20250601 Add support impulse climbmill LCB type:0x1E
    // 20250701 Modify 04A0 Timeout from 3 sec change to 7 sec.
    // 20250731 Add JHT Teradmill Non Used parameter can setting
    // 20260113 Add Universal and GTM LCB
    // 20260126 Add 0x1F,0x20,0x21,0xC9,0xF2 New LCB
                               //12345678901234561234567890123456
    LCMDisplay_show_string(0,0,"LCB Simulator V2Ver:20260126-01 ") ;
  #else  
    //----------------------------------------------------------------------------
                              //12345678901234561234567890123456
    LCMDisplay_show_string(0,0,"LCB Simulator V2Ver:20260126-02 ") ;  
  #endif  
#endif  
  MainSystemTime = 2000 ;
  //----------------------------------------------------------------------------
  // Default Go to Run Mode and First time Show is Active
  ShowScreen = 1 ;
  Mode = _RunMode ;
  //----------------------------------------------------------------------------  
  // Deafult Type Select parameter
  SetLCBType = LCBParameter.LCBSimulatorType ;
  for( Type = 0 ; Type < _MachineTypeSelectLimit ; Type++ )
      {
      if( MType[Type].Type == SetLCBType )
          break ;
      }
  TypeOld = Type ;
  //----------------------------------------------------------------------------  
  //
  for(;;)
      {     
      if( MainSystemTime == 0 )
          {
          switch( Mode )
              {
              default               :  
                                    Mode = _RunMode ;                
              case  _RunMode        :      
                                    //Modify By Kunlung 20180503
                                    //Show = LCBSimulator_Process(Show,(unsigned char*)&MType[TypeOld].Screen[0]) ; 
#ifndef _V3_Update // 20210526
                                    ScrPtr = (unsigned char*)&MType[TypeOld].Screen[0] ;
#else
                                    for( i= 0; i < 14; i++ )
                                        RunScreen[i] = MType[TypeOld].ModelName[i] ;
                                    // change type data
                                    if( MType[TypeOld].Type == _Rower )
                                        {
                                        RunScreen[12] = '0' ;
                                        RunScreen[13] = '2' ;
                                        }
                                    else if( MType[TypeOld].Type == _JISECB_I )
                                        {
                                        RunScreen[12] = '0' ;
                                        RunScreen[13] = 'B' ;
                                        }
                                    else if( MType[TypeOld].Type == _JISDCLCB ) // Add 20211201
                                        {
                                        RunScreen[12] = '0' ;
                                        RunScreen[13] = '4' ;
                                        }
                                    // 20220216 add cxp update
                                    //else
                                    else if( MType[TypeOld].Type == _CxpUpdate )
                                        {
                                        RunScreen[12] = '0' ;
                                        RunScreen[13] = '3' ;
                                        }
                                    //--------------------------------------------
                                    else
                                        {
                                        if( (MType[TypeOld].Type/0x10) < 10 )
                                            RunScreen[12] = '0'+ MType[TypeOld].Type/0x10 ;
                                        else
                                            RunScreen[12] = 'A'+ ((MType[TypeOld].Type/0x10)-0x0a) ;
                                        if( (MType[TypeOld].Type%0x10) < 10 )
                                            RunScreen[13] = '0'+ MType[TypeOld].Type%0x10 ;
                                        else
                                            RunScreen[13] = 'A'+ ((MType[TypeOld].Type%0x10)-0x0a) ;
                                        }
                                    ScrPtr = &RunScreen[0] ;
#endif                                    
                                    if( TypeOld == 0 )
                                        {
                                        if( LCBParameter.MachineType == _MBikeEP_ )
                                            ScrPtr = (unsigned char*)&MTypeLCB1_0[0] ;
                                        else if( LCBParameter.MachineType == _MSetpper_ )
                                            ScrPtr = (unsigned char*)&MTypeLCB1_1[0] ;
                                        }
                                    ShowScreen = LCBSimulator_Process(ShowScreen,ScrPtr);
                                    //
                                    if( ShowScreen == 2 )
                                        {
                                        Mode = _WaitButtonFree ;
                                        ShowScreen = 1 ;
                                        #ifdef  _PRODUCT_V2
                                        #ifndef _V3_Update // 20210526                                          
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"LCB Simulator V3TYPE:           ") ; 
                                        #else
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"                                ") ;                                         
                                        #endif
                                        #else
                                        #ifndef _V3_Update // 20210526
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"LCB Simulator V2TYPE:           ") ;
                                        #else
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"                                ") ;                                         
                                        #endif
                                        #endif 
                                        }   
                                    // Check Erp Mode
                                    if( LCBSimulator_GetErPStatus() == 1 )
                                        {
                                        Mode = _ErPActionMode ;
#ifdef  _PRODUCT_BOARD                                        
                                        ShowCurrent = 0 ;
                                        PX = 0 ;
#endif
                                        ShowScreen = 0 ;
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"     ErP Mode                   ") ;
                                        }
                                    // Check RIS cut off battery
                                    if( CommControlFlag.Bits.RIS_LCBMode == 1 && LCBSystemProcessStatus.B.sRISIntoVbatPowerOFFMode == 1)
                                        {
                                        Mode = _RISBatteryOFF ;
                                        ShowScreen = 1 ;
                                        }
                                    //  
                                    break ;
              case  _RISBatteryOFF  :
                                    //
                                    if( ShowScreen == 1 )
                                        {
                                        ShowScreen = 0 ;
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"The Battery is  cut off.........") ;
                                        BtnPressTime = LCBSystemTime ;
                                        }  
                                    //
                                    if( LCBSystemProcessStatus.B.sRISIntoVbatPowerOFFMode == 0 )
                                        {
                                        Mode = _RunMode ;
                                        ShowScreen = 1 ;
                                        }
                                    //
                                    _iButton(Btn) ;
                                    if( Btn == 0 )
                                        {
                                        if( SimulatorMain_CalculatorTimeTick(BtnPressTime) > 1000 )
                                            {
                                            BtnPressTime = LCBSystemTime ;
                                            LCBSystemProcessStatus.B.sRISVbatON = 1 ;
                                            }
                                        }
                                    else
                                        BtnPressTime = LCBSystemTime ;
                                    break ;
              case  _WaitButtonFree :
                                    _iButton(Btn) ;
                                    if( Btn != 0 )
                                        {
                                        Mode = _SelectLCBType ; 
                                        ShowScreen = 1 ;
                                        }
                                    break ;
              case  _SelectLCBType  : // Set LCB Type
                                    if( ShowScreen == 1 )
                                        {
                                        //--------------------------------------------------------------  
                                        #ifdef  _PRODUCT_V2
                                        #ifndef _V3_Update // 20210526                                          
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"LCB Simulator V3TYPE:           ") ; 
                                        #else
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"                                ") ;                                         
                                        #endif
                                        #else
                                        #ifndef _V3_Update // 20210526 
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"LCB Simulator V2TYPE:           ") ;
                                        #else
                                                                  //12345678901234561234567890123456
                                        LCMDisplay_show_string(0,0,"                                ") ;                                         
                                        #endif
                                        #endif                                          
                                        //
                                        ShowScreen = 2 ;
                                        SetLCBType = LCBParameter.LCBSimulatorType ;
                                        for( Type = 0 ; Type < _MachineTypeSelectLimit ; Type++ )
                                            {
                                            if( MType[Type].Type == SetLCBType )
                                                break ;
                                            }
                                        TypeOld = Type ;
                                        StringNo = 0 ;
                                        }
                                    else if( ShowScreen == 2 )
                                        {
                                        // Protect Array Memory Max Size
                                        if( TypeOld >= _MachineTypeSelectLimit )
                                            {
                                            TypeOld = 0 ; 
                                            Type = 0 ;
                                            SetLCBType = MType[TypeOld].Type ;
                                            }
                                        // Show Screen String to LCM
                                        //LCMDisplay_show_string(5,1,(unsigned char*)&MType[TypeOld].Screen[0]) ;
#ifndef _V3_Update // 20210526
                                        StringNo = LCMDisplay_show_scrollingstring(5,1,11,(unsigned char*)&MType[TypeOld].Screen[0],StringNo) ;
#else
                                        StringNo = LCMDisplay_show_scrollingstring(0,0,16,(unsigned char*)&MType[TypeOld].ModelName[0],StringNo) ;
                                        StringNo = LCMDisplay_show_scrollingstring(0,1,16,(unsigned char*)&MType[TypeOld].LcbType[0],StringNo) ;
#endif                                        
                                        if( StringNo == 0xFF )
                                            ShowScreen = 0 ;
                                        else
                                            ShowScreen = 3 ;
                                        }
                                    else if( ShowScreen == 3 )
                                        {                                        
                                        if( SimulatorMain_CalculatorTimeTick(ScrollTime) > 300 )
                                            {
                                            ScrollTime = LCBSystemTime ;
                                            ShowScreen = 2 ;
                                            }
                                        }
                                    // Process Mode Select VR
                                    Encoder_GetValue(0,(_MachineTypeSelectLimit-1),&Type,_RING,1) ;
                                    if( Type != TypeOld )
                                        {
                                        TypeOld = Type ;                                        
                                        SetLCBType = MType[TypeOld].Type ; 
                                        ShowScreen = 2 ;
                                        StringNo = 0 ;
                                        }
                                    // Process Push Button
                                    _iButton(Btn) ;
                                    if( Btn == 0 )
                                        BtnActive = 1 ;    
                                    else
                                        {
                                        if( BtnActive == 1 )
                                            {
                                            ShowScreen = 1 ; 
                                            Mode = _RunMode ;                                            
                                            if( LCBParameter.LCBSimulatorType != SetLCBType )
                                                {                                 
                                                LCBParameter.ClimbmillStatus.Full = 0 ; 
                                                LCBParameter.ClimbmillExtendStatus.Full = 0 ; 
                                                LCBSimulator_SetConsolePowerStatus(0) ;
                                                LCBSimulator_SetsSaveEEPROMStatus(1) ;
                                                oConsolePower(_OFF);
                                                #ifdef _SupportPhoenixConsole // Indoor Cycle VBAT_ON
                                                oBATON(_BATOFF); // Add 20220216
                                                #endif
                                                //Modify 20210413
                                                //if( SetLCBType == _IndoorCycle || SetLCBType == _Rower || SetLCBType == _SDrive )
                                                //Modiyf 20220216 Add Cxp Update
                                                //if( SetLCBType == _IndoorCycle || SetLCBType == _Rower || SetLCBType == _SDrive || SetLCBType == _UBCM || SetLCBType == _UBCP)
                                                if( SetLCBType == _IndoorCycle || SetLCBType == _Rower || SetLCBType == _SDrive || SetLCBType == _UBCM || SetLCBType == _UBCP || SetLCBType == _CxpUpdate || SetLCBType == _Universal || SetLCBType == _GTM)
                                                    {
                                                    RISLCBParameter.ManchineType = RIS_MType[SetLCBType] ;
                                                    if( CommControlFlag.Bits.RIS_LCBMode == 0 )
                                                        {
                                                        JHTLCBComm_RIS_HW_Initial() ;
                                                        ErrorCodeStatus.Full = 0 ;
                                                        JHTLCBComm_ClearAllErrorMessage(0) ;
                                                        }
                                                    }
                                                else
                                                    {
                                                    //if( LCBParameter.LCBSimulatorType == _IndoorCycle || LCBParameter.LCBSimulatorType == _Rower || LCBParameter.LCBSimulatorType == _SDrive)
                                                    if( CommControlFlag.Bits.RIS_LCBMode == 1 )
                                                        {
                                                        JHTLCBComm_HW_Initial() ;
                                                        ErrorCodeStatus.Full = 0 ;
                                                        JHTLCBComm_ClearAllErrorMessage(0) ;
                                                        }
                                                    }
                                                LCBParameter.LCBSimulatorType = SetLCBType ;                                                
                                                EE93CXX_WriteDataToEeprom(_EEMachineType,1,&LCBParameter.LCBSimulatorType) ; 
                                                LCBSimulator_SetDelayConsolePowerOnTimeLimit(2000) ;
                                                //Add 20210413
                                                LCBSimulator_Initial() ;
                                                //-------------------------------
                                                }
                                            }
                                        BtnActive = 0 ;
                                        }                
                                    break ;
              case  _ErPActionMode  :
                                    if( ShowScreen != 0 )
                                        {
                                        ShowScreen = 0 ;
                                                                  //1234567890123456
                                        LCMDisplay_show_string(0,0,"     ErP Mode   ") ;
                                        }
                                    //--------------------------------------------------------------
#ifdef  _PRODUCT_BOARD
                                    // Show Current                                    
                                    if( ShowCurrent != ADS1100_GetCurrentValue() )
                                        {
                                        ShowCurrent = ADS1100_GetCurrentValue() ;
                                        LCMDisplay_show_one_char(PX,1,' ') ; // Clear 'A' 
                                        PX = LCMDisplay_show_word_bcd(0,1,ShowCurrent,9) ;
                                        LCMDisplay_show_one_char(PX,1,'A') ;
                                        ShowScreen = 1 ;
                                        }
#endif                                    
                                    //---------------------------------------------------------------
                                    // Check Release Erp Mode
                                    if( LCBSimulator_GetErPStatus() == 0 )
                                        {
                                        ShowScreen = 1 ;
                                        Mode = _RunMode ;
                                        }    
                                    //---------------------------------------------------------------
                                    break ;                                                
              }

          }
      // Communication Receiver Decoder Process
      JHTLCBComm_RxProcess() ;
#ifdef _PRODUCT_BOARD      
      ADC_Process() ;
      ADS1100_Process() ;
#endif
      // Check Save Data To EEPROM
      if( LCBSimulator_GetSaveEEPROMStatus() == 1 )
          {          
          EE93CXX_WriteDataToEeprom(_EEClimbmillSetting,1,&LCBParameter.ClimbmillStatus.Full) ;
          LCBSimulator_SetsSaveEEPROMStatus(0) ;
          }
      //================================================================================================
      // 20181114    
#ifdef  _SupportSerialNumberCommand      
      if( LCBSimulator_GetSetSerialNumber() == 1 )
          {
          EE93CXX_WriteDataToEeprom(_EESerailNumberLength,1,&MachineSerialNumberLength) ;
          if( MachineSerialNumberLength == 0 || MachineSerialNumberLength >_MaxLengthOfSerialNumber )
              {
              for( count = 0; count < _MaxLengthOfSerialNumber ; count++ )
                  MachineSerialNumber[count] = 0 ;                
              EE93CXX_WriteDataToEeprom(_EESerialNumber,_MaxLengthOfSerialNumber,&MachineSerialNumber[0]) ;
              }
          else
              {                 
              EE93CXX_WriteDataToEeprom(_EESerialNumber,MachineSerialNumberLength,&MachineSerialNumber[0]) ;
              }
          LCBSimulator_SetSerialNumber(0) ;
          }
#endif     
      //==================================================================================================
#if 0      
      //------------------------------------------------------------------------
      // For Testing Hardware
      static unsigned short TestCount ;
      static unsigned short TestCountOld ;
      static unsigned char Btn ;      
      TestCount = TIM_GetCounter(TIM3) / 4 ;
      if( TestCountOld != TestCount )
          {
          TestCountOld = TestCount ;
          LCMDisplay_show_word(0,1,TestCountOld) ;
          }
      _iButton(Btn) ;
      if( Btn == 0 )
          LCMDisplay_show_one_char(15,1,0xFF) ;
      else
          LCMDisplay_show_one_char(15,1,0x20) ;
      //------------------------------------------------------------------------
#endif      
      }
  //----------------------------------------------------------------------------
}






























