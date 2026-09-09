/**
  ******************************************************************************
  * @file    
  * @author  
  * @version V0.0.1
  * @date    
  * @brief   
  *          
  *          
  ******************************************************************************
  * 
  */

/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#include  "JHTLCBComm.h"
#include  "JHTCOMMAND.h"
#include  "LCBSimulator.h"
#include  "EE93CXX.h"
#include  "Screen.h"
#include  "LCM.h"
#include  "LCMDisplay.h"
#include  "Encoder.h"
#include  "PinDefine.h"
#include  "Errorcode.h"
//
#include  "RowerWatts.h"

/* External variables -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/


//
/* Private define ------------------------------------------------------------*/
#define     _TM_Incline_Min_            276
#define     _TM_Incline_Max_            (32767-_TM_Incline_Min_)

#define     _AT_Incline_Min_            3276
#define     _AT_Incline_Max_            (32767-_TM_Incline_Min_)

#ifndef _V3_Update // 20210526
#define     _TypeScreenBarX             2 
#define     _TypeScreenBarY             0
#define     _TypeScreenBarLeagth        12
#else
#define     _TypeScreenBarX             1 
#define     _TypeScreenBarY             0
#define     _TypeScreenBarLeagth        14
#endif


#define     _ECB_Min_                   0
#define     _ECB_Max_                   500

#define     _JIS_Incline_Min_            20
#define     _JIS_Incline_Max_            (3276-_JIS_Incline_Min_)

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


volatile LCBSimulatorProcessStatusStatus    LCBSystemProcessStatus ;

unsigned short InclineAdject ;
unsigned short InclineTarget ;
unsigned short InclineTrackingTime ;
unsigned short JISECBTarget ;
unsigned short JISECBTrackingTime ;
unsigned short SafetyKeyPressTime ;
unsigned short SafetyKeyReleaseTime ;
unsigned short MotorTrackingTime ;
unsigned short MotorRPMAdject ;
unsigned short MotorRPMTarget ;
unsigned short TrackTime ;
unsigned short TrackTime1 ;
volatile unsigned short SimulatorSystemTime ;
LCBSystemControlDataStatus       LCBParameter ;
unsigned long ConsoleWaitPowerTime ;
unsigned long DelayConsolePowerOnTimeLimit ;
unsigned long DelayConsolePowerOffTimeLimit ;
unsigned long ConsolePowerCycleCount ;
unsigned short OldStepRPM ;

unsigned char   SetMode = 0 ;
unsigned short  Select = 0 ;
unsigned short  OldSelect = 15 ;
unsigned char   SelectLimit = 8 ;
unsigned short  SetData = 0 ;
unsigned short  OldSetData = 1 ;
// For 2017 RIS LCB
RISLCBSystemData RISLCBParameter ;
unsigned char   RIS_MasterStatus ;
RISLCBSensorStatusDataStruct  RIS_SensorFunctionStatus ;
unsigned long RISConsoleWaitPowerTime ;
unsigned short VbatONPressTime ;
unsigned short WaitVbatONReleaseTime ;
unsigned long RISBatteyOFFTime ;
unsigned short ErrorCodeList[_ErrorListSize] = { 0x0000, 0x14A, 0x248, 0x250, 0x251, 0x256, 0x301, 0x306, 0x344,  0x2A0, 0x01AC, 0x2AF, 0x4A0 } ;   // 20170727
const PowerTable  PowerTableParameters = {
   0x55555501,12.442201649523,27.6337176705794,-102757.205865035,472.333408895571,-783467.24100673,3749792403.93317,
   1.80763885652257,231.297943111327,-929180.055120685,705346938.048753,-12760328567852.7,9.75,13230,1513832255
} ;
// Anthena
const unsigned long ResistanceTable[26] = 
{//
    0, // L0
    700,  1230, 1440, 1593, 1719, 1829, 1927, 2017, 2101, 2181,  // L1  ~ L10
    2256, 2329, 2399, 2467, 2534, 2599, 2664, 2728, 2791, 2855,  // L11 ~ L20
    2918, 2983, 3048, 3115, 3183,                                // L21 ~ L25
};// L0 ~ L25, unit: 1/16-step

// 20230314 add ICR70
const unsigned short ICR70ECBcount[25] = 
{   18000,                                                      // L1
    15420,  13470,  11900,  10640,  9600, 8740, 7980, 7320,     // L2 ~ L9
    6740,   6210,   5720,   5270,   4830, 4400, 3975, 3555,     // L10 ~ L17
    3135,   2715,   2295,   1875,   1460, 1050, 650,  250	// L18 ~ L25
}; // L1-L25



//------------------------------------------------------------------------------
// Add 20181114
#ifdef  _SupportSerialNumberCommand 
unsigned char MachineSerialNumberLength ;
unsigned char MachineSerialNumber[_MaxLengthOfSerialNumber+1] ;
// Add 20210602
unsigned char ITC_ConsoleSN[_ITCMaxLengthOfSerialNumber+1] ;
unsigned char ITC_FrameSN[_ITCMaxLengthOfSerialNumber+1] ;
#endif




/* Private function prototypes -----------------------------------------------*/
unsigned short LCBSimulator_CalculatorTimeTick( unsigned short last_TimeTick );
void LCBSimulator_TreadMillESTOPProcess(void) ;
void LCBSimulator_TreadMillJISESTOPProcess(void) ;
void LCBSimulator_AscentTrainerProcess( unsigned char IncluneType );
void LCBSimulator_JISA30ECBProcess( void ) ;
void LCBSimulator_JISInclineProcess( void ) ;
void LCBSimulator_AthenaProcess(void) ;
void LCBSimulator_ITCProcess(void) ;
void LCBSimulator_EPBikeProcess(void);
void LCBSimulator_RISProcess(unsigned char Button,Parameters *ptr) ;
unsigned char LCBSimulator_ParameterProcess(unsigned char MenuLimit ,unsigned char Button, Parameters *ptr ,unsigned char ShowStatus);

//
/* Functions -----------------------------------------------------------------*/
void LCBSimulator_Initial(void)
{
  unsigned char TempEEdata ;
  LCBSystemProcessStatus.All = 0 ;
  LCBParameter.InclineType = 1 ;
  //------------------------------------------------------------------------------
  // Add 20181114
#ifdef  _SupportSerialNumberCommand 
  for( TempEEdata = 0; TempEEdata < _MaxLengthOfSerialNumber ; TempEEdata++ )
      MachineSerialNumber[TempEEdata] = 0 ;  
  EE93CXX_ReadDataFromEeprom(_EESerailNumberLength,1,&MachineSerialNumberLength) ;
  if( MachineSerialNumberLength > _MaxLengthOfSerialNumber )
      {
      //------------------------------------------------------------------------------------------
      // if Error to Clear Memory  
      MachineSerialNumberLength = 0 ;
      EE93CXX_WriteDataToEeprom(_EESerailNumberLength,1,&MachineSerialNumberLength) ;
      EE93CXX_WriteDataToEeprom(_EESerialNumber,_MaxLengthOfSerialNumber,&MachineSerialNumber[0]) ;
      //------------------------------------------------------------------------------------------
      }
  else
      {
      EE93CXX_ReadDataFromEeprom(_EESerialNumber,MachineSerialNumberLength,&MachineSerialNumber[0]) ;
      }
#endif
  //
  EE93CXX_ReadDataFromEeprom(_EEMachineType,1,&LCBParameter.LCBSimulatorType) ;
  EE93CXX_ReadDataFromEeprom(_EEClimbmillSetting,1,&LCBParameter.ClimbmillStatus.Full) ;
  EE93CXX_ReadDataFromEeprom(_EEPowered,1,&TempEEdata) ;
  if( TempEEdata > 1 )
      {
      TempEEdata = 1 ;
      EE93CXX_WriteDataToEeprom(_EEPowered,1,&TempEEdata) ;   
      }
  
  LCBSystemProcessStatus.B.sDCPlugin = 1 ;
  RIS_LCBStatus.bit.ACPluginStatus = 0 ;  // Add 20210415
  switch(LCBParameter.LCBSimulatorType) 
      {
      case _Climbmill_    :  
      case _ClimbmillEN_  :
      case _ClimbmillR_   :
      case _IMPULSE_LCB   : // 20250526 Add IMPULSE
      case _CMLCB_2026    : // 20260126 
                          break ;
      case _LCB3_         :
                          LCBParameter.InclineType = 2 ;
      case _LCB1_         :
      case _LCBA_         :
      case _LCBA_M_       :
      case _LCB1x_        :      
      case _BKLCBA_2026   : // 20260126
                          if( TempEEdata == 0x00 ) // != 00
                              {
                              LCBSystemProcessStatus.B.sDCPlugin = 0 ;
                              }
                          break ;
      case _ATLCB2_2026   : //20260126
      case _LCB2_         :        
      case _DCI_          :      
      case _TopTek_       : 
      case _Delta_I       :    
      case _Delta_H       : 
      case _Delta_I1      :
      case _Delta_I2      :  
      case _Delta_I3      :
      case _Delta_I4      :
      case _Liteon        :         
      case _LiteonJHT     ://Add 20210223
      case _VAVE_A1       ://Add 20210322  
      case _VAVE_A2       ://Add 20210322 
      case _VAVE_A3       ://Add 20210322   
      case _VAVE_F1       ://Add 20210322 
      case _PS_PMSM       ://20260126
      case _PP_PMSM       ://20260126
                          break ;
      case _Rower         :
                          RISLCBParameter.ManchineType = _RIS_Rower ;        
                          break ;        
      case _SDrive        :
                          RISLCBParameter.ManchineType = _RIS_SDrive ;        
                          break ;        
      case _IndoorCycle   :
      // 20251215
      case  _Universal    :
      case  _GTM          :
      //         
                          RISLCBParameter.ManchineType = _RIS_IndoorCycle ;        
                          break ;                 
//------------------------------------------------------------------------------                          
// Add 20210413 Upper Body Cycle (UBCM & UBCP)
      case _UBCP          :      
                          LCBParameter.Calculation.NunberOfParameter = 21 ;
                          LCBParameter.Erg.NunberOfParameter = 7 ;   
                          RIS_LCBStatus.bit.ACPluginStatus = 1 ;
                          if( RISLCBParameter.Direction == 0 || RISLCBParameter.Direction < 2 )
                              RISLCBParameter.Direction = 1 ;
                          RISLCBParameter.ManchineType = _RIS_UpBodyCycle ;
                          break ;
      case _UBCM          :
                          RIS_LCBStatus.bit.ACPluginStatus = 0 ;
                          if( RISLCBParameter.Direction == 0 || RISLCBParameter.Direction < 2 )
                              RISLCBParameter.Direction = 1 ;
                          RISLCBParameter.ManchineType = _RIS_UpBodyCycle ;
                          break ;
// Add 20220216 Cxp Update                          
      case _CxpUpdate     :
                          RISLCBParameter.ManchineType = _RIS_IndoorCycle ;        
                          break ;                           
//------------------------------------------------------------------------------                          
      case _JISECB_I      :
                          LCBParameter.ECBStatus.Full = 0 ;
                          break ;
      case _JISEM         :
      case _JISDCTM       :
      case _JISECB        :
      case _JISEM_I       :
      case _JISACTM110    :
      case _JISACTM220    :
      case _JISDCLCB      :// add 20211201 
      case _JISGMCB110    :// add 20230522
      case _JISGMCB220    :// add 20230522
                          break ;
      case _Athena        :  
                          // 20200707
                          LCBParameter.Calculation.NunberOfParameter = 21 ;
                          LCBParameter.Erg.NunberOfParameter = 7 ;
                          //
                          break ;
//------------------------------------------------------------------------------
      case  _ITC          :
                          LCBParameter.LoadCellOffset = 0 ;
                          LCBParameter.LoadCellScale = 0 ;
                          LCBParameter.CableTension =6620 ;      // default 6620
                          LCBParameter.LoadCellADC = 0 ;
                          LCBParameter.LoadCellEncoder = 0;     
                          LCBParameter.RepData.Member.mass_method2_dag = 1000 ;
                          LCBParameter.RepData.Member.mass_method1_dag = 1000 ;
                          LCBParameter.LoadCellConfig.Cfg.Reserved = 0 ;
                          LCBParameter.LoadCellConfig.Cfg.NoMotionTimeout = 60 ; //sec.
                          LCBParameter.LoadCellConfig.Cfg.SmartRepAccTrigger = 8000 ;
                          LCBParameter.LoadCellConfig.Cfg.mmPer1000count = 2283 ;
                          LCBParameter.LoadCellConfig.Cfg.BottomThreshold_mm = 4 ;
                          LCBParameter.LoadCellConfig.Cfg.TriggerHeight_mm = 9 ;
                          LCBParameter.LoadCellConfig.Cfg.StrokeDown_mm = 80 ;
                          LCBParameter.LoadCellConfig.Cfg.StrokeUp_mm = 100 ;
                          LCBParameter.LoadCellConfig.Cfg.Direction = 1 ;
                          LCBParameter.AutoRepTime = 0 ;
                          break ;
//------------------------------------------------------------------------------    
// 20221130 Add Support ICR70
      case  _JISICR70LCB  :
                          LCBParameter.ResistanceLevel = 1 ;
                          LCBParameter.ECBCount = ICR70ECBcount[(LCBParameter.ResistanceLevel-1)] ;
                          LCBParameter.ECBPosition = LCBParameter.ECBCount ;
                          LCBSystemProcessStatus.B.sDCPlugin = 1 ;
                          break ;
//------------------------------------------------------------------------------                             
      default             :
                          LCBParameter.LCBSimulatorType = _LCBA_ ;
                          EE93CXX_WriteDataToEeprom(_EEMachineType,1,&LCBParameter.LCBSimulatorType) ;
                          LCBParameter.ClimbmillStatus.Full = 0 ;
                          EE93CXX_WriteDataToEeprom(_EEClimbmillSetting,1,&LCBParameter.ClimbmillStatus.Full) ;
                          break ;
      }
     
  LCBParameter.StepRPM = 60 ;
  //
  SpecControlStatus.bit.NoUser = 0 ; //Add 20250731 No user
  LCBParameter.TreadmillUseStatus = 0xFF ;
  //
  SimulatorSystemTime = 0  ;  
  InclineTarget = 0 ;
  InclineTrackingTime = 0 ;
  SafetyKeyPressTime = 0 ;
  SafetyKeyReleaseTime = 0 ;
  ConsoleWaitPowerTime = 0 ;
  DelayConsolePowerOnTimeLimit = 2000 ;
  LCBParameter.PowerOffTime = 10 ;
  DelayConsolePowerOffTimeLimit = (unsigned long)LCBParameter.PowerOffTime*60000 ;
  ConsolePowerCycleCount = 0 ;  
  LCBParameter.ClimbmillBrakeONPressureValue = _DEFAULT_BRAKE - 50 ; // add for Climbmil EN
  LCBParameter.ClimbmillBrakeOFFPressureValue = _DEFAULT_BRAKE ; // add for Climbmil EN
  //-------------------------------------------------------------------------------------------------
  // 2017 RIS LCB
  RISLCBParameter.BatteryVoltage = 400 ;
  RISLCBParameter.Torque = 0 ;
  RISLCBParameter.RPM = 0 ;
  EE93CXX_ReadDataFromEeprom(_EER_PosMin,2,(unsigned char*)&RISLCBParameter.ResistancePositionMin) ;
  EE93CXX_ReadDataFromEeprom(_EER_PosMax,2,(unsigned char*)&RISLCBParameter.ResistancePositionMax) ;
  if( RISLCBParameter.ResistancePositionMin > 0 && RISLCBParameter.ResistancePositionMin < 9999 )
      RISLCBParameter.ResistancePosition = RISLCBParameter.ResistancePositionMin ;
  else
      RISLCBParameter.ResistancePosition = 100 ;
  RIS_SensorFunctionStatus.Full = 0 ;                 // Default All is Disable
  //---------------------------------------------------------------------------------------------
  /* Document 
  #define   _NoSuuprtErp                        0x00
  #define   _IntoErp                            0x01
  #define   _NotIntoErp                         0x02
  */
  RISLCBParameter.EntryToErPMode = _IntoErp ;
  // 
  //--------------------------------------------------------------------------------------------------
  // Reset OK
  LCBSystemProcessStatus.B.sTreadmillOperation = 1 ;
  LCBSystemProcessStatus.B.sResetOK = 1 ;
  //
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SystemTime(void) 
{
  SimulatorSystemTime += 1 ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short LCBSimulator_CalculatorTimeTick( unsigned short last_TimeTick )
{
  unsigned short TempTick ;
  if( SimulatorSystemTime < last_TimeTick )
      TempTick = ( 0xFFFF - last_TimeTick ) + SimulatorSystemTime ;
  else  
      TempTick = SimulatorSystemTime - last_TimeTick ;
  //
  return TempTick ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_Process(unsigned char Show,unsigned char *TypeScreenPtr) 
{
  //  
  unsigned char TempEEdata ;
  unsigned char Btn ;  
  static unsigned long BtnHoldCount = 0 ;
  //unsigned short ShowRPM , ShowIncline;
  static unsigned char CommStatus = 0 ;
  static unsigned char BtnStatus = 0 ;
  unsigned char ShowType = 0 ;  
  static unsigned char dccc ;
  static unsigned short ShowScrollTime ;
  static unsigned char ShowScrollString = 0 ;
  // Add 20180402 Show Parameter Switch
  static unsigned char ShowPrnSW = 0 ;
  //
  if( Show == 1 )
      {
      // ------------ Clear Screen -----------------------------------
                                //12345678901234561234567890123456
      LCMDisplay_show_string(0,0,"                                ") ;
      // -------------------------------------------------------------                     
      dccc = 0  ;
      ShowScrollString = 0 ;
      dccc = LCMDisplay_show_scrollingstring(_TypeScreenBarX,_TypeScreenBarY,_TypeScreenBarLeagth,TypeScreenPtr,dccc ) ;
      if( dccc != 0xFF )
          {
          ShowScrollString = 1 ;
          }
      ShowPrnSW = 0 ;
      }  

  if( ShowScrollString == 1 )
      {
      if( LCBSimulator_CalculatorTimeTick(ShowScrollTime) > 300 )
          {
          ShowScrollTime = SimulatorSystemTime ;
          ShowType = 1 ;
          }
      if( ShowType == 1 )
          {
          dccc = LCMDisplay_show_scrollingstring(_TypeScreenBarX,_TypeScreenBarY,_TypeScreenBarLeagth,TypeScreenPtr,dccc ) ;
          }
      }
  //   
  switch(LCBParameter.LCBSimulatorType) 
      {
      case _LCB1_       : // Modify by Kunlung 20190410
                        if( Show == 1 )
                            {
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _BikeParameterSize-1 ;
                            SelectLimit = _BikeParameterSize-1 ;
                            EE93CXX_ReadDataFromEeprom(_EEPowered,1,&TempEEdata) ;
                            if( TempEEdata != 0x00 )
                                {
                                LCBSystemProcessStatus.B.sDCPlugin = 1 ;
                                }
                            else
                                {
                                LCBSystemProcessStatus.B.sDCPlugin = 0 ;
                                }
                            }
                        // Check machine is Stepper
                        if( LCBParameter.MachineType == _MSetpper_ )
                            {
                            if( LCBSystemProcessStatus.B.sSetMachineType == 1 )
                                {
                                OldSelect = _StepperParameterSize-1 ;
                                SelectLimit = _StepperParameterSize-1 ; 
                                LCBSystemProcessStatus.B.sSetMachineType = 0 ;
                                }
                            LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&StepperLCB[0],0) ;
                            }
                        else // machine is bike/ep
                            LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&BikeLCB[0],0) ;
                        //
                        BtnStatus = 0 ;
                        break ;             
      case _LCBA_       ://Chopper or JIS 1x
      case _LCBA_M_     :
      case _LCB1x_      :
      case _BKLCBA_2026 : // 20260126  
                        if( Show == 1 )
                            {
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _ChopperBikeParameterSize-1 ;
                            SelectLimit = _ChopperBikeParameterSize-1 ;
                            EE93CXX_ReadDataFromEeprom(_EEPowered,1,&TempEEdata) ;
                            if( TempEEdata != 0x00 )
                                {
                                LCBSystemProcessStatus.B.sDCPlugin = 1 ;
                                }
                            else
                                {
                                LCBSystemProcessStatus.B.sDCPlugin = 0 ;
                                }
                            }
                        //LCBMain_SetDCPluginStatus(1) ;
                        //LCBSimulator_EPBikeProcess() ;
                        // Modify By KunLung 20180503 Add Check Machine Type
                        //LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&BikeLCB[0],0) ;
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&ChopperBikeLCB[0],0) ;
                        //
                        BtnStatus = 0 ;
                        break ;        
      case _LCB3_       :
#if 0
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0," Ascent Trainer                 ") ;
                            Show =  0;
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            OldSelect = _AscentTrainerParameterSize-1 ;
                            SelectLimit = _AscentTrainerParameterSize-1 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            SetData = 0 ;
                            OldSetData = 1 ;                            
                            }
                        //LCBMain_SetDCPluginStatus(1) ;
                        LCBSimulator_AscentTrainerProcess(LCBParameter.InclineType) ;
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&AscentTrainerLCB[0],0) ;
                        BtnStatus = 0 ;
                        break ;
#endif
      case _LCB2_       :
      case _ATLCB2_2026 : //20260126  
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0," Ascent Trainer                 ") ;
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show =  0;
                            OldStepRPM = 0 ;
                            OldSelect = _AscentTrainerParameterSize-1 ;
                            SelectLimit = _AscentTrainerParameterSize-1 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            }
                        //LCBMain_SetDCPluginStatus(1) ;
                        LCBSimulator_AscentTrainerProcess(LCBParameter.InclineType) ;
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&AscentTrainerLCB[0],0) ;
                        BtnStatus = 0 ;
                        break ;
      case _ClimbmillR_ :
      case _ClimbmillEN_:
      case _IMPULSE_LCB : // 20250526 Add IMPULSE  
      case _CMLCB_2026  : // 20260126   
                        if( Show == 1 )
                            {
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _ENClimbmillParameterSize-1 ;
                            SelectLimit = _ENClimbmillParameterSize-1 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;
                            }        
                        LCBMain_SetDCPluginStatus(1) ;
                        LCBSimulator_ClimbmillProcess(BtnStatus) ;
                        ShowPrnSW = LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&ENClimbmill[0],ShowPrnSW) ;
                        BtnStatus = 0 ; 
                        break ;
      case _Climbmill_  :
                        if( Show == 1 )
                            {
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _ClimbmillParameterSize-1 ;
                            SelectLimit = _ClimbmillParameterSize-1 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            }        
                        LCBMain_SetDCPluginStatus(1) ;
                        LCBSimulator_ClimbmillProcess(BtnStatus) ;
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&Climbmill[0],0) ;
                        BtnStatus = 0 ;                        
                        break ;
      case _DCI_        :
      case _TopTek_     :
      case _Delta_I     :
      case _Delta_H     :
      case _Delta_I1    :
      case _Delta_I2    :
      case _Delta_I3    :
      case _Delta_I4    :
      case _Liteon      :    
      case _LiteonJHT   ://Add 20210223  
      case _VAVE_A1     ://Add 20210322  
      case _VAVE_A2     ://Add 20210322 
      case _VAVE_A3     ://Add 20210322   
      case _VAVE_F1     ://Add 20210322      
      case _PS_PMSM     ://20260126
      case _PP_PMSM     ://20260126   
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0," Treadmill (C4) M       I       ") ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _TreadmillParameterSize-1 ;
                            SelectLimit = _TreadmillParameterSize-1 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0;
                            }
                        LCBMain_SetDCPluginStatus(1) ;
                        //LCBSimulator_TreadMillESTOPProcess() ;
                        LCBSimulator_TreadMillProcess((LCBParameter.LCBSimulatorType-_DCI_)) ;
                        //-- Add by Kunlung 20161103...
                        if( LCBSystemProcessStatus.B.sTreadMillEStop == 0 ) // IF EStop can't Show Data
                            {
                            LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&TreadmillLCB[0],ShowPrnSW) ;
                            //Add 20250731 No user
                            if( SpecControlStatus.bit.NoUser == 0 )
                                LCBParameter.TreadmillUseStatus = 0xFF ;
                            else
                                LCBParameter.TreadmillUseStatus = 0x55 ;
                            //
                            BtnStatus = 0 ;
                            ShowPrnSW = 0 ;
                            }
                        else
                            {
                            ShowPrnSW = 1 ;
                            }
                        //                       
                        break ;
      case  _Rower      :
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0,"  Rower                         ") ;
                            OldStepRPM = 0 ;
                            if( RISLCBParameter.RPM == 0 )
                                OldStepRPM = 1 ;
                            RIS_MasterStatus = 0x55 ; // For First Show Status
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _RowerParameterSize-1 ;
                            SelectLimit = _RowerParameterSize-1 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;      
                            }         
                        LCBSimulator_RISProcess(BtnStatus,(Parameters*)&RowerLCB[0]) ;
                        BtnStatus = 0 ;
                        break ;                         
      case  _SDrive     :               
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0,"  S-Drive                       ") ;
                            OldStepRPM = 0 ;
                            if( RISLCBParameter.RPM == 0 )
                                OldStepRPM = 1 ;
                            RIS_MasterStatus = 0x55 ; // For First Show Status
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _SDriveParameterSize-1 ;
                            SelectLimit = _SDriveParameterSize-1 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            }   
                        LCBSimulator_RISProcess(BtnStatus,(Parameters*)&SDriveLCB[0]) ;
                        BtnStatus = 0 ;
                        break ;  
                      
      case  _IndoorCycle:
      // 20251215
      case  _Universal  :
      case  _GTM        :
      //  
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0,"  Indoor Cycle                  ") ;
                            OldStepRPM = 0 ;
                            if( RISLCBParameter.RPM == 0 )
                                OldStepRPM = 1 ;
                            RIS_MasterStatus = 0x55 ; // For First Show Status
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _IndoorCycleParameterSize-1 ;
                            SelectLimit = _IndoorCycleParameterSize-1 ; 
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            } 
                        LCBSimulator_RISProcess(BtnStatus,(Parameters*)&IndoorCycleLCB[0]) ;
                        BtnStatus = 0 ;
                        break ;
      case _JISACTM110  :
      case _JISACTM220  : 
      case _JISDCTM     :   
      case _JISDCLCB    :// add 20211201  
      case _JISGMCB110  :// add 20230522 
      case _JISGMCB220  :// add 20230522   
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0," Treadmill (C4) M       I       ") ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _JISTreadmillParameterSize-1 ;
                            SelectLimit = _JISTreadmillParameterSize-1 ;
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0;
                            }
                        LCBMain_SetDCPluginStatus(1) ;
                        //LCBSimulator_TreadMillJISESTOPProcess();
                        LCBSimulator_TreadMillProcess(0xFF) ;
                        if( LCBSystemProcessStatus.B.sTreadMillEStop == 0 ) 
                            {
                            LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&JISTreadmillLCB[0],ShowPrnSW) ;
                            BtnStatus = 0 ;
                            ShowPrnSW = 0 ;
                            }
                        else
                            {
                            ShowPrnSW = 1 ;
                            }
                        //
                        break ;
      case _JISEM_I     :
                        if( Show == 1 )
                            {
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _A50ParameterSize-1 ;
                            SelectLimit = _A50ParameterSize-1 ;
                            }        
                        LCBSimulator_JISInclineProcess() ;
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&JISLCB15[0],0) ;
                        //
                        BtnStatus = 0 ;                           
                        break ;        
      case _JISEM       :
                        if( Show == 1 )
                            {
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _URE50ParameterSize-1 ;
                            SelectLimit = _URE50ParameterSize-1 ;
                            }  
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&JISLCB0C[0],0) ;
                        //20210315
                        BtnStatus = 0 ;  
                        break ;        
      case _JISECB      :
                        if( Show == 1 )
                            {
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _Bike30ParameterSize-1 ;
                            SelectLimit = _Bike30ParameterSize-1 ;
                            }
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&JISLCB12[0],0) ;
                        //
                        BtnStatus = 0 ;        
                        break ;
      case _JISECB_I    :        
                        if( Show == 1 )
                            {
                            OldStepRPM = 0 ;
                            if( LCBParameter.StepRPM == 0 )
                                OldStepRPM = 1 ;
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _A30ParameterSize-1 ;
                            SelectLimit = _A30ParameterSize-1 ;
                            }
                        LCBSimulator_JISA30ECBProcess() ;
                        LCBSimulator_JISInclineProcess() ;
                        LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&JISLCB0B[0],0) ;
                        //
                        BtnStatus = 0 ;        
                        break ;
      case _Athena      :  
                        if( Show == 1 )
                            {
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _AthenaParameterSize-1 ;
                            SelectLimit = _AthenaParameterSize-1 ;                            
                            }
                        LCBSimulator_AthenaProcess() ;
                        ShowPrnSW = LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&AthenaLCB[0],ShowPrnSW) ;
                        //
                        BtnStatus = 0 ;
                        break ; 
// Add 20210413 Upper Body Cycle (UBCM & UBCP)
      case _UBCM        : // follow CxM indoor cycle 
                        if( Show == 1 )
                            {                         
                            OldStepRPM = 0 ;
                            if( RISLCBParameter.RPM == 0 )
                                OldStepRPM = 1 ;
                            RIS_MasterStatus = 0x55 ; // For First Show Status
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _IndoorCycleParameterSize-1 ;
                            SelectLimit = _IndoorCycleParameterSize-1 ; 
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            } 
                        LCBSimulator_RISProcess(BtnStatus,(Parameters*)&IndoorCycleLCB[0]) ;
                        BtnStatus = 0 ; 
                        break ;
      case _UBCP        : // follow Athena indoor cycle
                        if( Show == 1 )
                            {                         
                            OldStepRPM = 0 ;
                            if( RISLCBParameter.RPM == 0 )
                                OldStepRPM = 1 ;
                            RIS_MasterStatus = 0x55 ; // For First Show Status
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _UBCPParameterSize-1 ;
                            SelectLimit = _UBCPParameterSize-1 ; 
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            } 
                        RISLCBParameter.FlywheelRPM = (unsigned short)((float)RISLCBParameter.RPM * 9.77) ;
                        LCBSimulator_RISProcess(BtnStatus,(Parameters*)&UBCPLCB[0]) ;
                        BtnStatus = 0 ;
                        break ;
// Add 20220216 Cxp Update                          
      case  _CxpUpdate  :  
                        if( Show == 1 )
                            {                         //12345678901234561234567890123456
                            //LCMDisplay_show_string(0,0,"  Indoor Cycle                  ") ;
                            OldStepRPM = 0 ;
                            if( RISLCBParameter.RPM == 0 )
                                OldStepRPM = 1 ;
                            RIS_MasterStatus = 0x55 ; // For First Show Status
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _CxpUpdateParameterSize-1 ;
                            SelectLimit = _CxpUpdateParameterSize-1 ; 
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            } 
                        LCBSimulator_RISProcess(BtnStatus,(Parameters*)&CxpUpdateLCB[0]) ;
                        BtnStatus = 0 ;
                        break ;        
//------------------------------------------------------------------------------
      case  _ITC        :
                        if( Show == 1 )
                            {
                            Show = 0 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _ITCParameterSize ;
                            SelectLimit = _ITCParameterSize-1 ;                            
                            }
                        LCBSimulator_ITCProcess() ;
                        ShowPrnSW = LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&ITCLCB[0],ShowPrnSW) ;
                        //
                        BtnStatus = 0 ;        
                        break ;
//------------------------------------------------------------------------------      
// 20221130 Add Support ICR70
      case  _JISICR70LCB:
                        if( Show == 1 )
                            {                         
                            OldStepRPM = 0 ;
                            if( LCBParameter.CrankRPM == 0 )
                                OldStepRPM = 1 ;
                            SetMode = 0 ;
                            Select = 0 ;
                            OldSelect = _ICR70ParameterSize-1 ;
                            SelectLimit = _ICR70ParameterSize-1 ; 
                            SetData = 0 ;
                            OldSetData = 1 ;  
                            Show = 0 ;                            
                            } 
                        
                        ShowPrnSW = LCBSimulator_ParameterProcess(SelectLimit,BtnStatus,(Parameters*)&ICR70LCB[0],ShowPrnSW) ;
                        BtnStatus = 0 ;        
                        break ;
//------------------------------------------------------------------------------                            
      default           :
                        if( Show == 1 )
                            {
                            Show = 0 ;
                            }
                        break ;
      }
  // Check Stop Simulator
  _iButton(Btn) ;
  if( Btn == 0 )
      {
      BtnHoldCount += 1 ;
      if( BtnHoldCount > 200000 )
          {
          BtnHoldCount = 0 ;  
          Show = 2 ;
          }
      }
  else
      {
      if( BtnHoldCount > 100 )
          BtnStatus = 1 ;
      BtnHoldCount = 0 ;
      }
  //----------------------------------------------------------------------------
  // Communication Status LED  
  if( CommStatus != CommControlFlag.Bits.StatusLED )
      {
      CommStatus = CommControlFlag.Bits.StatusLED ;
      if( CommStatus == 1 )
          LCMDisplay_show_one_char(15,1, 0xFF ) ;
      else
          LCMDisplay_show_one_char(15,1,' ') ;
      }
  //
  return Show ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_ClimbmillProcess(unsigned char Button)
{
  //----------------------------------------------------------------------------
  if( LCBParameter.ClimbmillStatus.Bits.SafetyStop == 0 )
      {
      if( CommControlFlag.Bits.ClimbmillStart == 1 )
          {
          CommControlFlag.Bits.ClimbmillStart = 0 ;
          //LCBParameter.ClimbmillBrakePressureValue = 280 ; // add for Climbmil EN                
          if( LCBParameter.ClimbmillStatus.Bits.Start == 0)
              {              
              LCBParameter.ClimbmillStatus.Bits.Start = 1 ;
              }      
          }
      else
          {
          if( CommControlFlag.Bits.ClimbmillStop == 1 )
              {
              CommControlFlag.Bits.ClimbmillStop = 0 ;
              //LCBParameter.ClimbmillBrakePressureValue = 330 ; // add for Climbmil EN             
              if( LCBParameter.ClimbmillStatus.Bits.Start == 1)
                  {                         
                  LCBParameter.ClimbmillStatus.Bits.Start = 0 ;                  
                  }
              //LCBParameter.LimitRpmForResistance = 0 ;
              LCBParameter.StepRPM = 0 ;
              }
          }
      }
  else
      {
      CommControlFlag.Bits.ClimbmillStart = 0 ;
      CommControlFlag.Bits.ClimbmillStop = 0 ;
      LCBParameter.ClimbmillStatus.Bits.Start = 0 ;
      //LCBParameter.LimitRpmForResistance = 0 ;
      LCBParameter.StepRPM = 0 ;
      //LCBParameter.ClimbmillBrakePressureValue = 330 ; // add for Climbmil EN
      }
  //----------------------------------------------------------------------------
  // Control zone , Frame IR , E-STOP1 , E-STOP2 process 
  LCBSimulator_ClimbmillSafetyProcess() ;
  //LCBSimulator_ParameterProcess(SelectLimit,Button,(Parameters*)&Climbmill[0],0) ;
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_ClimbmillSafetyProcess(void) //unsigned char Button)
{
  //unsigned char show = 0 ;
  //
  if( LCBParameter.LCBSimulatorType == _Climbmill_ )
      {
      //SelectLimit = 1 ;
      LCBParameter.ClimbmillExtendStatus.Full = 0 ; // Clear All extend status
      }
  
  //----------------------------------------------------------------------------
  if( LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable == 1 )
      {
      if( LCBParameter.ClimbmillStatus.Bits.SafetyKey == 1 )
          {
          LCBParameter.ClimbmillStatus.Bits.SafetyStop = 1 ;
          }
      }
  //else
  //    LCBParameter.ClimbmillStatus.Bits.SafetyStop = 0 ;
  //
  if( LCBParameter.ClimbmillStatus.Bits.IRSensorEnable == 1 )
      {
      if( LCBParameter.ClimbmillStatus.Bits.IRSensor == 1 )
          {
          LCBParameter.ClimbmillStatus.Bits.IRSensorSafteyStop = 1 ;
          LCBParameter.ClimbmillStatus.Bits.SafetyStop = 1 ;
          }
      }
  else
      LCBParameter.ClimbmillStatus.Bits.IRSensorSafteyStop = 0 ; 
  
  if( LCBParameter.LCBSimulatorType == _ClimbmillEN_ || LCBParameter.LCBSimulatorType == _ClimbmillR_ )
      {
      if( LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitch == 1 )
          {
          LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitchSafetyStop = 1 ; 
          LCBParameter.ClimbmillStatus.Bits.SafetyStop = 1 ;
          }
      
      if( LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitch == 1 )
          {
          LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitchSafetyStop = 1 ;  
          LCBParameter.ClimbmillStatus.Bits.SafetyStop = 1 ;
          } 
      }
  
  // unlock safety stop status
  if( CommControlFlag.Bits.ClimbmillUnlockSafetyKey == 1 )
      {
      CommControlFlag.Bits.ClimbmillUnlockSafetyKey = 0 ;
      // Check Release All safety devices      
      if( LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitch == 0 )
          {
          LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitchSafetyStop = 0 ;           
          }
      
      if( LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitch == 0 )
          {
          LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitchSafetyStop = 0 ;           
          }
      
      if( LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable == 0 && LCBParameter.ClimbmillStatus.Bits.IRSensorEnable == 0)
          {
          if( LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitchSafetyStop == 0 && LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitchSafetyStop == 0 )
              {
              LCBParameter.ClimbmillStatus.Bits.SafetyStop = 0 ;
              }            
          }
      else
          {
          if( LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitchSafetyStop == 0 && LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitchSafetyStop == 0 )
              {
              if( LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable == 1 && LCBParameter.ClimbmillStatus.Bits.IRSensorEnable == 1 \
                  && LCBParameter.ClimbmillStatus.Bits.SafetyKey == 0 && LCBParameter.ClimbmillStatus.Bits.IRSensor == 0 )
                  {
                  LCBParameter.ClimbmillStatus.Bits.IRSensorSafteyStop = 0 ; 
                  LCBParameter.ClimbmillStatus.Bits.SafetyStop = 0 ;
                  }
              else if( LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable == 1 && LCBParameter.ClimbmillStatus.Bits.IRSensorEnable == 0 && LCBParameter.ClimbmillStatus.Bits.SafetyKey == 0)
                  {
                  LCBParameter.ClimbmillStatus.Bits.IRSensorSafteyStop = 0 ;
                  LCBParameter.ClimbmillStatus.Bits.SafetyStop = 0 ;                  
                  }
              else if( LCBParameter.ClimbmillStatus.Bits.SafetyKeyEnable == 0 && LCBParameter.ClimbmillStatus.Bits.IRSensorEnable == 1 && LCBParameter.ClimbmillStatus.Bits.IRSensor == 0)
                  {
                  LCBParameter.ClimbmillStatus.Bits.IRSensorSafteyStop = 0 ;
                  LCBParameter.ClimbmillStatus.Bits.SafetyStop = 0 ;
                  }
              } 
          }
      }
  //
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_TreadMillESTOPProcess(void)
{  
  //----------------------------------------------------------------------------
  //-- Add by Kunlung 20161103...
  // Check Safety Key
  if( iSafetyKey == _SafetyKeyPress )
      {
      SafetyKeyReleaseTime = SimulatorSystemTime ;
      if( LCBSimulator_CalculatorTimeTick(SafetyKeyPressTime) > 50 )
          {            
          SafetyKeyPressTime = SimulatorSystemTime ;  
          if( LCBSystemProcessStatus.B.sTreadMillEStop == 0 )
              {
              // Process Safety Key
              LCBSystemProcessStatus.B.sTreadMillEStop = 1 ;
              Screen_ShowEmergency(1) ;
              // Add by Kunlung 20180402 simulator real E-Stop flowchart
              ErrorCodeStatus.bit.EC02B2 = 1 ; 
              //ErrorCodeStatus.bit.EC01AC = 1 ; // for test
              TMReleaseEstopStatus = 0 ;
              //
              }
          }
      }
  else
      {
      SafetyKeyPressTime = SimulatorSystemTime ;
      if( LCBSimulator_CalculatorTimeTick(SafetyKeyReleaseTime) > 50 )
          {
          SafetyKeyReleaseTime = SimulatorSystemTime ;
          if( LCBSystemProcessStatus.B.sTreadMillEStop == 1 )
              {
              // Process Safety Key Release
              /* Modify by Kunlung 20180402 simulator real E-Stop release flowchart
              LCBSystemProcessStatus.B.sTreadMillEStop = 0 ;  
              Screen_ShowEmergency(0) ;
              */
              if( TMReleaseEstopStatus == _RelaseESTOP )
                  {
                  LCBSystemProcessStatus.B.sTreadMillEStop = 0 ;  
                  ErrorCodeStatus.bit.EC02B2 = 0 ;
                  Screen_ShowEmergency(0) ;  
                  JHTLCBComm_SkipAnyErrorCode(0x02B2) ;
                  }
              }
          }
      }
  //----------------------------------------------------------------------------
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_TreadMillJISESTOPProcess(void)
{  
  //----------------------------------------------------------------------------
  //-- Add by Kunlung 20200318
  // Check Safety Key
  if( iSafetyKey == _SafetyKeyPress )
      {
      SafetyKeyReleaseTime = SimulatorSystemTime ;
      if( LCBSimulator_CalculatorTimeTick(SafetyKeyPressTime) > 50 )
          {            
          SafetyKeyPressTime = SimulatorSystemTime ;  
          if( LCBSystemProcessStatus.B.sTreadMillEStop == 0 )
              {
              // Process Safety Key
              LCBSystemProcessStatus.B.sTreadMillEStop = 1 ;
              Screen_ShowEmergency(1) ;
              ErrorCodeStatus.bit.EC02B2 = 1 ; 
              TMReleaseEstopStatus = 0 ;
              //
              }
          }
      }
  else
      {
      SafetyKeyPressTime = SimulatorSystemTime ;
      if( LCBSimulator_CalculatorTimeTick(SafetyKeyReleaseTime) > 50 )
          {
          SafetyKeyReleaseTime = SimulatorSystemTime ;
          if( LCBSystemProcessStatus.B.sTreadMillEStop == 1 )
              {
              // Process Safety Key Release
              LCBSystemProcessStatus.B.sTreadMillEStop = 0 ;  
              ErrorCodeStatus.bit.EC02B2 = 0 ;
              Screen_ShowEmergency(0) ;  
              JHTLCBComm_SkipAnyErrorCode(0x02B2) ;
              }
          }
      }
  //----------------------------------------------------------------------------
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_TreadMillProcess( unsigned char Type )
{  
#if   0  
  //----------------------------------------------------------------------------
  //-- Add by Kunlung 20161103...
  // Check Safety Key
  if( iSafetyKey == _SafetyKeyPress )
      {
      SafetyKeyReleaseTime = SimulatorSystemTime ;
      if( LCBSimulator_CalculatorTimeTick(SafetyKeyPressTime) > 50 )
          {            
          SafetyKeyPressTime = SimulatorSystemTime ;  
          if( LCBSystemProcessStatus.B.sTreadMillEStop == 0 )
              {
              // Process Safety Key
              LCBSystemProcessStatus.B.sTreadMillEStop = 1 ;
              Screen_ShowEmergency(1) ;
              // Add by Kunlung 20180402 simulator real E-Stop flowchart
              ErrorCodeStatus.bit.EC02B2 = 1 ; 
              //ErrorCodeStatus.bit.EC01AC = 1 ; // for test
              TMReleaseEstopStatus = 0 ;
              //
              }
          }
      }
  else
      {
      SafetyKeyPressTime = SimulatorSystemTime ;
      if( LCBSimulator_CalculatorTimeTick(SafetyKeyReleaseTime) > 50 )
          {
          SafetyKeyReleaseTime = SimulatorSystemTime ;
          if( LCBSystemProcessStatus.B.sTreadMillEStop == 1 )
              {
              // Process Safety Key Release
              /* Modify by Kunlung 20180402 simulator real E-Stop release flowchart
              LCBSystemProcessStatus.B.sTreadMillEStop = 0 ;  
              Screen_ShowEmergency(0) ;
              */
              if( TMReleaseEstopStatus == _RelaseESTOP )
                  {
                  LCBSystemProcessStatus.B.sTreadMillEStop = 0 ;  
                  ErrorCodeStatus.bit.EC02B2 = 0 ;
                  Screen_ShowEmergency(0) ;  
                  JHTLCBComm_SkipAnyErrorCode(0x02B2) ;
                  }
              }
          }
      }
#endif
  if( Type == 0xFF )
      {
      LCBSimulator_TreadMillJISESTOPProcess();        
      LCBSimulator_JISInclineProcess() ;
      }
  else
      {
      LCBSimulator_TreadMillESTOPProcess() ;
      // Maunal Incline
      if( LCBParameter.InclineControl.Bits.ManualUp == 1 )
          {
          LCBParameter.InclineControl.Bits.ManualUp = 0 ;
          LCBParameter.InclineControl.Bits.Manual = 1 ;
          InclineTarget = _TM_Incline_Max_ ;
          }
      else if( LCBParameter.InclineControl.Bits.ManualDown == 1 )
          {
          LCBParameter.InclineControl.Bits.ManualDown = 0 ;
          LCBParameter.InclineControl.Bits.Manual = 1 ;
          InclineTarget = _TM_Incline_Min_ ;
          }
      else if( LCBParameter.InclineControl.Bits.ManualStop == 1 )
          {
          LCBParameter.InclineControl.Bits.ManualStop = 0 ;
          LCBParameter.InclineControl.Bits.Manual = 0 ;
          InclineTarget = LCBParameter.InclineTargetLocation.Position ;
          }
      //----------------------------------------------------------------------------
      // Check UCB incline command is Change
      if( LCBParameter.InclineControl.Bits.Manual == 0 )
          {
          // Incline Action
          if( InclineTarget != LCBParameter.InclineTargetLocation.Position )
              {
              if( LCBParameter.InclineTargetLocation.Position > InclineTarget )
                  InclineAdject = ( LCBParameter.InclineTargetLocation.Position - InclineTarget ) / 10 + 5 ;
              else
                  InclineAdject = ( InclineTarget - LCBParameter.InclineTargetLocation.Position  ) / 10 + 5 ;
              
              InclineTarget = LCBParameter.InclineTargetLocation.Position ;
              }
          }
      // Add by Kunlung 20180503 Manual Incline Process
      else
          {
          InclineAdject = 100 ;
          }
      //----------------------------------------------------------------------------
      // Tracking Incline
      if( LCBSimulator_CalculatorTimeTick(InclineTrackingTime) > 100 )
          {
          InclineTrackingTime = SimulatorSystemTime ;
          if( LCBParameter.InclineLocation.Position != InclineTarget )
              {
              if( LCBParameter.InclineLocation.Position < InclineTarget )
                  {
                  LCBStatus.bit.UpInclineStatus = 1 ;
                  LCBStatus.bit.DnInclineStatus = 0 ;              
                  LCBParameter.InclineLocation.Position += InclineAdject ;
                  if( LCBParameter.InclineLocation.Position > InclineTarget )
                      LCBParameter.InclineLocation.Position = InclineTarget ; 
                  }
              else
                  {
                  LCBStatus.bit.UpInclineStatus = 0 ;
                  LCBStatus.bit.DnInclineStatus = 1 ; 
                  if( LCBParameter.InclineLocation.Position > InclineAdject )
                      {
                      LCBParameter.InclineLocation.Position -= InclineAdject ;
                      if( LCBParameter.InclineLocation.Position < InclineTarget )
                          LCBParameter.InclineLocation.Position = InclineTarget ;  
                      }
                  else                  
                      LCBParameter.InclineLocation.Position = InclineTarget ;
                      
                  }
              }
          else
              {
              //--------------------------------------------------------------------
              //Modify by Kunlung 20180507 Add error code 0x140
              if( LCBParameter.InclineControl.Bits.Manual == 1 )
                  {
                  if( InclineTarget == _TM_Incline_Min_ || InclineTarget == _TM_Incline_Max_ )
                      {
                      ErrorCodeStatus.bit.EB0140 = 1 ;
                      }   
                  LCBParameter.InclineControl.Bits.Manual = 0 ;
                  LCBParameter.InclineTargetLocation.Position = InclineTarget ;
                  }
              else
                  {
                  if( LCBParameter.InclineTargetLocation.Position != 0 )
                      {
                      if( LCBParameter.InclineLocation.Position <= _TM_Incline_Min_ )
                          {
                          InclineTarget = _TM_Incline_Min_ ;
                          ErrorCodeStatus.bit.EB0140 = 1 ;
                          }
                      else if( LCBParameter.InclineLocation.Position >= _TM_Incline_Max_ )
                          {
                          InclineTarget = _TM_Incline_Max_ ;
                          ErrorCodeStatus.bit.EB0140 = 1 ;
                          }
                      LCBParameter.InclineLocation.Position = InclineTarget ;
                      LCBParameter.InclineTargetLocation.Position = InclineTarget ;
                      }
                  }
              //-------------------------------------------------------------------
              if( LCBStatus.bit.UpInclineStatus == 1 )
                  {
                  LCBStatus.bit.UpInclineStatus = 0 ;
                  }
              
              if( LCBStatus.bit.DnInclineStatus == 1 )
                  {
                  LCBStatus.bit.DnInclineStatus = 0 ;
                  }
              }
          }
      }
  //------------------------------------------------------------------------------------
  // Tracking Motor Speed
  if( LCBSystemProcessStatus.B.sTreadmillOperation == 1 )
      {
      if( MotorRPMTarget != LCBParameter.MotorTargetSpeed.RPM  )
          {      
          if( LCBParameter.MotorTargetSpeed.RPM > MotorRPMTarget )
              MotorRPMAdject = ( LCBParameter.MotorTargetSpeed.RPM - MotorRPMTarget ) / 10 + 5 ;
          else
              MotorRPMAdject = ( MotorRPMTarget - LCBParameter.MotorTargetSpeed.RPM  ) / 10 + 5 ;
          MotorRPMTarget = LCBParameter.MotorTargetSpeed.RPM ;
          }
      
      if( LCBSimulator_CalculatorTimeTick(MotorTrackingTime) > 100 )
          {
          MotorTrackingTime = SimulatorSystemTime ;
          if( LCBParameter.MotorSpeed.RPM != MotorRPMTarget )
              {
               
              if( LCBParameter.MotorSpeed.RPM > MotorRPMTarget )
                  {
                  LCBParameter.MotorSpeed.RPM += MotorRPMAdject ;
                  if( LCBParameter.MotorSpeed.RPM > MotorRPMTarget )
                      LCBParameter.MotorSpeed.RPM = MotorRPMTarget ;      
                  }
              else
                  {
                  if( LCBParameter.MotorSpeed.RPM > MotorRPMAdject )
                      {
                      LCBParameter.MotorSpeed.RPM -= MotorRPMAdject ;
                      if( LCBParameter.MotorSpeed.RPM < MotorRPMTarget )
                          LCBParameter.MotorSpeed.RPM = MotorRPMTarget ;    
                      }
                  else
                      LCBParameter.MotorSpeed.RPM = MotorRPMTarget ;
                  }
              }
          //
          if( LCBParameter.MotorSpeed.RPM != 0 )
              {
              LCBStatus.bit.ACPluginStatus = 1 ; // Encoder Status
              LCBStatus.bit.MainMotorStatus = 1 ;
              }
          else
              {
              LCBStatus.bit.ACPluginStatus = 0 ; // Encoder Status
              LCBStatus.bit.MainMotorStatus = 0 ;
              }
          //
          }
      }
  else
      {
      LCBStatus.bit.ACPluginStatus = 0 ;
      MotorRPMTarget = 0 ;
      LCBParameter.MotorSpeed.RPM = 0 ;
      MotorTrackingTime = SimulatorSystemTime ;
      /*
      // Stop Incline
      LCBParameter.InclineControl.Bits.ManualUp = 0 ;
      LCBParameter.InclineControl.Bits.ManualDown = 0 ;
      LCBParameter.InclineControl.Bits.ManualStop = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 0 ;
      InclineTarget = LCBParameter.InclineTargetLocation.Position ;
      //
      */
      }
  //----------------------------------------------------------------------------
 
  //
  return ;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :  1: one Incline motor
*                   2: Two Incline Motor
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_AscentTrainerProcess( unsigned char IncluneType )
{
  static unsigned short AutoCalibrateDelayTime = 0 ;
  // Maunal Incline
  if( LCBParameter.InclineControl.Bits.ManualUp == 1 )
      {
      LCBParameter.InclineControl.Bits.ManualUp = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 1 ;
      InclineTarget = _AT_Incline_Max_ ;
      }
  else if( LCBParameter.InclineControl.Bits.ManualDown == 1 )
      {
      LCBParameter.InclineControl.Bits.ManualDown = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 1 ;
      InclineTarget = _AT_Incline_Min_ ;
      }
  else if( LCBParameter.InclineControl.Bits.ManualStop == 1 )
      {
      LCBParameter.InclineControl.Bits.ManualStop = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 0 ;
      InclineTarget = LCBParameter.InclineTargetLocation.Position ;
#ifdef  _AT_SupportInclineAutoCalibrate      
      //------------------------------------------------------------------------
      if( LCBParameter.InclineControl.Bits.AutoCalibrate == 1 )
          {
          LCBStatus.bit.InitialStatus = 0 ;
          LCBParameter.InclineControl.Bits.AutoCalibrate = 0 ;
          AutoCalibrateDelayTime = 0 ;
          }
      //
#endif      
      }
#ifdef  _AT_SupportInclineAutoCalibrate    
  // Check Auto Cablibrate
  if( LCBParameter.InclineControl.Bits.AutoCalibrate == 1 )
      {
      if( LCBStatus.bit.InitialStatus == 0 )
          {
          AutoCalibrateDelayTime = SimulatorSystemTime ;
          LCBStatus.bit.InitialStatus = 1 ;
          }
      if( LCBSimulator_CalculatorTimeTick(AutoCalibrateDelayTime) > 20000 )
          {
          AutoCalibrateDelayTime = SimulatorSystemTime ;
          LCBStatus.bit.InitialStatus = 0 ;
          LCBParameter.InclineControl.Bits.AutoCalibrate = 0 ;
          }
      }
#endif  
  //----------------------------------------------------------------------------
  if( LCBParameter.InclineControl.Bits.Manual == 0 )
      {
      // Incline Action
      if( InclineTarget != LCBParameter.InclineTargetPercent )
          {
          if( LCBParameter.InclineTargetPercent > InclineTarget )
              InclineAdject = ( LCBParameter.InclineTargetPercent - InclineTarget ) / 10 + 5 ;
          else
              InclineAdject = ( InclineTarget - LCBParameter.InclineTargetPercent  ) / 10 + 5 ;
          
          InclineTarget = LCBParameter.InclineTargetPercent ;
          }
      }
    
  // Tracking Incline
  if( LCBSimulator_CalculatorTimeTick(InclineTrackingTime) > 100 )
      {
      InclineTrackingTime = SimulatorSystemTime ;
      if( LCBParameter.InclinePercent != InclineTarget )
          {
          if( LCBParameter.InclinePercent < InclineTarget )
              {
              LCBStatus.bit.UpInclineStatus = 1 ;
              LCBStatus.bit.DnInclineStatus = 0 ;              
              LCBParameter.InclinePercent += InclineAdject ;
              if( LCBParameter.InclinePercent > InclineTarget )
                  LCBParameter.InclinePercent = InclineTarget ;      
              }
          else
              {
              LCBStatus.bit.UpInclineStatus = 0 ;
              LCBStatus.bit.DnInclineStatus = 1 ; 
              if( LCBParameter.InclinePercent > InclineAdject )
                  {
                  LCBParameter.InclinePercent -= InclineAdject ;
                  if( LCBParameter.InclinePercent < InclineTarget )
                      LCBParameter.InclinePercent = InclineTarget ;    
                  }
              else
                  LCBParameter.InclinePercent = InclineTarget ;
              }
          }
      else
          {
          if( LCBStatus.bit.UpInclineStatus == 1 )
              LCBStatus.bit.UpInclineStatus = 0 ;
          
          if( LCBStatus.bit.DnInclineStatus == 1 )
              LCBStatus.bit.DnInclineStatus = 0 ;
          }
      }
#if 0
  //------------------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // RPM
  Encoder_GetValue(0,250,&LCBParameter.StepRPM,_NORMAL,1) ;
  // Display RPM
  if( OldStepRPM != LCBParameter.StepRPM )
      {
      OldStepRPM = LCBParameter.StepRPM ;
      LCMDisplay_show_word_bcd(11,1,LCBParameter.StepRPM,3) ;
      }
  //----------------------------------------------------------------------------  
#endif
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :  1: one Incline motor
*                   2: Two Incline Motor
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_JISA30ECBProcess( void )
{
  //------------------------------------------------------------------------------------
  // ECB
  if( LCBParameter.ECBActionControl.Bits.Up == 1 )
        {
        LCBParameter.ECBActionControl.Bits.Up = 0 ;
        JISECBTarget = _ECB_Max_ ;
        }
    else if( LCBParameter.ECBActionControl.Bits.Down == 1 )
        {
        LCBParameter.ECBActionControl.Bits.Down = 0 ;
        JISECBTarget = _ECB_Min_ ;
        }
    else if( LCBParameter.ECBActionControl.Bits.Stop == 1 )
        {
        LCBParameter.InclineControl.Bits.ManualStop = 0 ;
        JISECBTarget = LCBParameter.ECBCount ;
        } 
    else  if( LCBParameter.ECBActionControl.Bits.Zero == 1 )
        {
        LCBParameter.ECBActionControl.Bits.Zero = 0 ;
        JISECBTarget = _ECB_Min_ ;
        }      
    else
        {
        if( LCBParameter.ECBPosition != LCBParameter.ECBCount && JISECBTarget != LCBParameter.ECBPosition )
            {
            JISECBTarget = LCBParameter.ECBPosition ;
            }
        }
  // Tracking ECB
  if( LCBSimulator_CalculatorTimeTick(JISECBTrackingTime) > 500 )
      {
      JISECBTrackingTime = SimulatorSystemTime ;
      if( LCBParameter.ECBCount != JISECBTarget )
          {
          if( LCBParameter.ECBCount < JISECBTarget )
              {
              LCBParameter.ECBStatus.Bits.Up = 1 ;
              LCBParameter.ECBStatus.Bits.Down = 0 ; 
              LCBParameter.ECBStatus.Bits.Stop = 0 ;
              LCBParameter.ECBCount += 20 ;
              if( LCBParameter.ECBCount > JISECBTarget )
                  LCBParameter.ECBCount = JISECBTarget ;      
              }
          else
              {
              LCBParameter.ECBStatus.Bits.Up = 0 ;
              LCBParameter.ECBStatus.Bits.Down = 1 ; 
              LCBParameter.ECBStatus.Bits.Stop = 0 ;              
              if( LCBParameter.ECBCount > 20 )
                  {
                  LCBParameter.ECBCount -= 20 ;
                  if( LCBParameter.ECBCount < JISECBTarget )
                      LCBParameter.ECBCount = JISECBTarget ;    
                  }
              else
                  LCBParameter.ECBCount = JISECBTarget ;
              }
          }
      else
          {
          if( LCBParameter.ECBStatus.Bits.Up == 1 )
              {
              LCBParameter.ECBStatus.Bits.Up = 0 ;
              }
          if( LCBParameter.ECBStatus.Bits.Down == 1 )
              {
              LCBParameter.ECBStatus.Bits.Down = 0 ;
              }
          if( LCBParameter.ECBStatus.Bits.Stop == 1 )
              {
              LCBParameter.ECBStatus.Bits.Stop = 0 ;
              }  
          
          if( LCBParameter.ECBCount == _ECB_Min_ )
              LCBParameter.ECBStatus.Bits.Zero = 1 ;
          else
              LCBParameter.ECBStatus.Bits.Zero = 0 ;
          }
      }  
  //
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :  1: one Incline motor
*                   2: Two Incline Motor
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_JISInclineProcess( void )
{
  // Maunal Incline
  if( LCBParameter.InclineControl.Bits.ManualUp == 1 )
      {
      LCBParameter.InclineControl.Bits.ManualUp = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 1 ;
      InclineTarget = _JIS_Incline_Max_ ;
      }
  else if( LCBParameter.InclineControl.Bits.ManualDown == 1 )
      {
      LCBParameter.InclineControl.Bits.ManualDown = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 1 ;
      InclineTarget = _JIS_Incline_Min_ ;
      }
  else if( LCBParameter.InclineControl.Bits.ManualStop == 1 )
      {
      LCBParameter.InclineControl.Bits.ManualStop = 0 ;
      LCBParameter.InclineControl.Bits.Manual = 0 ;
      InclineTarget = LCBParameter.InclineTargetLocation.Position ;
      }
  //----------------------------------------------------------------------------
  // Check UCB incline command is Change
  if( LCBParameter.InclineControl.Bits.Manual == 0 )
      {
      // Incline Action
      if( InclineTarget != LCBParameter.InclineTargetLocation.Position )
          {
          if( LCBParameter.InclineTargetLocation.Position > InclineTarget )
              InclineAdject = ( LCBParameter.InclineTargetLocation.Position - InclineTarget ) / 10 + 5 ;
          else
              InclineAdject = ( InclineTarget - LCBParameter.InclineTargetLocation.Position  ) / 10 + 5 ;
          
          InclineTarget = LCBParameter.InclineTargetLocation.Position ;
          }
      }
  else
      {
      InclineAdject = 20 ;
      }
  //----------------------------------------------------------------------------
  // Tracking Incline
  if( LCBSimulator_CalculatorTimeTick(InclineTrackingTime) > 200 )
      {
      InclineTrackingTime = SimulatorSystemTime ;
      if( LCBParameter.InclineLocation.Position != InclineTarget )
          {
          if( LCBParameter.InclineLocation.Position < InclineTarget )
              {
              LCBStatus.bit.UpInclineStatus = 1 ;
              LCBStatus.bit.DnInclineStatus = 0 ;              
              LCBParameter.InclineLocation.Position += InclineAdject ;
              if( LCBParameter.InclineLocation.Position > InclineTarget )
                  LCBParameter.InclineLocation.Position = InclineTarget ; 
              }
          else
              {
              LCBStatus.bit.UpInclineStatus = 0 ;
              LCBStatus.bit.DnInclineStatus = 1 ; 
              if( LCBParameter.InclineLocation.Position > InclineAdject )
                  {
                  LCBParameter.InclineLocation.Position -= InclineAdject ;
                  if( LCBParameter.InclineLocation.Position < InclineTarget )
                      LCBParameter.InclineLocation.Position = InclineTarget ;  
                  }
              else                  
                  LCBParameter.InclineLocation.Position = InclineTarget ;
                  
              }
          }
      else
          {
          //--------------------------------------------------------------------
          //Modify by Kunlung 20180507 Add error code 0x140
          if( LCBParameter.InclineControl.Bits.Manual == 1 )
              {
              if( InclineTarget == _JIS_Incline_Min_ || InclineTarget == _JIS_Incline_Max_ )
                  {
                  ErrorCodeStatus.bit.EB0140 = 1 ;
                  }   
              LCBParameter.InclineControl.Bits.Manual = 0 ;
              LCBParameter.InclineTargetLocation.Position = InclineTarget ;
              }
          else
              {
              if( LCBParameter.InclineTargetLocation.Position != 0 )
                  {
                  if( LCBParameter.InclineLocation.Position <= _JIS_Incline_Min_ )
                      {
                      InclineTarget = _JIS_Incline_Min_ ;
                      ErrorCodeStatus.bit.EB0140 = 1 ;
                      }
                  else if( LCBParameter.InclineLocation.Position >= _JIS_Incline_Max_ )
                      {
                      InclineTarget = _JIS_Incline_Max_ ;
                      ErrorCodeStatus.bit.EB0140 = 1 ;
                      }
                  LCBParameter.InclineLocation.Position = InclineTarget ;
                  LCBParameter.InclineTargetLocation.Position = InclineTarget ;
                  }
              }
          //-------------------------------------------------------------------
          if( LCBStatus.bit.UpInclineStatus == 1 )
              {
              LCBStatus.bit.UpInclineStatus = 0 ;
              }
          
          if( LCBStatus.bit.DnInclineStatus == 1 )
              {
              LCBStatus.bit.DnInclineStatus = 0 ;
              }
          }
      }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :  
*                   
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_AthenaProcess(void)
{  
  //----------------------------------------------------------------------------
  LCBParameter.FlywheelRPM = (unsigned short)((float)LCBParameter.CrankRPM * 9.77) ;


  
  //LCBParameter.ResistanceLevel = LCBSimulator_MotorPosToResistanceLevel(LCBParameter.MotorPosition.Full) ;
  //----------------------------------------------------------------------------
  return ;
}


//20210602
/*******************************************************************************
* Function Name  : ITC Simulator
* Description    : 
* Input          :  
*                   
* Output         : None
* Return         : None
*******************************************************************************/
#define   _ITCProcessTime       100        // ms
const unsigned char AutoRepData[4] = { 50, 100, 50 , 0 } ;
void LCBSimulator_ITCProcess(void)
{
  static unsigned short OldLoadCellEncoder = 0 ;
  static unsigned long NoMotionTime = 0 ;
  static unsigned short AutoRepTrackTime= 0 ;
  static unsigned char IdxCount = 0 ;
  unsigned short  i,j;
  // Emulation Data
  // Encoder count
  // Auto Rep
  if( LCBParameter.AutoRepTime != 0 )
      {
      if( ITCStatus.bit.E_Lift == 0 )
          AutoRepTrackTime = LCBParameter.AutoRepTime*250 ;
      
      if( LCBSimulator_CalculatorTimeTick(TrackTime1) >= AutoRepTrackTime )
          {
          TrackTime1 = SimulatorSystemTime ;
          LCBParameter.RepCount = AutoRepData[IdxCount] ;
          IdxCount += 1 ;
          if( IdxCount > 3 )
              IdxCount = 0 ;          
          }
      }
  else
      {
      if( AutoRepTrackTime != 0 )
          {
          LCBParameter.RepCount = 0 ;
          AutoRepTrackTime = 0 ;
          IdxCount = 0 ;  
          }      
      TrackTime1 = SimulatorSystemTime ;
      }
  
  LCBParameter.LoadCellEncoder = LCBParameter.RepCount * 10 ;
  LCBParameter.RepData.Member.relative_encoder_counts = LCBParameter.LoadCellEncoder ;
  LCBParameter.RepData.Member.mass_method2_dag = LCBParameter.RepData.Member.mass_method1_dag ;
  // Encoder count to distance mm.
  LCBParameter.LoadCellDisMM = (unsigned short)(((float)LCBParameter.LoadCellEncoder / 1000) * LCBParameter.LoadCellConfig.Cfg.mmPer1000count) ;
  if( LCBSimulator_CalculatorTimeTick(TrackTime) >= _ITCProcessTime )
      {
      TrackTime = SimulatorSystemTime ;
      //
      if( ITCStatus.bit.E_Lift == 0 )
          {
          if( LCBParameter.LoadCellDisMM >= LCBParameter.LoadCellConfig.Cfg.StrokeUp_mm )
              {
              ITCStatus.bit.Quarter_rep = 1 ;
              ITCStatus.bit.E_Lift = 1 ;
              LCBParameter.RepData.Member.start_height_mm = LCBParameter.LoadCellDisMM ;
              }
          }

      //
      if( LCBParameter.LoadCellEncoder > OldLoadCellEncoder )
          {
          i = LCBParameter.LoadCellEncoder - OldLoadCellEncoder ;
          j = (unsigned short)(((float)i / 1000) * LCBParameter.LoadCellConfig.Cfg.mmPer1000count) ;
          if( j > LCBParameter.RepData.Member.peak_height_mm )
              {
              LCBParameter.RepData.Member.peak_height_mm = j ;  
              }
          //
          NoMotionTime = 0 ;
          LCBParameter.RepData.Member.time_down_ms = 0 ;
          LCBParameter.RepData.Member.time_up_ms += _ITCProcessTime ;
          LCBParameter.RepData.Member.burst_time_ms += _ITCProcessTime ;
          LCBParameter.RepData.Member.dist_up_mm += j ;
          LCBParameter.RepData.Member.time_suspend_ms = 0 ;
          }
      else if( LCBParameter.LoadCellEncoder < OldLoadCellEncoder )
          {
          i = OldLoadCellEncoder - LCBParameter.LoadCellEncoder ;
          j = (unsigned short)(((float)i / 1000) * LCBParameter.LoadCellConfig.Cfg.mmPer1000count) ;
          //
          NoMotionTime = 0 ;
          LCBParameter.RepData.Member.time_up_ms = 0 ;
          LCBParameter.RepData.Member.time_down_ms += _ITCProcessTime ;
          LCBParameter.RepData.Member.burst_time_ms += _ITCProcessTime ;
          LCBParameter.RepData.Member.dist_down_mm += j ;
          LCBParameter.RepData.Member.time_suspend_ms = 0 ;
          }
      else
          {
          NoMotionTime += _ITCProcessTime ;
          if( LCBParameter.LoadCellEncoder != 0 )
              {
              if( LCBParameter.RepData.Member.time_up_ms != 0 || LCBParameter.RepData.Member.time_down_ms != 0)
                  {
                  if( LCBParameter.RepData.Member.time_suspend_ms < 60000 )
                      LCBParameter.RepData.Member.time_suspend_ms += _ITCProcessTime ;
                  }
              }
          else
              {
              LCBParameter.RepData.Member.burst_time_ms = 0  ;
              LCBParameter.RepData.Member.dist_up_mm = 0 ;
              LCBParameter.RepData.Member.dist_down_mm = 0 ;
              LCBParameter.RepData.Member.peak_height_mm = 0 ;
              LCBParameter.RepData.Member.start_height_mm = 0 ;
              ITCStatus.bit.E_Lift = 0 ;
              }
          
          if( (NoMotionTime/1000) >= LCBParameter.LoadCellConfig.Cfg.NoMotionTimeout )
              {
              NoMotionTime = 0 ;
              LCBParameter.RepData.Member.time_up_ms = 0 ;
              LCBParameter.RepData.Member.time_down_ms = 0 ;
              LCBParameter.RepData.Member.time_suspend_ms = 0 ;                     // at START [[ this item is last out the port before CRC]]
              LCBParameter.RepData.Member.start_height_mm = 0 ;                     // important for E-lift
              LCBParameter.RepData.Member.peak_height_mm = 0 ;                      // can determine stroke, then average velocity
              LCBParameter.RepData.Member.dist_down_mm = 0 ;
              LCBParameter.RepData.Member.time_dwell_ms = 0 ;
              LCBParameter.RepData.Member.burst_peak_accel = 0 ;                    //during time period burst_time. the peak mm_s2
              LCBParameter.RepData.Member.burst_work_N_dm = 0 ;                     //during time period(above) Newton-decimeter work
              LCBParameter.RepData.Member.burst_time_ms = 0;                        //duration from start to end of the e-lift up
              LCBParameter.RepData.Member.dist_up_mm = 0 ;                          // total accumulated, user may have gone down and up
              LCBParameter.RepData.Member.work_up_N_dm = 0 ;                        // from work and time, console can compute power  
              LCBParameter.RepData.Member.work_down_N_dm = 0 ;                      // 1 Newton-decimeter = 0.1 N-m (1 dm = 0.1m)
              }
          }
      OldLoadCellEncoder = LCBParameter.LoadCellEncoder  ;
      }    
  //----------------------------------------------------------------------------
  return;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :  
*                   
* Output         : None
* Return         : None
*******************************************************************************/
/*
void LCBSimulator_EPBikeProcess(void)
{
  
  //----------------------------------------------------------------------------
  
  
  //----------------------------------------------------------------------------
  // RPM
  Encoder_GetValue(0,250,&LCBParameter.StepRPM,_NORMAL,1) ;
  // Display RPM
  if( OldStepRPM != LCBParameter.StepRPM )
      {
      OldStepRPM = LCBParameter.StepRPM ;
      LCMDisplay_show_word_bcd(11,1,LCBParameter.StepRPM,3) ;
      }
  //----------------------------------------------------------------------------
}
*/

/*******************************************************************************
* Function Name  : LCBMain_GetDCPluginStatus
* Description    : Return DC power plugin Status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_ConsolePowerControl(void)
{
  //
  if(LCBSystemProcessStatus.B.sResetOK == 1 )
      {        
#ifndef _SupportPhoenixConsole
      if( LCBSystemProcessStatus.B.sConsolePowerOn == 0 )
          {
          // Delay Console Power On Control
          if( ConsoleWaitPowerTime < DelayConsolePowerOnTimeLimit )
              ConsoleWaitPowerTime += 1;
          else
              {
              LCBSystemProcessStatus.B.sConsolePowerOn = 1 ;
              oConsolePower(_ON);
              ConsoleWaitPowerTime = 0 ;
              ConsolePowerCycleCount += 1 ;
              RISConsoleWaitPowerTime = 0 ;
              LCBSystemProcessStatus.B.sRISVbatON = 1 ;
              }
          }
      else
          {
          // Check Power OFF Contorl
          if( LCBParameter.PowerOffTime == 0 )
              {
              LCBSystemProcessStatus.B.sConsolePowerOn = 0 ;
              oConsolePower(_OFF);
              DelayConsolePowerOnTimeLimit = 10000 ;
              LCBParameter.PowerOffTime = 10 ;
              DelayConsolePowerOffTimeLimit = (unsigned long)LCBParameter.PowerOffTime*60000 ;
              }
          else
              {
              // for selfPower
              if( LCBSystemProcessStatus.B.sDCPlugin == 0 && LCBParameter.StepRPM < 25 )
                  {                    
                  if( ConsoleWaitPowerTime < DelayConsolePowerOffTimeLimit )
                      ConsoleWaitPowerTime += 1;
                  else
                      {
                      LCBParameter.PowerOffTime = 0 ;
                      ConsoleWaitPowerTime = 0 ;
                      }
                  }
              else // AC plugin
                  {
                  ConsoleWaitPowerTime = 0 ;
                  }
              }
          }        
#else        
      if( LCBSystemProcessStatus.B.sConsolePowerOn == 0 )
          {
          // Delay Console Power On Control
          if( ConsoleWaitPowerTime < DelayConsolePowerOnTimeLimit )
              ConsoleWaitPowerTime += 1;
          else
              {
              #ifndef  _PRODUCT_V2                
              LCBSystemProcessStatus.B.sConsolePowerOn = 1 ;
              oConsolePower(_ON);
              //----------------------------------------------------------------              
              oBATON(_BATONReleas);
              #else
              LCBSystemProcessStatus.B.sConsolePowerOn = 1 ;
              if( CommControlFlag.Bits.RIS_LCBMode == 0 ) 
                  {
                  oConsolePower(_ON);
                  oBATON(_BATOFF) ;
                  }
              else
                  {
                  oConsolePower(_OFF) ;
                  oBATON(_BATON) ;
                  // 20251215  Add check LCB Type is 0x05,0x06 to Output 12V power in molex 8pin connecter        
                  if( LCBParameter.LCBSimulatorType == _Universal || LCBParameter.LCBSimulatorType == _GTM ) 
                      {
                      oConsolePower(_ON) ;
                      }
                  //------------------------------------------------------------
                  }
              #endif
              //----------------------------------------------------------------              
              ConsoleWaitPowerTime = 0 ;
              ConsolePowerCycleCount += 1 ;
              RISConsoleWaitPowerTime = 0 ;
              LCBSystemProcessStatus.B.sRISVbatON = 1 ;
              }
          }
      else
          {
          if( CommControlFlag.Bits.RIS_LCBMode == 0 )  
              {
              // Check Power OFF Contorl
              if( LCBParameter.PowerOffTime == 0 )
                  {
                  LCBSystemProcessStatus.B.sConsolePowerOn = 0 ;
                  oConsolePower(_OFF);
                  #ifndef  _PRODUCT_V2
                  oBATON(_BATONReleas);
                  #else
                  oBATON(_BATOFF);
                  #endif
                  DelayConsolePowerOnTimeLimit = 10000 ;
                  LCBParameter.PowerOffTime = 10 ;
                  DelayConsolePowerOffTimeLimit = (unsigned long)LCBParameter.PowerOffTime*60000 ;
                  }
              else
                  {
                  // for selfPower
                  if( LCBSystemProcessStatus.B.sDCPlugin == 0 && LCBParameter.StepRPM < 25 )
                      {                    
                      if( ConsoleWaitPowerTime < DelayConsolePowerOffTimeLimit )
                          ConsoleWaitPowerTime += 1;
                      else
                          {
                          LCBParameter.PowerOffTime = 0 ;
                          ConsoleWaitPowerTime = 0 ;
                          }
                      }
                  else // AC plugin
                      {
                      ConsoleWaitPowerTime = 0 ;
                      }
                  }
              }
          else
              {
              //----------------------------------------------------------------
              // Check RIS Vbat_ON signal status
              if( LCBSystemProcessStatus.B.sRISVbatONSignalCheck == 1 )
                  {
                  //------------------------------------------------------------
                  #ifndef  _PRODUCT_V2
                      oBATON(_BATONReleas);
                      for( i = 0 ; i < 800 ; i++);                      
                  #endif
                  //------------------------------------------------------------
                  if( iBATON == _BATONPress )
                      {
                      VbatONPressTime += 1 ;
                      if( VbatONPressTime > 20000 )
                          VbatONPressTime = 20000 ;
                      }
                  else
                      VbatONPressTime = 0 ;
                  //------------------------------------------------------------
                  #ifndef  _PRODUCT_V2
                  if( LCBSystemProcessStatus.B.sRISVbatOutputActive == 1 )
                      oBATON(_BATONPress);  
                  #endif
                  //------------------------------------------------------------
                  }
              //----------------------------------------------------------------
              if( LCBSystemProcessStatus.B.sRISVbatON == 1 )
                  {
                  if( LCBSystemProcessStatus.B.sRISVbatOutputActive == 0 )
                      {
                      if( RISConsoleWaitPowerTime < 500 )
                          RISConsoleWaitPowerTime += 1 ;
                      else
                          {
                          LCBSystemProcessStatus.B.sRISVbatOutputActive = 1 ;
                          #ifndef  _PRODUCT_V2
                          oBATON(_BATONPress);
                          #else
                          oBATON(_BATON);  
                          #endif
                          RISConsoleWaitPowerTime = 0 ;
                          LCBSystemProcessStatus.B.sRISVbatONSignalCheck = 1 ;
                          LCBSystemProcessStatus.B.sRISIntoVbatPowerOFFMode = 0 ;
                          } 
                      }
                  else
                      {
                      if( RIS_SensorFunctionStatus.bit.ButtonToPowerOFF == 1 )
                          {
                          if( VbatONPressTime > 10000 )
                              {
                              LCBSystemProcessStatus.B.sRISVbatON = 0 ;
                              LCBSystemProcessStatus.B.sRISVbatOutputActive = 0 ;
                              #ifndef  _PRODUCT_V2
                              oBATON(_BATONReleas);
                              #else
                              oBATON(_BATOFF);
                              #endif
                              RISConsoleWaitPowerTime = 0 ;
                              LCBSystemProcessStatus.B.sRISWaitVbatONButtonRelease = 1 ;
                              WaitVbatONReleaseTime = 0 ;
                              LCBSystemProcessStatus.B.sRISIntoVbatPowerOFFMode = 1 ;
                              RIS_SensorFunctionStatus.Full = 0 ;
                              }
                          } 
                      else // Check PowerOffDelay Time
                          {
                          VbatONPressTime = 0 ;
                          if( LCBSystemProcessStatus.B.sRISVbatOFFDelayTimeCheck == 1 )
                              {
                              // Modify By Kunlung 20170704 Add check RPM
                              if( RISLCBParameter.RPM == 0 )
                                  {
                                  //if( RISLCBParameter.BatteryOffDelayTime == 0 )
                                  if( RISBatteyOFFTime == 0 )
                                      {
                                      LCBSystemProcessStatus.B.sRISVbatON = 0 ;
                                      LCBSystemProcessStatus.B.sRISVbatOutputActive = 0 ;
                                      #ifndef  _PRODUCT_V2
                                      oBATON(_BATONReleas);
                                      #else
                                      oBATON(_BATOFF);
                                      #endif
                                      RISConsoleWaitPowerTime = 0 ;
                                      LCBSystemProcessStatus.B.sRISVbatOFFDelayTimeCheck = 0 ;
                                      LCBSystemProcessStatus.B.sRISWaitVbatONButtonRelease = 1 ;
                                      WaitVbatONReleaseTime = 0 ;
                                      LCBSystemProcessStatus.B.sRISIntoVbatPowerOFFMode = 1 ;
                                      RIS_SensorFunctionStatus.Full = 0 ;
                                      RISBatteyOFFTime = (unsigned long)RISLCBParameter.BatteryOffDelayTime * 60000 ;
                                      }
                                  else
                                      //RISLCBParameter.BatteryOffDelayTime -= 1 ;
                                      RISBatteyOFFTime -= 1 ;
                                  }
                              else
                                  RISBatteyOFFTime = (unsigned long)RISLCBParameter.BatteryOffDelayTime * 60000 ;
                              }
                          }
                      }                  
                  }
              else
                  {
                  if( LCBSystemProcessStatus.B.sRISVbatOutputActive == 1 )
                      {
                      LCBSystemProcessStatus.B.sRISVbatOutputActive = 0 ;
                      #ifndef  _PRODUCT_V2                      
                      oBATON(_BATONReleas);
                      #else
                      oBATON(_BATOFF);
                      #endif                      
                      RISConsoleWaitPowerTime = 0 ;
                      RIS_SensorFunctionStatus.Full = 0 ;
                      }    
                  // Check Vbat wakeup from Button
                  if( LCBSystemProcessStatus.B.sRISWaitVbatONButtonRelease == 1 )
                      {
                      if( VbatONPressTime == 0 )
                          {
                          WaitVbatONReleaseTime += 1 ;
                          if( WaitVbatONReleaseTime > 1000 )
                              {
                              LCBSystemProcessStatus.B.sRISWaitVbatONButtonRelease = 0 ;    
                              WaitVbatONReleaseTime = 0 ;
                              }
                          }
                      else
                          WaitVbatONReleaseTime =  0;
                      }
                  else
                      {
                      if( VbatONPressTime > 10 )
                          {
                          RISConsoleWaitPowerTime = 0 ;
                          LCBSystemProcessStatus.B.sRISVbatON = 1 ;
                          VbatONPressTime = 0 ;
                          }
                      }
                  }
              }
          }
#endif       
      //
      return 1 ;
      } 

  return 0 ;
}

/*******************************************************************************
* Function Name  : LCBMain_SetDCPluginStatus
* Description    : Set DC power plugin Status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBMain_SetDCPluginStatus(unsigned char Mode)
{
  //
  if( Mode == 1 )
      LCBSystemProcessStatus.B.sDCPlugin = 1 ;
  else
      LCBSystemProcessStatus.B.sDCPlugin = 0 ;
  //
  return ;   
}



/*******************************************************************************
* Function Name  : LCBMain_GetDCPluginStatus
* Description    : Return DC power plugin Status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_GetDCPluginStatus(void)
{
  if( LCBSystemProcessStatus.B.sDCPlugin == 1 )
      return 1 ;
  return 0 ;      
}


/*******************************************************************************
* Function Name  : LCBMain_GetDCPluginStatus
* Description    : Return DC power plugin Status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetConsolePowerStatus(unsigned char Status)
{
  if( Status == 1 )
      LCBSystemProcessStatus.B.sConsolePowerOn = 1 ;
  else
      LCBSystemProcessStatus.B.sConsolePowerOn = 0 ;

  return ;
}


/*******************************************************************************
* Function Name  : LCBMain_GetDCPluginStatus
* Description    : Return DC power plugin Status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_GetConsolePowerStatus(void)
{
  if( LCBSystemProcessStatus.B.sConsolePowerOn == 1 )
      return 1 ;
  return 0 ;
}

/*******************************************************************************
* Function Name  : LCBSimulator_SetDelayConsolePowerOnTimeLimit
* Description    : None
* Input          : Delay Time
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetDelayConsolePowerOnTimeLimit(unsigned long Time)
{
  DelayConsolePowerOnTimeLimit = Time ;
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetErPStatus(unsigned char SetStatus)
{
  if( SetStatus != 0 )
      LCBSystemProcessStatus.B.sErPAction = 1 ;
  else
      LCBSystemProcessStatus.B.sErPAction = 0 ;
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_GetErPStatus(void)
{
  if(LCBSystemProcessStatus.B.sErPAction == 1 )
      return 1 ;

  return 0 ;
}


/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetResetOKStatus(unsigned char SetStatus)
{
  if( SetStatus == 1 )
      LCBSystemProcessStatus.B.sResetOK = 1 ;
  else
      LCBSystemProcessStatus.B.sResetOK = 0 ;
}



/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_GetResetOKStatus(void)
{
  if(LCBSystemProcessStatus.B.sResetOK == 1 )
      return 1 ;

  return 0 ;
}


/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetTreadmillOperation(unsigned char SetStatus) 
{
  if( SetStatus == 0 )
      LCBSystemProcessStatus.B.sTreadmillOperation = 0 ;
  else
      LCBSystemProcessStatus.B.sTreadmillOperation = 1 ;
  return ;  
}



/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetsSaveEEPROMStatus(unsigned char SetStatus)
{
  if( SetStatus == 1 )
      LCBSystemProcessStatus.B.sSaveEEPROM = 1 ;
  else
      LCBSystemProcessStatus.B.sSaveEEPROM = 0 ;
}



/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_GetSaveEEPROMStatus(void)
{
  if(LCBSystemProcessStatus.B.sSaveEEPROM == 1 )
      return 1 ;

  return 0 ;
}



/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_SetSerialNumber(unsigned char SetStatus)
{
  if( SetStatus == 1 )
      LCBSystemProcessStatus.B.sSetSerialNumber = 1 ;
  else
      LCBSystemProcessStatus.B.sSetSerialNumber = 0 ;
}



/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_GetSetSerialNumber(void)
{
  if(LCBSystemProcessStatus.B.sSetSerialNumber == 1 )
      return 1 ;

  return 0 ;
}

/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCBSimulator_RISProcess(unsigned char Button, Parameters *ptr )
{
  //
  LCBSimulator_ParameterProcess(SelectLimit,Button,ptr,0) ;
  //-----------------------------------------------------------------------------
  if( CommControlFlag.Bits.RIS_MasterMode != RIS_MasterStatus )
      {
      if( CommControlFlag.Bits.RIS_MasterMode == 0 )
          LCMDisplay_show_one_char( 15, 0 , 'S' ) ;   
      else
          LCMDisplay_show_one_char( 15, 0 , 'M' ) ; 
      RIS_MasterStatus = CommControlFlag.Bits.RIS_MasterMode ;
      }
  //----------------------------------------------------------------------------
  return ;
}




/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_ParameterProcess(unsigned char MenuLimit ,unsigned char Button, Parameters *ptr ,unsigned char ShowStatus)
{
  //
  unsigned char show = ShowStatus ;
  unsigned char px ;
  static unsigned char Pdccc ;
  static unsigned short PScrollTime ;
  static unsigned char PScrollString = 0 ;
  static unsigned short PrvShowData = 0 ; //Add by Kunlung 20200323
  //
  if( SetMode == 0) // Parameter select
      {
      Encoder_GetValue(0,MenuLimit,&Select,_RING,1) ;
      if( OldSelect != Select )
          {
          //--------------------------------------------------------------------
          // 20170619 Add Check Sensor Using 
          if( CommControlFlag.Bits.RIS_LCBMode == 1 ) 
              {
              if( RISLCBParameter.ManchineType == _RIS_IndoorCycle )
                  {
                  if( OldSelect > Select )
                      {
                      if( Select == 2 && RIS_SensorFunctionStatus.bit.Resistanceposition == 0)
                          {
                          Select -= 1 ;
                          }                        
                      if( Select == 1 && RIS_SensorFunctionStatus.bit.Torque == 0)
                          {
                          Select -= 1 ;
                          }
                      }
                  else                        
                      {
                      if( Select == 1 && RIS_SensorFunctionStatus.bit.Torque == 0)
                          {
                          Select += 1 ;
                          }
                      if( Select == 2 && RIS_SensorFunctionStatus.bit.Resistanceposition == 0)
                          {
                          Select += 1 ;
                          }
                      }
                  }
              else if( RISLCBParameter.ManchineType == _RIS_SDrive )
                  {
                  if( OldSelect > Select )
                      {
                      if( Select == 4 && RIS_SensorFunctionStatus.bit.LoadCell2 == 0)
                          {
                          Select -= 1 ;
                          }             
                      if( Select == 3 && RIS_SensorFunctionStatus.bit.LoadCell1 == 0)
                          {
                          Select -= 1 ;
                          }                      
                      }
                  else
                      {
                      if( Select == 3 && RIS_SensorFunctionStatus.bit.LoadCell1 == 0)
                          {
                          Select += 1 ;
                          }
                      if( Select == 4 && RIS_SensorFunctionStatus.bit.LoadCell2 == 0)
                          {
                          Select += 1 ;
                          }
                      }
                  }
              }
          // 20210625
          else if( LCBParameter.LCBSimulatorType == _ITC )
              {
              if( OldSelect > Select )
                  {
                  if( Select == 1 && (LCBParameter.AutoRepTime != 0 ))
                      Select -= 1 ;
                  }
              else
                  {
                  if( Select == 1 && (LCBParameter.AutoRepTime != 0 ))
                      Select += 1 ;
                  }
              }          
          // Protect Memory
          if( Select > MenuLimit )
              {
              Select = MenuLimit ;
              }          
          //--------------------------------------------------------------------  
          OldSelect = Select ;
          show = 1 ;
          }
      //
      if( Button == 1 )
          {
          if( (ptr+OldSelect)->EditAvailable == 1 )  
              {
              SetMode = 1 ;    
              LCMDisplay_show_one_char(0,0,'E') ;
              //---------------------------------------------------------------------------
              //
              if( (ptr+OldSelect)->ShowType ==  _TYPE_BIT )
                  {
                  if( (*((ptr+OldSelect)->ParameterPtr) & (ptr+OldSelect)->BITMASK ) != 0 )
                      SetData = 1 ;
                  else
                      SetData = 0 ;
                  }
              else if( (ptr+OldSelect)->ShowType ==  _TYPE_ERROR ) // 20170727
                  SetData = 0 ; 
              else
                  SetData = *((ptr+OldSelect)->ParameterPtr) ;
              //
              //---------------------------------------------------------------------------
              }
          }
      //
      // 20170728
      //if( (ptr+OldSelect)->EditAvailable == 0 ) 
      //    {
      if( SetData != *((ptr+OldSelect)->ParameterPtr) )
          {
          SetData = *((ptr+OldSelect)->ParameterPtr) ;
          show = 1 ;
          }
      //    }
      //
      }
  else  // Parameter Setting
      {
      //
      Encoder_GetValue((ptr+OldSelect)->MinLimit,(ptr+OldSelect)->MaxLimit,&SetData,(ptr+OldSelect)->AdjMode,(ptr+OldSelect)->Scale) ;
      //
      if( OldSetData != SetData )
          {
          OldSetData = SetData ;
          if( (ptr+OldSelect)->ShowType ==  _TYPE_BIT )
              {
              if( OldSetData == 1 )
                  *((ptr+OldSelect)->ParameterPtr) = *((ptr+OldSelect)->ParameterPtr) | (ptr+OldSelect)->BITMASK ;
              else
                  *((ptr+OldSelect)->ParameterPtr) = *((ptr+OldSelect)->ParameterPtr) & ~(ptr+OldSelect)->BITMASK ;
              }
          else if( (ptr+OldSelect)->ShowType ==  _TYPE_ERROR ) // 20170727
                {
                if( OldSetData > (_ErrorListSize-1) )
                    {
                    OldSetData = 0 ;
                    SetData = 0 ;
                    }
                *((ptr+OldSelect)->ParameterPtr) = ErrorCodeList[OldSetData] ;
                }
          else 
              *((ptr+OldSelect)->ParameterPtr) = SetData ;
          //--------------------------------------------------------------------  
          show = 1 ;
          }
      //
      if( Button == 1 )
          {
          SetMode = 0 ; 
          LCMDisplay_show_one_char(0,0,' ') ;
          SetData = *((ptr+OldSelect)->ParameterPtr) ;
          // 20170727
          if( (ptr+OldSelect)->ShowType ==  _TYPE_ERROR ) // 20170727
              {
              if( *((ptr+OldSelect)->ParameterPtr) != 0 )
                  {                  
                  if( ( *((ptr+OldSelect)->ParameterPtr) & 0x00FF ) >= 0xA0 )
                      JHTLCBComm_SaveErrorCode( _CLASS_C_, *((ptr+OldSelect)->ParameterPtr)) ;
                  else if( ( *((ptr+OldSelect)->ParameterPtr) & 0x00FF ) >= 0x40 )
                      JHTLCBComm_SaveErrorCode( _CLASS_B_, *((ptr+OldSelect)->ParameterPtr)) ;
                  else 
                      JHTLCBComm_SaveErrorCode( _CLASS_A_, *((ptr+OldSelect)->ParameterPtr)) ;
                  }
              }
          //
          }
      }
  // Show data
  if( show == 1 )
      {
      show = 0 ;
      switch((ptr+OldSelect)->ShowType)  
          {
          case  _TYPE_BPM       :
          case  _TYPE_RPM       :
          case  _TYPE_RES_RPM   :
                                Screen_ShowNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;            
                                break ;
          case  _TYPE_RPM1       :
                                Screen_ShowRPMParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr,LCBParameter.SPM) ;            
                                break ;                                
          case  _TYPE_NM        :
                                                                                  //0123456
                                LCMDisplay_show_string((ptr+OldSelect)->ShowAddr,1,"       ") ;
                                px = Screen_ShowNNNDNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;            
                                LCMDisplay_show_one_char( px, 1 , 'N' ) ;
                                LCMDisplay_show_one_char( px+1, 1 , 'm' ) ;
                                break ;            
          case  _TYPE_VOLTAGE   :
                                px = Screen_ShowNNDNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ; 
                                LCMDisplay_show_one_char( px, 1 , 'V' ) ;
                                break ; 
          case  _TYPE_BRAKE     :
                                Screen_ShowBrakePressureValue( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;
                                break ;
          case  _TYPE_SPM       :
                                Screen_ShowSPMParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr,LCBParameter.TargetSPM) ;
                                PrvShowData = LCBParameter.TargetSPM ; //Add by Kunlung 20200323
                                break ;
          case  _TYPE_WATT      :
                                Screen_ShowWattsParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr,LCBParameter.TargetWatts) ;
                                PrvShowData = LCBParameter.TargetWatts ;//Add by Kunlung 20200323
                                break ;
          case  _TYPE_BIT       :
                                if( (*((ptr+OldSelect)->ParameterPtr) & (ptr+OldSelect)->BITMASK ) != 0 )
                                    Screen_ShowOnOffParameter((unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,1) ;
                                else
                                    Screen_ShowOnOffParameter((unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,0) ;                                    
                                break ; 
          case  _TYPE_WATTS     :
                                Screen_ShowNNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;          
                                break ;
          case  _TYPE_ERROR     :// 20170727
                                Screen_ShowHexNNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;          
                                break ;    
          case  _TYPE_MOTOR     :
          case  _TYPE_INCLINE   :// 20170727
                                Screen_ShowNNNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,(*(ptr+OldSelect)->ParameterPtr&0x7FFF)) ;
#if 0
                                if( (*(ptr+OldSelect)->ParameterPtr&0x8000) != 0 )
                                    LCMDisplay_show_one_char( (ptr+OldSelect)->ShowAddr, 1 , '-' ) ;  
                                else
                                    LCMDisplay_show_one_char( (ptr+OldSelect)->ShowAddr, 1 , '+' ) ;                                 
#endif
                                break ;
          case  _TYPE_WATTX100  :
                                Screen_ShowNNNDNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;                      
                                break ;
          case  _TYPE_WATTX10   :
                                Screen_ShowNNNDNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;                      
                                break ;                                
          case  _TYPE_STRING    :
                                Pdccc = 0  ;
                                PScrollString = 0 ;
                                LCMDisplay_show_string(0,1,(unsigned char*)(ptr+OldSelect)->ScrPtr) ;
                                Pdccc = LCMDisplay_show_scrollingstring((ptr+OldSelect)->ShowAddr,1,(14-(ptr+OldSelect)->ShowAddr),(unsigned char*)(ptr+OldSelect)->ParameterPtr,Pdccc ) ;
                                if( Pdccc != 0xFF )
                                    {
                                    PScrollString = 1 ;
                                    }            
                                break ;
          // Athena
          case  _TYPE_MOT       :
                                Screen_ShowNNNNNNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,(ptr+OldSelect)->ParameterPtr) ;
                                break ;
          //
          case  _TYPE_BarX10    :
                                Screen_ShowBarParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,(ptr+OldSelect)->ParameterPtr,10) ;
                                break ;                                
          case  _TYPE_BarX50    :
                                Screen_ShowBarParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,(ptr+OldSelect)->ParameterPtr,50) ;
                                break ;
          //20210602
          case  _TYPE_Plate     :
                                                                                  //01234567
                                LCMDisplay_show_string((ptr+OldSelect)->ShowAddr,1,"        ") ;            
                                px = Screen_ShowNNNDNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;                                  
                                LCMDisplay_show_one_char( px, 1 , 'k' ) ;
                                LCMDisplay_show_one_char( px+1, 1 , 'g' ) ;
                                break ;
          //20210625            
          case  _TYPE_Auto      :      
                                if( *(ptr+OldSelect)->ParameterPtr == 0 )
                                    {
                                    Screen_ShowOnOffParameter((unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,0) ;
                                    }
                                else
                                    {
                                    px = Screen_ShowNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;
                                    LCMDisplay_show_one_char( px, 1 , 's' ) ;
                                    LCMDisplay_show_one_char( px+1, 1 , 'e' ) ;
                                    LCMDisplay_show_one_char( px+2, 1 , 'c' ) ;
                                    LCMDisplay_show_one_char( px+3, 1 , '.' ) ;
                                    }
                                break ;
          case  _TYPE_LC        :                      
          case  _TYPE_RPOS      :
          case  _TYPE_PWM       :
          default               :            
                                Screen_ShowNNNNNParameter( (unsigned char*)(ptr+OldSelect)->ScrPtr,0,1,(ptr+OldSelect)->ShowAddr,*(ptr+OldSelect)->ParameterPtr) ;            
                                break ;           
          }
      //
      //RIS_MasterStatus = ~CommControlFlag.Bits.RIS_MasterMode ;
      }
  //-----------------------------------------------------------------------------------------------------------------------
  // 20190411 show scroll string
  else
      {
      switch((ptr+OldSelect)->ShowType)
          {
          case  _TYPE_STRING    :  
                                if( PScrollString == 1 )
                                    {
                                    if( LCBSimulator_CalculatorTimeTick(PScrollTime) > 300 )
                                        {
                                        PScrollTime = SimulatorSystemTime ;
                                        Pdccc = LCMDisplay_show_scrollingstring((ptr+OldSelect)->ShowAddr,1,(14-(ptr+OldSelect)->ShowAddr),(unsigned char*)(ptr+OldSelect)->ParameterPtr,Pdccc ) ;
                                        }
                                    }            
                                break ;
          // Add By Kunlung 20200323                                 
          case  _TYPE_WATT      :
                                if( PrvShowData != LCBParameter.TargetWatts )
                                    {                                    
                                    if( SetMode == 0 )
                                        *(ptr+OldSelect)->ParameterPtr = LCBParameter.TargetWatts*10 ;
                                    show = 1 ;
                                    }
                                break ;
          case  _TYPE_SPM       :
                                if( PrvShowData != LCBParameter.TargetSPM )
                                    {
                                    if( SetMode == 0 )
                                        *(ptr+OldSelect)->ParameterPtr = LCBParameter.TargetSPM ;
                                    show = 1 ;
                                    }
                                break ;
          //--------------------------------------------------------------------                                
          default               :
                                PScrollString = 0 ;
                                break ;
          }
      }
  //----------------------------------------------------------------------------
  //
  return show ;
}



/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char LCBSimulator_MotorPosToResistanceLevel(unsigned long Mpos)
{
  
  AthenaStepperMotor Temp ;
  unsigned long MaxStep ;
  float person ;
  unsigned long Pos ;
  unsigned char i ;
  Temp.Full = Mpos ;
  //
  MaxStep = LCBSimulator_GetMotorPosMax(Temp.Member.Mode) ;
  //
  if( Temp.Member.Position > MaxStep )
      Temp.Member.Position=MaxStep ;
  
  person = (float)Temp.Member.Position / MaxStep ;
  //
  Pos = (unsigned long)(3200 * person) ;
  
  for( i = 0 ; i < 26 ; i++ )
      {
      if( Pos < ResistanceTable[i] )
          {
          if( i != 0 )
              return (i-1) ;
          else
              return 0 ;
          }
      }
  
  return 25 ;
}


/*******************************************************************************
* Function Name  : 
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned long LCBSimulator_GetMotorPosMax(unsigned char Mode)
{
  // 0x00: full step, 0x01: half step, 0x02: quarter step, 0x03: 1/8 step, 0x04: 1/16 step,
  unsigned char scale  ;
  unsigned long MaxStep ;

  if( Mode == 0 || Mode > 4 )
      scale = 1 ;
  else
      {
      if( Mode == 3 )
          Mode = 4 ;
      else if( Mode == 4 )
          Mode = 8 ;
      
      scale = Mode * 2 ;
      }
  
  MaxStep = 200 * scale ;
  
  return MaxStep ;
}
